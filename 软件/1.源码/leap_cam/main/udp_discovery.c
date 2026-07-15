#include "udp_discovery.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_netif_ip_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "wifi_manager.h"

static const char *TAG = "UDP_DISCOVERY";

#define MAVLINK_V2_MAGIC 0xFD
#define MAVLINK_SYS_ID 2
#define MAVLINK_COMP_ID 100
#define MAVLINK_MSG_ID_HEARTBEAT 0
#define MAVLINK_MSG_ID_STATUSTEXT 253
#define MAVLINK_MSG_ID_HEARTBEAT_CRC_EXTRA 50
#define MAVLINK_MSG_ID_STATUSTEXT_CRC_EXTRA 83
#define MAVLINK_MSG_HEARTBEAT_LEN 9
#define MAVLINK_MSG_STATUSTEXT_LEN 54
#define MAVLINK_MAX_FRAME_LEN 80
#define MAV_TYPE_CAMERA 30
#define MAV_TYPE_GCS 6
#define MAV_AUTOPILOT_INVALID 8
#define MAV_STATE_ACTIVE 4
#define MAV_SEVERITY_INFO 6

static TaskHandle_t s_discovery_task;

static uint16_t mavlink_crc_accumulate(uint8_t data, uint16_t crc)
{
    data ^= (uint8_t)(crc & 0xFF);
    data ^= (uint8_t)(data << 4);
    return ((((uint16_t)data << 8) | (crc >> 8)) ^
            (uint8_t)(data >> 4) ^
            ((uint16_t)data << 3));
}

static uint16_t mavlink_crc_calculate(const uint8_t *buffer, size_t len, uint8_t crc_extra)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = mavlink_crc_accumulate(buffer[i], crc);
    }
    return mavlink_crc_accumulate(crc_extra, crc);
}

static uint16_t mavlink_pack_v2(
    uint8_t *out,
    size_t out_len,
    uint8_t sequence,
    uint32_t msg_id,
    const uint8_t *payload,
    uint8_t payload_len,
    uint8_t crc_extra
)
{
    const size_t frame_len = 10U + payload_len + 2U;
    if (out_len < frame_len) {
        return 0;
    }

    out[0] = MAVLINK_V2_MAGIC;
    out[1] = payload_len;
    out[2] = 0;
    out[3] = 0;
    out[4] = sequence;
    out[5] = MAVLINK_SYS_ID;
    out[6] = MAVLINK_COMP_ID;
    out[7] = (uint8_t)(msg_id & 0xFF);
    out[8] = (uint8_t)((msg_id >> 8) & 0xFF);
    out[9] = (uint8_t)((msg_id >> 16) & 0xFF);
    memcpy(&out[10], payload, payload_len);

    uint16_t crc = mavlink_crc_calculate(&out[1], 9U + payload_len, crc_extra);
    out[10 + payload_len] = (uint8_t)(crc & 0xFF);
    out[11 + payload_len] = (uint8_t)(crc >> 8);
    return frame_len;
}

static uint16_t build_heartbeat(uint8_t *out, size_t out_len, uint8_t sequence)
{
    uint8_t payload[MAVLINK_MSG_HEARTBEAT_LEN] = {0};

    payload[4] = MAV_TYPE_CAMERA;
    payload[5] = MAV_AUTOPILOT_INVALID;
    payload[6] = 0;
    payload[7] = MAV_STATE_ACTIVE;
    payload[8] = 3;

    return mavlink_pack_v2(
        out,
        out_len,
        sequence,
        MAVLINK_MSG_ID_HEARTBEAT,
        payload,
        sizeof(payload),
        MAVLINK_MSG_ID_HEARTBEAT_CRC_EXTRA
    );
}

static uint16_t build_statustext(
    uint8_t *out,
    size_t out_len,
    uint8_t sequence,
    const char *text
)
{
    uint8_t payload[MAVLINK_MSG_STATUSTEXT_LEN] = {0};

    payload[0] = MAV_SEVERITY_INFO;
    strncpy((char *)&payload[1], text, 50);

    return mavlink_pack_v2(
        out,
        out_len,
        sequence,
        MAVLINK_MSG_ID_STATUSTEXT,
        payload,
        sizeof(payload),
        MAVLINK_MSG_ID_STATUSTEXT_CRC_EXTRA
    );
}

static void send_mavlink_packet(
    int sock,
    const struct sockaddr_in *target,
    uint8_t *sequence,
    uint16_t (*builder)(uint8_t *, size_t, uint8_t)
)
{
    uint8_t frame[MAVLINK_MAX_FRAME_LEN];
    uint16_t len = builder(frame, sizeof(frame), *sequence);
    if (len == 0) {
        return;
    }

    int sent = sendto(sock, frame, len, 0, (const struct sockaddr *)target, sizeof(*target));
    if (sent < 0) {
        ESP_LOGW(TAG, "MAVLink discovery send failed: errno %d", errno);
        return;
    }

    (*sequence)++;
}

static void send_statustext(
    int sock,
    const struct sockaddr_in *target,
    uint8_t *sequence,
    const char *text
)
{
    uint8_t frame[MAVLINK_MAX_FRAME_LEN];
    uint16_t len = build_statustext(frame, sizeof(frame), *sequence, text);
    if (len == 0) {
        return;
    }

    int sent = sendto(sock, frame, len, 0, (const struct sockaddr *)target, sizeof(*target));
    if (sent < 0) {
        ESP_LOGW(TAG, "MAVLink statustext send failed: errno %d", errno);
        return;
    }

    (*sequence)++;
}

static bool is_gcs_heartbeat(const uint8_t *data, int len)
{
    uint8_t payload_len = 0;
    uint32_t msg_id = 0;
    const uint8_t *payload = NULL;

    if (len >= 8 && data[0] == 0xFE) {
        payload_len = data[1];
        if (len < 6 + payload_len + 2) {
            return false;
        }

        msg_id = data[5];
        payload = &data[6];
    } else if (len >= 12 && data[0] == MAVLINK_V2_MAGIC) {
        payload_len = data[1];
        uint8_t signature_len = (data[2] & 0x01) ? 13 : 0;
        if (len < 10 + payload_len + 2 + signature_len) {
            return false;
        }

        msg_id = (uint32_t)data[7] |
                 ((uint32_t)data[8] << 8) |
                 ((uint32_t)data[9] << 16);
        payload = &data[10];
    } else {
        return false;
    }

    return msg_id == MAVLINK_MSG_ID_HEARTBEAT &&
           payload_len >= MAVLINK_MSG_HEARTBEAT_LEN &&
           payload[4] == MAV_TYPE_GCS;
}

static void discovery_task(void *arg)
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket: errno %d", errno);
        s_discovery_task = NULL;
        vTaskDelete(NULL);
        return;
    }

    int reuse = 1;
    int broadcast_enable = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

    struct sockaddr_in listen_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(UDP_DISCOVERY_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    if (bind(sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        ESP_LOGE(TAG, "UDP discovery bind failed: errno %d", errno);
        close(sock);
        s_discovery_task = NULL;
        vTaskDelete(NULL);
        return;
    }

    struct timeval recv_timeout = {
        .tv_sec = 0,
        .tv_usec = 20000,
    };
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

    struct sockaddr_in target_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(UDP_DISCOVERY_PORT),
        .sin_addr.s_addr = inet_addr("255.255.255.255"),
    };

    bool connected = false;
    uint8_t sequence = 0;
    uint32_t loop_counter = 0;
    uint8_t rx_buf[256];

    ESP_LOGI(TAG, "MAVLink discovery broadcasting on UDP port %d", UDP_DISCOVERY_PORT);

    while (true) {
        struct sockaddr_in source_addr;
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(
            sock,
            rx_buf,
            sizeof(rx_buf),
            0,
            (struct sockaddr *)&source_addr,
            &socklen
        );

        if (len > 0 && is_gcs_heartbeat(rx_buf, len)) {
            if (!connected || target_addr.sin_addr.s_addr != source_addr.sin_addr.s_addr) {
                target_addr = source_addr;
                target_addr.sin_port = htons(UDP_DISCOVERY_PORT);
                connected = true;
                ESP_LOGI(TAG, "MAVLink discovery target locked: %s:%d",
                         inet_ntoa(target_addr.sin_addr),
                         ntohs(target_addr.sin_port));
            }
        }

        loop_counter++;
        if (loop_counter % 50 == 0) {
            char text[50] = {0};
            esp_ip4_addr_t ip = wifi_manager_get_ip();

            send_mavlink_packet(sock, &target_addr, &sequence, build_heartbeat);

            snprintf(text, sizeof(text), "DEVICE_NAME:%s", wifi_manager_get_device_name());
            send_statustext(sock, &target_addr, &sequence, text);

            snprintf(text, sizeof(text), "DEVICE_IP:" IPSTR, IP2STR(&ip));
            send_statustext(sock, &target_addr, &sequence, text);

            send_statustext(sock, &target_addr, &sequence, "DEVICE_TYPE:maturo_esp32s3_cam");
            send_statustext(sock, &target_addr, &sequence, "HTTP_PORT:80");
            send_statustext(sock, &target_addr, &sequence, "STREAM_PORT:81");
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

esp_err_t udp_discovery_start(void)
{
    if (s_discovery_task) {
        return ESP_OK;
    }

    BaseType_t ok = xTaskCreate(
        discovery_task,
        "udp_discovery",
        4096,
        NULL,
        4,
        &s_discovery_task
    );

    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

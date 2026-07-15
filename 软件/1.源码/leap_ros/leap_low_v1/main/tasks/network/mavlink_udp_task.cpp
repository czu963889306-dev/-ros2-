#include "system_globals.h"
#include "mavlink_common.h"

#include "arpa/inet.h"
#include "lwip/sockets.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MAVLINK_PORT 14550
#define UDP_RX_BUFFER_SIZE 32768

struct UdpMavlinkTransportContext {
    int sock;
    const struct sockaddr_in *target_addr;
};

static void WriteUdpMavlinkBytes(const uint8_t *data, size_t len, void *ctx) {
    UdpMavlinkTransportContext *udp = static_cast<UdpMavlinkTransportContext *>(ctx);
    if (udp == nullptr || udp->sock < 0 || udp->target_addr == nullptr || data == nullptr || len == 0) {
        return;
    }

    sendto(
        udp->sock,
        data,
        len,
        0,
        reinterpret_cast<const struct sockaddr *>(udp->target_addr),
        sizeof(*udp->target_addr));
}

void mavlink_udp_task(void *pvParameters) {
    static char rx_buffer[UDP_RX_BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t socklen = sizeof(client_addr);

    int sock_mav = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    struct sockaddr_in mav_server_addr = {};
    mav_server_addr.sin_family = AF_INET;
    mav_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    mav_server_addr.sin_port = htons(MAVLINK_PORT);
    bind(sock_mav, reinterpret_cast<struct sockaddr *>(&mav_server_addr), sizeof(mav_server_addr));

    int broadcast_enable = 1;
    setsockopt(sock_mav, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));
    fcntl(sock_mav, F_SETFL, fcntl(sock_mav, F_GETFL, 0) | O_NONBLOCK);

    struct sockaddr_in mav_target_addr = {};
    mav_target_addr.sin_family = AF_INET;
    mav_target_addr.sin_port = htons(MAVLINK_PORT);
    mav_target_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    UdpMavlinkTransportContext udp_ctx = {
        .sock = sock_mav,
        .target_addr = &mav_target_addr,
    };

    mavlink_message_t rx_msg = {};
    mavlink_status_t udp_status = {};
    MavlinkPublishState publish_state = {};

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20);

    while (true) {
        const bool udp_enabled = g_wifi_comm_mode == WifiCommMode::kMavlinkUdp;
        MavlinkTransport transports[1] = {};
        size_t transport_count = 0;

        if (udp_enabled) {
            transports[transport_count++] = {
                .write = WriteUdpMavlinkBytes,
                .ctx = &udp_ctx,
            };
        }

        if (udp_enabled) {
            socklen = sizeof(client_addr);
            const int len_mav = recvfrom(
                sock_mav,
                rx_buffer,
                sizeof(rx_buffer),
                0,
                reinterpret_cast<struct sockaddr *>(&client_addr),
                &socklen);
            if (len_mav > 0) {
                if (len_mav >= static_cast<int>(sizeof(rx_buffer))) {
                    printf("[MAVLINK_UDP] received packet too large or truncated, len=%d\n", len_mav);
                }
                for (int i = 0; i < len_mav; ++i) {
                    if (mavlink_parse_char(MAVLINK_COMM_0, rx_buffer[i], &rx_msg, &udp_status)) {
                        mav_target_addr = client_addr;
                        MavlinkTransport udp_reply = {
                            .write = WriteUdpMavlinkBytes,
                            .ctx = &udp_ctx,
                        };
                        mavlink_common_handle_message(
                            &rx_msg,
                            &udp_reply,
                            MavlinkTransportSource::kUdp);
                    }
                }
            }
        }

        mavlink_common_publish(transports, transport_count, &publish_state);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

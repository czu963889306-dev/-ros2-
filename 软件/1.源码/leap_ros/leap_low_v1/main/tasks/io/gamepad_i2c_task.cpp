#include "system_globals.h"

#include "board.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "i2c_bus_lock.h"
#include "msg/gamepad_msg.h"

namespace {

constexpr char kTag[] = "GAMEPAD_I2C";
constexpr uint8_t kGamepadI2cAddress = 0x50;
constexpr uint8_t kExpectedHeader1 = 0x56;
constexpr uint8_t kExpectedHeader2 = 0xAB;
constexpr uint8_t kExpectedTail = 0xCF;
constexpr TickType_t kGamepadPollPeriod = pdMS_TO_TICKS(50);
constexpr uint8_t kGamepadPollCommand[] = {0x55, 0xBB};

typedef struct {
    uint8_t header1;
    uint8_t header2;
    uint8_t command;
    uint8_t address;
    uint8_t left_stick_btn;
    uint8_t right_stick_btn;
    uint8_t buttons;
    int8_t left_x;
    int8_t left_y;
    int8_t right_x;
    int8_t right_y;
    uint8_t reserved[3];
    uint8_t checksum;
    uint8_t tail;
} __attribute__((packed)) GamepadData_t;

static_assert(sizeof(GamepadData_t) == 16, "GamepadData_t size must be 16 bytes");

esp_err_t WriteGamepadPollCommand() {
    if (!shared_i2c_bus_lock_take(pdMS_TO_TICKS(100))) {
        return ESP_ERR_TIMEOUT;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (kGamepadI2cAddress << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, const_cast<uint8_t*>(kGamepadPollCommand), sizeof(kGamepadPollCommand), true);
    i2c_master_stop(cmd);

    const esp_err_t ret = i2c_master_cmd_begin(imu.GetPort(), cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    shared_i2c_bus_lock_give();
    return ret;
}

esp_err_t ReadGamepadFrame(GamepadData_t* frame) {
    if (frame == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!shared_i2c_bus_lock_take(pdMS_TO_TICKS(100))) {
        return ESP_ERR_TIMEOUT;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (kGamepadI2cAddress << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, reinterpret_cast<uint8_t*>(frame), sizeof(*frame) - 1, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, reinterpret_cast<uint8_t*>(frame) + sizeof(*frame) - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    const esp_err_t ret = i2c_master_cmd_begin(imu.GetPort(), cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    shared_i2c_bus_lock_give();
    return ret;
}

bool HasValidMarkers(const GamepadData_t& frame) {
    return frame.header1 == kExpectedHeader1 &&
           frame.header2 == kExpectedHeader2 &&
           frame.tail == kExpectedTail;
}

uint8_t CalculateChecksum(const GamepadData_t& frame) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&frame);
    uint8_t sum = 0;
    for (int i = 2; i <= 13; ++i) {
        sum = static_cast<uint8_t>(sum + bytes[i]);
    }
    return sum;
}

bool HasValidChecksum(const GamepadData_t& frame) {
    return CalculateChecksum(frame) == frame.checksum;
}

uint16_t DecodeButtonMask(const GamepadData_t& frame) {
    return static_cast<uint16_t>((frame.buttons & 0xFE) | (frame.right_stick_btn & 0x01));
}

GamepadMsg MakeGamepadMsg(const GamepadData_t& frame, bool connected) {
    GamepadMsg msg = {};
    msg.left_x = frame.left_x;
    msg.left_y = frame.left_y;
    msg.right_x = frame.right_x;
    msg.right_y = frame.right_y;
    msg.left_stick_btn = frame.left_stick_btn;
    msg.right_stick_btn = frame.right_stick_btn;
    msg.buttons = frame.buttons;
    msg.button_mask = DecodeButtonMask(frame);
    msg.update_ms = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
    msg.connected = connected ? 1 : 0;
    return msg;
}

void PublishDisconnected(void) {
    if (q_gamepad_state == nullptr) {
        return;
    }

    GamepadMsg msg = {};
    if (xQueuePeek(q_gamepad_state, &msg, 0) != pdTRUE) {
        msg = {};
    }
    msg.connected = 0;
    msg.update_ms = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
    xQueueOverwrite(q_gamepad_state, &msg);
}

void PublishGamepadState(const GamepadData_t& frame) {
    if (q_gamepad_state == nullptr) {
        return;
    }

    const GamepadMsg msg = MakeGamepadMsg(frame, true);
    xQueueOverwrite(q_gamepad_state, &msg);
}

void LogFrame(const GamepadData_t& frame) {
    ESP_LOGI(
        kTag,
        "cmd=0x%02X addr=0x%02X left_btn=0x%02X right_btn=0x%02X buttons=0x%02X lx=%d ly=%d rx=%d ry=%d mask=0x%04X",
        frame.command,
        frame.address,
        frame.left_stick_btn,
        frame.right_stick_btn,
        frame.buttons,
        static_cast<int>(frame.left_x),
        static_cast<int>(frame.left_y),
        static_cast<int>(frame.right_x),
        static_cast<int>(frame.right_y),
        DecodeButtonMask(frame));
}

}  // namespace

void gamepad_i2c_task(void* p) {
    GamepadData_t frame = {};
    TickType_t last_wake_tick = xTaskGetTickCount();

    while (1) {
        const esp_err_t write_ret = WriteGamepadPollCommand();
        if (write_ret != ESP_OK) {
            // ESP_LOGW(kTag, "I2C write to 0x%02X failed: %s", kGamepadI2cAddress, esp_err_to_name(write_ret));
            PublishDisconnected();
        } else {
            const esp_err_t read_ret = ReadGamepadFrame(&frame);
            if (read_ret != ESP_OK) {
                ESP_LOGW(kTag, "I2C read from 0x%02X failed: %s", kGamepadI2cAddress, esp_err_to_name(read_ret));
                PublishDisconnected();
            } else if (!HasValidMarkers(frame)) {
                ESP_LOGW(
                    kTag,
                    "Invalid frame markers: header=0x%02X 0x%02X tail=0x%02X",
                    frame.header1,
                    frame.header2,
                    frame.tail);
                esp_log_buffer_hex(kTag, &frame, sizeof(frame));
                PublishDisconnected();
            } else if (!HasValidChecksum(frame)) {
                ESP_LOGW(kTag, "Invalid checksum: got=0x%02X expected=0x%02X", frame.checksum, CalculateChecksum(frame));
                esp_log_buffer_hex(kTag, &frame, sizeof(frame));
                PublishDisconnected();
            } else {
                LogFrame(frame);
                PublishGamepadState(frame);
            }
        }
        vTaskDelayUntil(&last_wake_tick, kGamepadPollPeriod);
    }
}

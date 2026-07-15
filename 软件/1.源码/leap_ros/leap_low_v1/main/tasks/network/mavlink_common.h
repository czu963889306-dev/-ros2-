#pragma once

#include <stddef.h>
#include <stdint.h>

#include "my_robot/mavlink.h"

enum class MavlinkTransportSource : uint8_t {
    kUdp = 0,
};

struct MavlinkTransport {
    void (*write)(const uint8_t *data, size_t len, void *ctx);
    void *ctx;
};

struct MavlinkPublishState {
    uint32_t loop_counter;
    bool component_info_sent;
};

void mavlink_common_handle_message(
    const mavlink_message_t *msg,
    const MavlinkTransport *reply_transport,
    MavlinkTransportSource source);

void mavlink_common_publish(
    const MavlinkTransport *transports,
    size_t transport_count,
    MavlinkPublishState *state);

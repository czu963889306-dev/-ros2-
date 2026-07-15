#pragma once
// MESSAGE SET_RGB_LED PACKING

#define MAVLINK_MSG_ID_SET_RGB_LED 150


typedef struct __mavlink_set_rgb_led_t {
 uint8_t r; /*<  Red component (0-255)*/
 uint8_t g; /*<  Green component (0-255)*/
 uint8_t b; /*<  Blue component (0-255)*/
} mavlink_set_rgb_led_t;

#define MAVLINK_MSG_ID_SET_RGB_LED_LEN 3
#define MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN 3
#define MAVLINK_MSG_ID_150_LEN 3
#define MAVLINK_MSG_ID_150_MIN_LEN 3

#define MAVLINK_MSG_ID_SET_RGB_LED_CRC 93
#define MAVLINK_MSG_ID_150_CRC 93



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_SET_RGB_LED { \
    150, \
    "SET_RGB_LED", \
    3, \
    {  { "r", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_set_rgb_led_t, r) }, \
         { "g", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_set_rgb_led_t, g) }, \
         { "b", NULL, MAVLINK_TYPE_UINT8_T, 0, 2, offsetof(mavlink_set_rgb_led_t, b) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_SET_RGB_LED { \
    "SET_RGB_LED", \
    3, \
    {  { "r", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_set_rgb_led_t, r) }, \
         { "g", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_set_rgb_led_t, g) }, \
         { "b", NULL, MAVLINK_TYPE_UINT8_T, 0, 2, offsetof(mavlink_set_rgb_led_t, b) }, \
         } \
}
#endif

/**
 * @brief Pack a set_rgb_led message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param r  Red component (0-255)
 * @param g  Green component (0-255)
 * @param b  Blue component (0-255)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_set_rgb_led_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t r, uint8_t g, uint8_t b)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SET_RGB_LED_LEN];
    _mav_put_uint8_t(buf, 0, r);
    _mav_put_uint8_t(buf, 1, g);
    _mav_put_uint8_t(buf, 2, b);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#else
    mavlink_set_rgb_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SET_RGB_LED;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
}

/**
 * @brief Pack a set_rgb_led message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param r  Red component (0-255)
 * @param g  Green component (0-255)
 * @param b  Blue component (0-255)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_set_rgb_led_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint8_t r, uint8_t g, uint8_t b)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SET_RGB_LED_LEN];
    _mav_put_uint8_t(buf, 0, r);
    _mav_put_uint8_t(buf, 1, g);
    _mav_put_uint8_t(buf, 2, b);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#else
    mavlink_set_rgb_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SET_RGB_LED;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#endif
}

/**
 * @brief Pack a set_rgb_led message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param r  Red component (0-255)
 * @param g  Green component (0-255)
 * @param b  Blue component (0-255)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_set_rgb_led_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t r,uint8_t g,uint8_t b)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SET_RGB_LED_LEN];
    _mav_put_uint8_t(buf, 0, r);
    _mav_put_uint8_t(buf, 1, g);
    _mav_put_uint8_t(buf, 2, b);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#else
    mavlink_set_rgb_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SET_RGB_LED;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
}

/**
 * @brief Encode a set_rgb_led struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param set_rgb_led C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_set_rgb_led_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_set_rgb_led_t* set_rgb_led)
{
    return mavlink_msg_set_rgb_led_pack(system_id, component_id, msg, set_rgb_led->r, set_rgb_led->g, set_rgb_led->b);
}

/**
 * @brief Encode a set_rgb_led struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param set_rgb_led C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_set_rgb_led_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_set_rgb_led_t* set_rgb_led)
{
    return mavlink_msg_set_rgb_led_pack_chan(system_id, component_id, chan, msg, set_rgb_led->r, set_rgb_led->g, set_rgb_led->b);
}

/**
 * @brief Encode a set_rgb_led struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param set_rgb_led C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_set_rgb_led_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_set_rgb_led_t* set_rgb_led)
{
    return mavlink_msg_set_rgb_led_pack_status(system_id, component_id, _status, msg,  set_rgb_led->r, set_rgb_led->g, set_rgb_led->b);
}

/**
 * @brief Send a set_rgb_led message
 * @param chan MAVLink channel to send the message
 *
 * @param r  Red component (0-255)
 * @param g  Green component (0-255)
 * @param b  Blue component (0-255)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_set_rgb_led_send(mavlink_channel_t chan, uint8_t r, uint8_t g, uint8_t b)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SET_RGB_LED_LEN];
    _mav_put_uint8_t(buf, 0, r);
    _mav_put_uint8_t(buf, 1, g);
    _mav_put_uint8_t(buf, 2, b);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_RGB_LED, buf, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
#else
    mavlink_set_rgb_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_RGB_LED, (const char *)&packet, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
#endif
}

/**
 * @brief Send a set_rgb_led message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_set_rgb_led_send_struct(mavlink_channel_t chan, const mavlink_set_rgb_led_t* set_rgb_led)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_set_rgb_led_send(chan, set_rgb_led->r, set_rgb_led->g, set_rgb_led->b);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_RGB_LED, (const char *)set_rgb_led, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
#endif
}

#if MAVLINK_MSG_ID_SET_RGB_LED_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_set_rgb_led_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t r, uint8_t g, uint8_t b)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, r);
    _mav_put_uint8_t(buf, 1, g);
    _mav_put_uint8_t(buf, 2, b);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_RGB_LED, buf, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
#else
    mavlink_set_rgb_led_t *packet = (mavlink_set_rgb_led_t *)msgbuf;
    packet->r = r;
    packet->g = g;
    packet->b = b;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_RGB_LED, (const char *)packet, MAVLINK_MSG_ID_SET_RGB_LED_MIN_LEN, MAVLINK_MSG_ID_SET_RGB_LED_LEN, MAVLINK_MSG_ID_SET_RGB_LED_CRC);
#endif
}
#endif

#endif

// MESSAGE SET_RGB_LED UNPACKING


/**
 * @brief Get field r from set_rgb_led message
 *
 * @return  Red component (0-255)
 */
static inline uint8_t mavlink_msg_set_rgb_led_get_r(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field g from set_rgb_led message
 *
 * @return  Green component (0-255)
 */
static inline uint8_t mavlink_msg_set_rgb_led_get_g(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  1);
}

/**
 * @brief Get field b from set_rgb_led message
 *
 * @return  Blue component (0-255)
 */
static inline uint8_t mavlink_msg_set_rgb_led_get_b(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  2);
}

/**
 * @brief Decode a set_rgb_led message into a struct
 *
 * @param msg The message to decode
 * @param set_rgb_led C-struct to decode the message contents into
 */
static inline void mavlink_msg_set_rgb_led_decode(const mavlink_message_t* msg, mavlink_set_rgb_led_t* set_rgb_led)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    set_rgb_led->r = mavlink_msg_set_rgb_led_get_r(msg);
    set_rgb_led->g = mavlink_msg_set_rgb_led_get_g(msg);
    set_rgb_led->b = mavlink_msg_set_rgb_led_get_b(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_SET_RGB_LED_LEN? msg->len : MAVLINK_MSG_ID_SET_RGB_LED_LEN;
        memset(set_rgb_led, 0, MAVLINK_MSG_ID_SET_RGB_LED_LEN);
    memcpy(set_rgb_led, _MAV_PAYLOAD(msg), len);
#endif
}

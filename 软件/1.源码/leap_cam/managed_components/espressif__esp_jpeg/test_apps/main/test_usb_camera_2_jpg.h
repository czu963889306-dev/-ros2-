/*
Raw data from Logitech C170 USB camera was reconstructed to usb_camera_2.jpg
It was converted to RGB888 array with jpg_to_rgb888_hex.py
*/

// JPEG encoded frame 160x120, 1384 bytes, has broken 0xFFFF marker
extern const unsigned char camera_2_jpg[] asm("_binary_usb_camera_2_jpg_start");

extern char _binary_usb_camera_2_jpg_start;
extern char _binary_usb_camera_2_jpg_end;
// Must be defined as macro because extern variables are not known at compile time (but at link time)
#define camera_2_jpg_len (&_binary_usb_camera_2_jpg_end - &_binary_usb_camera_2_jpg_start)

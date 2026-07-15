/*
Raw data from Logitech C270 USB camera was reconstructed to usb_camera.jpg
It was converted to RGB888 array with jpg_to_rgb888_hex.py
*/

// JPEG encoded frame 160x120, 2632 bytes, no huffman tables, double block size (16x8 pixels)
extern const unsigned char jpeg_no_huffman[] asm("_binary_usb_camera_jpg_start");

extern char _binary_usb_camera_jpg_start;
extern char _binary_usb_camera_jpg_end;
// Must be defined as macro because extern variables are not known at compile time (but at link time)
#define jpeg_no_huffman_len (&_binary_usb_camera_jpg_end - &_binary_usb_camera_jpg_start)

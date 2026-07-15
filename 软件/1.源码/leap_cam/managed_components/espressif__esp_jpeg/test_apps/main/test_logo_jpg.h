// JPEG encoded image 46x46, 7561 bytes
extern const unsigned char logo_jpg[] asm("_binary_logo_jpg_start");

extern char _binary_logo_jpg_start;
extern char _binary_logo_jpg_end;
// Must be defined as macro because extern variables are not known at compile time (but at link time)
#define logo_jpg_len (&_binary_logo_jpg_end - &_binary_logo_jpg_start)

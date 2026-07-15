# JPEG Decoder: TJpgDec - Tiny JPEG Decompressor

[![Component Registry](https://components.espressif.com/components/espressif/esp_jpeg/badge.svg)](https://components.espressif.com/components/espressif/esp_jpeg)
![maintenance-status](https://img.shields.io/badge/maintenance-actively--developed-brightgreen.svg)

TJpgDec is a lightweight JPEG image decompressor optimized for embedded systems with minimal memory consumption.

On some microcontrollers, TJpgDec is available in ROM and will be used by default, though this can be disabled in menuconfig if desired[^1].

[^1]: **_NOTE:_** When the ROM decoder is used, the configuration can't be changed. The configuration is fixed.

## Features

**Compilation configuration:**
- Stream input buffer size (default: 512 bytes)
- Output pixel format (default: RGB888; options: RGB888/RGB565)
- Enable/disable output descaling (default: enabled)
- Use table-based saturation for arithmetic operations (default: enabled)
- Use default Huffman tables: Useful from decoding frames from cameras, that do not provide Huffman tables (default: disabled to save ROM)
- Three optimization levels (default: 32-bit MCUs) for different CPU types:
  - 8/16-bit MCUs
  - 32-bit MCUs
  - Table-based Huffman decoding

**Runtime configuration:**
- Pixel format options: RGB888, RGB565
- Selectable scaling ratios: 1/1, 1/2, 1/4, or 1/8 (chosen at decompression)
- Option to swap the first and last bytes of color values

## TJpgDec in ROM

On certain microcontrollers, TJpgDec is available in ROM and used by default. This can be disabled in menuconfig if you prefer to use the library code provided in this component.

### List of MCUs, which have TJpgDec in ROM
- ESP32
- ESP32-S3
- ESP32-C3
- ESP32-C6
- ESP32-C5
- ESP32-C61

### Fixed compilation configuration of the ROM code
The ROM version uses the following fixed settings:
- Stream input buffer: 512 bytes
- Output pixel format: RGB888
- Output descaling: enabled
- Saturation table: enabled
- Optimization level: Basic (JD_FASTDECODE = 0)

### Pros and cons using ROM code

**Advantages:**
- Saves approximately 5 KB of flash memory with the same configuration

**Disadvantages:**
- Compilation configuration cannot be changed
- Certain configurations may provide faster performance

## Speed comparison

The table below shows example decoding times for a JPEG image using various configurations:
* Image size: 320 x 180 px
* Output format: RGB565
* CPU: ESP32-S3
* CPU frequency: 240 MHz
* SPI mode: DIO
* Internal RAM used
* Measured in 1000 retries

| ROM used | JD_SZBUF | JD_FORMAT | JD_USE_SCALE | JD_TBLCLIP | JD_FASTDECODE | RAM buffer | Flash size | Approx. time |
| :------: | :------: | :-------: | :----------: | :--------: | :-----------: | :--------: | :--------: | :----------: |
|   YES    |    512   |   RGB888  |      1       |      1     |       0       |    3.1 kB  |    0 kB    |     52 ms    |    
|   NO     |    512   |   RGB888  |      1       |      1     |       0       |    3.1 kB  |    5 kB    |     50 ms    |    
|   NO     |    512   |   RGB888  |      1       |      0     |       0       |    3.1 kB  |    4 kB    |     68 ms    |     
|   NO     |    512   |   RGB888  |      1       |      1     |       1       |    3.1 kB  |    5 kB    |     50 ms    |      
|   NO     |    512   |   RGB888  |      1       |      0     |       1       |    3.1 kB  |    4 kB    |     62 ms    |   
|   NO     |    512   |   RGB888  |      1       |      1     |       2       |   65.5 kB  |   5.5 kB   |     46 ms    |  
|   NO     |    512   |   RGB888  |      1       |      0     |       2       |   65.5 kB  |   4.5 kB   |     59 ms    |  
|   NO     |    512   |   RGB565  |      1       |      1     |       0       |    5 kB    |    5 kB    |     60 ms    |     
|   NO     |    512   |   RGB565  |      1       |      1     |       1       |    5 kB    |    5 kB    |     59 ms    |     
|   NO     |    512   |   RGB565  |      1       |      1     |       2       |   65.5 kB  |   5.5 kB   |     56 ms    |     

## Add to project

Packages from this repository are uploaded to [Espressif's component service](https://components.espressif.com/).
You can add them to your project via `idf.py add-dependancy`, e.g. 
```
    idf.py add-dependency esp_jpeg==1.0.0
```

Alternatively, you can create `idf_component.yml`. More is in [Espressif's documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html).

## Example use

Here is example of usage. This calling is **blocking**.

```
esp_jpeg_image_cfg_t jpeg_cfg = {
    .indata = (uint8_t *)jpeg_img_buf,
    .indata_size = jpeg_img_buf_size,
    .outbuf = out_img_buf,
    .outbuf_size = out_img_buf_size,
    .out_format = JPEG_IMAGE_OUT_FORMAT_RGB565,
    .out_scale = JPEG_IMAGE_SCALE_0,
    .flags = {
        .swap_color_bytes = 1,
    }
};
esp_jpeg_image_output_t outimg;

esp_jpeg_decode(&jpeg_cfg, &outimg);
```

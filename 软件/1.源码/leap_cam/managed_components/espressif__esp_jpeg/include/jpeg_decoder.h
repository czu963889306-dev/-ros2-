/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Scale of output image
 *
 */
typedef enum {
    JPEG_IMAGE_SCALE_0 = 0, /*!< No scale */
    JPEG_IMAGE_SCALE_1_2,   /*!< Scale 1:2 */
    JPEG_IMAGE_SCALE_1_4,   /*!< Scale 1:4 */
    JPEG_IMAGE_SCALE_1_8,   /*!< Scale 1:8 */
} esp_jpeg_image_scale_t;

/**
 * @brief Format of output image
 *
 */
typedef enum {
    JPEG_IMAGE_FORMAT_RGB888 = 0,   /*!< Format RGB888 */
    JPEG_IMAGE_FORMAT_RGB565,       /*!< Format RGB565 */
} esp_jpeg_image_format_t;

/**
 * @brief JPEG Configuration Type
 *
 */
typedef struct esp_jpeg_image_cfg_s {
    uint8_t *indata;        /*!< Input JPEG image */
    uint32_t indata_size;   /*!< Size of input image  */
    uint8_t *outbuf;        /*!< Output buffer */
    uint32_t outbuf_size;   /*!< Output buffer size */
    esp_jpeg_image_format_t out_format; /*!< Output image format */
    esp_jpeg_image_scale_t  out_scale; /*!< Output scale */

    struct {
        uint8_t swap_color_bytes: 1; /*!< Swap first and last color bytes */
    } flags;

    struct {
        void *working_buffer;       /*!< If set to NULL, a working buffer will be allocated in esp_jpeg_decode().
                                         Tjpgd does not use dynamic allocation, se we pass this buffer to Tjpgd that uses it as scratchpad */
        size_t working_buffer_size; /*!< Size of the working buffer. Must be set it working_buffer != NULL.
                                         Default size is 3.1kB or 65kB if JD_FASTDECODE == 2 */
    } advanced;

    struct {
        uint32_t read;  /*!< Internal count of read bytes */
    } priv;
} esp_jpeg_image_cfg_t;

/**
 * @brief JPEG output info
 */
typedef struct esp_jpeg_image_output_s {
    uint16_t width;    /*!< Width of the output image */
    uint16_t height;   /*!< Height of the output image */
    size_t output_len; /*!< Length of the output image in bytes */
} esp_jpeg_image_output_t;

/**
 * @brief Decode JPEG image
 *
 * @note This function is blocking.
 *
 * @param[in]  cfg: Configuration structure
 * @param[out] img: Output image info
 *
 * @return
 *      - ESP_OK            on success
 *      - ESP_ERR_NO_MEM    if there is no memory for allocating main structure
 *      - ESP_FAIL          if there is an error in decoding JPEG
 */
esp_err_t esp_jpeg_decode(esp_jpeg_image_cfg_t *cfg, esp_jpeg_image_output_t *img);

/**
 * @brief Get information about the JPEG image
 *
 * Use this function to get the size of the JPEG image without decoding it.
 * Allocate a buffer of size img->output_len to store the decoded image.
 *
 * @note cfg->outbuf and cfg->outbuf_size are not used in this function.
 * @param[in]  cfg: Configuration structure
 * @param[out] img: Output image info
 *
 * @return
 *      - ESP_OK              on success
 *      - ESP_ERR_INVALID_ARG if cfg or img is NULL
 *      - ESP_FAIL            if there is an error in decoding JPEG
 */
esp_err_t esp_jpeg_get_image_info(esp_jpeg_image_cfg_t *cfg, esp_jpeg_image_output_t *img);

#ifdef __cplusplus
}
#endif

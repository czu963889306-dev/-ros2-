#include "system_globals.h"
#include "camsense_lidar.h"
#include "msg/lidar_msg.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "LIDAR_TASK";

void lidar_task(void *p) {

    CamsenseLidar* lidar = new CamsenseLidar(

        UART_NUM_1,

        (gpio_num_t)UART_PIN_NO_CHANGE,

        GPIO_NUM_41,

        115200

    );



    lidar->Init();



    static LidarPoint full_scan_data[MAX_LIDAR_POINTS_PER_SCAN];

    uint16_t scan_point_count = 0;

    LidarMsg lidar_msg;



    ESP_LOGI(TAG, "Lidar task started on UART1, RX=41");



    while (1) {

        bool parsed_lidar_data = lidar->Poll(full_scan_data, &scan_point_count);



        if (parsed_lidar_data) {

            memset(&lidar_msg, 0, sizeof(LidarMsg));



            for (int i = 0; i < scan_point_count; i++) {

                int angle_deg = (int)(full_scan_data[i].angle + 0.5f);



                if (angle_deg >= 360) angle_deg -= 360;

                if (angle_deg < 0) angle_deg = 0;



                // 左右镜像

                angle_deg = (360 - angle_deg) % 360;



                uint16_t dist = (uint16_t)full_scan_data[i].distance;



                if (dist > 0) {

                    if (lidar_msg.distances[angle_deg] == 0 ||

                        dist < lidar_msg.distances[angle_deg]) {

                        lidar_msg.distances[angle_deg] = dist;

                    }

                }

            }



            xQueueOverwrite(q_lidar_state, &lidar_msg);

        }



        vTaskDelay(pdMS_TO_TICKS(10));

    }

}

#include "system_globals.h"
#include "board.h"
#include "msg/ultrasonic_msg.h"

void ultrasonic_task(void *p) {
    UltrasonicMsg msg;
    while (1) {
        msg.distance_cm = ultrasonic.GetDistanceCm(25.0f);
            //   ESP_LOGI("TAG", "%.2f",msg.distance_cm);
        xQueueOverwrite(q_ultrasonic_state, &msg);
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}
#ifndef NETWORK_TASK_H
#define NETWORK_TASK_H

#define TINY_GSM_MODEM_ALMAB
#define TINY_GSM_USE_GPRS true

#include <TinyGsmClient.h>

#include "modem.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h"

#include "buffer_manager.h"
#include "../../include/config.h"

#define MAX_BATCH_SIZE 1

// Network task implementation
void network_task(void* parameters);

// Process frame and send over HTTP
bool send_frame_over_http(camera_fb_t* frame);

#endif // NETWORK_TASK_H
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <time.h>
#include "cbuffer.h"
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../../include/config.h"

// Global circular buffer for image frames
extern CircularBuffer<camera_fb_t> cbuffer;

// Synchronization primitives
extern SemaphoreHandle_t buffer_mutex;
extern SemaphoreHandle_t data_available;
extern SemaphoreHandle_t space_available;

// Initialize buffer and synchronization primitives
void buffer_init();

// Thread-safe buffer operations
bool buffer_add_frame(camera_fb_t* frame);
camera_fb_t* buffer_get_frame();
int buffer_get_batch(camera_fb_t** frames, int max_frames);

#endif // BUFFER_MANAGER_H
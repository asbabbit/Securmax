#ifndef CAMERA_TASK_H
#define CAMERA_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "camera.h"
#include "buffer_manager.h"

// Camera task implementation
void camera_task(void* parameters);

#endif // CAMERA_TASK_H
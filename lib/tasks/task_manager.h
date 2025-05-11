#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "buffer_manager.h"
#include "../../include/config.h"

// Task priorities
#define CAMERA_TASK_PRIORITY      4
#define NETWORK_TASK_PRIORITY     7

// Task stack sizes
#define CAMERA_TASK_STACK_SIZE    4096
#define NETWORK_TASK_STACK_SIZE   8192

// Task handle references
extern TaskHandle_t camera_task_handle;
extern TaskHandle_t network_task_handle;

// Task declarations
void camera_task(void* parameters);
void network_task(void* parameters);

// Task management
void tasks_init();
void start_camera_task();
void start_network_task();
void pause_camera_task();
void resume_camera_task();
void pause_network_task();
void resume_network_task();

#endif // TASK_MANAGER_H
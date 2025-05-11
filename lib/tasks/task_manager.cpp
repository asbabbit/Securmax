#include "task_manager.h"

// Task handles
TaskHandle_t camera_task_handle = NULL;
TaskHandle_t network_task_handle = NULL;

void tasks_init() {
    // Initialize the buffer manager first
    buffer_init();
    
    // Start the tasks
    start_camera_task();
    start_network_task();
    
    SerialMon.println("[TASKS] Task manager initialized");
}

void start_camera_task() {
    xTaskCreatePinnedToCore(
        camera_task,            // Task function
        "camera_task",          // Task name
        CAMERA_TASK_STACK_SIZE, // Stack size (bytes)
        NULL,                   // Parameters
        CAMERA_TASK_PRIORITY,   // Task Priority
        &camera_task_handle,    // Task handle
        1                       // Core ID
    );
    
    SerialMon.println("[TASKS] Camera task started");
}

void start_network_task() {
    xTaskCreate(
        network_task,
        "network_task",
        NETWORK_TASK_STACK_SIZE,
        NULL,
        NETWORK_TASK_PRIORITY,
        &network_task_handle
    );
    
    SerialMon.println("[TASKS] Network task started");
}

void pause_camera_task() {
    if (camera_task_handle != NULL) {
        vTaskSuspend(camera_task_handle);
        SerialMon.println("[TASKS] Camera task paused");
    }
}

void resume_camera_task() {
    if (camera_task_handle != NULL) {
        vTaskResume(camera_task_handle);
        SerialMon.println("[TASKS] Camera task resumed");
    }
}

void pause_network_task() {
    if (network_task_handle != NULL) {
        vTaskSuspend(network_task_handle);
        SerialMon.println("[TASKS] Network task paused");
    }
}

void resume_network_task() {
    if (network_task_handle != NULL) {
        vTaskResume(network_task_handle);
        SerialMon.println("[TASKS] Network task resumed");
    }
}
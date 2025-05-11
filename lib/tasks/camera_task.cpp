#include "camera_task.h"

void camera_task(void* parameters) {
    // Notify that task has started
    SerialMon.println("[CAMERA_TASK] Camera task started");
    
    // Task main loop
    while (true) {
        // Take a picture
        camera_fb_t* fb = take_image();
        
        if (!fb) {
            // If failed to take image, wait a bit and try again
            SerialMon.println("[CAMERA_TASK] Failed to capture image, retrying...");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        
        // Log the image capture
        SerialMon.printf("[CAMERA_TASK] Captured image: %d bytes\n", fb->len);
        
        // Try to add frame to buffer
        bool added = buffer_add_frame(fb);
        
        if (!added) {
            // If buffer is full, we need to return the frame
            // to avoid memory leaks
            SerialMon.println("[CAMERA_TASK] Buffer full, discarding frame");
            esp_camera_fb_return(fb);
            
            // Wait a bit before trying again
            vTaskDelay(pdMS_TO_TICKS(50));
        } else {
            SerialMon.println("[CAMERA_TASK] Frame added to buffer");
        }
        
        // Small delay to control frame rate
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
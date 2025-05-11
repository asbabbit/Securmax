#include "network_task.h"

bool process_frame_batch(camera_fb_t** frames, int count) {
    if (count == 0) {
        return false;
    }
    
    SerialMon.printf("[NETWORK_TASK] Processing batch of %d frames\n", count);
    
    // Keep track of the batch number
    static int batch_counter = 0;
    batch_counter = (batch_counter + 1) % 100; // Keep it within reasonable limits
    
    // Send all frames as one multipart request
    //bool success = send_post(batch_counter, frames, count);
    bool success = false;
    
    if (!success) {
        SerialMon.println("[NETWORK_TASK] Failed to send batch");
    } else {
        SerialMon.println("[NETWORK_TASK] Batch sent successfully");
    }
    
    // Return all frames to the camera system
    for (int i = 0; i < count; i++) {
        esp_camera_fb_return(frames[i]);
    }
    
    SerialMon.printf("[NETWORK_TASK] Batch processing complete, success: %s\n", 
                    success ? "true" : "false");
                    
    return success;
}


void network_task(void* parameters) {
    // Notify that task has started
    SerialMon.println("[NETWORK_TASK] Network task started");
    
    // Array to hold batch of frames
    camera_fb_t* frames[MAX_BATCH_SIZE];
    
    // Task main loop
    while (true) {
        // Get batch of frames (up to MAX_BATCH_SIZE)
        int count = buffer_get_batch(frames, MAX_BATCH_SIZE);
        
        if (count == 0) {
            // If no frames available, wait a bit and try again
            SerialMon.println("[NETWORK_TASK] No frames available, waiting...");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        
        SerialMon.printf("[NETWORK_TASK] Got %d frames from buffer, processing batch...\n", count);
    
        process_frame_batch(frames, count);
        
        // Add a small delay based on network conditions
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
#include "buffer_manager.h"

// Global circular buffer
CircularBuffer<camera_fb_t> cbuffer;

// Synchronization primitives
SemaphoreHandle_t buffer_mutex = NULL;
SemaphoreHandle_t data_available = NULL;
SemaphoreHandle_t space_available = NULL;

void buffer_init() {
    // Initialize circular buffer
    cbuffer.init();
    
    // Create mutex for protecting circular buffer access
    buffer_mutex = xSemaphoreCreateMutex();
    
    // Create semaphores for signaling between tasks
    // BUFFER_SIZE is defined in cbuffer.h and is the size of the circular buffer
    data_available = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
    space_available = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
    
    SerialMon.println("[BUFFER] Initialized buffer manager with size: " + String(BUFFER_SIZE));
}

bool buffer_add_frame(camera_fb_t* frame) {
    unsigned long timestamp = millis();
    
    if (!frame) {
        return false;
    }
    
    // Wait for space to be available (with timeout)
    if (xSemaphoreTake(space_available, pdMS_TO_TICKS(100)) != pdTRUE) {
        // Buffer full and timed out, return failure
        SerialMon.println("[BUFFER] Failed to add frame: buffer full");
        return false;
    }
    
    // Take mutex to protect circular buffer access
    xSemaphoreTake(buffer_mutex, portMAX_DELAY);
    
    // Add frame to buffer
    cbuffer.add(frame);
    
    // Release mutex
    xSemaphoreGive(buffer_mutex);
    
    // Signal that data is available
    xSemaphoreGive(data_available);

    SerialMon.printf("[BUFFER] Time added: %lu ms \n", millis());
    
    return true;
}

camera_fb_t* buffer_get_frame() {
    unsigned long timestamp = millis();
    // Wait for data to be available (with timeout)
    if (xSemaphoreTake(data_available, pdMS_TO_TICKS(100)) != pdTRUE) {
        // No data available within timeout
        return NULL;
    }
    
    // Take mutex to protect circular buffer access
    xSemaphoreTake(buffer_mutex, portMAX_DELAY);
    
    // Get frame from buffer
    camera_fb_t* frame = cbuffer.get();
    
    // Release mutex
    xSemaphoreGive(buffer_mutex);
    
    // Signal that space is available
    xSemaphoreGive(space_available);

    SerialMon.printf("[BUFFER] Frame sent: %lu ms \n", millis());
    
    return frame;
}

int buffer_get_batch(camera_fb_t** frames, int max_frames) {
    unsigned long timestamp = millis();
    int count = 0;
    
    // First check how many frames are available without removing them
    // We use a quick check without blocking to estimate availability
    int estimated_available = uxSemaphoreGetCount(data_available);
    int frames_to_get = (estimated_available >= max_frames) ? max_frames : estimated_available;
    
    // If no frames available, return early
    if (frames_to_get == 0) {
        return 0;
    }
    
    // Try to get the number of frames we estimated
    for (int i = 0; i < frames_to_get; i++) {
        // Try to get one frame with a short timeout
        if (xSemaphoreTake(data_available, pdMS_TO_TICKS(20)) != pdTRUE) {
            // If we can't get more frames, just return what we have
            break;
        }
        
        // We successfully reserved a frame slot, now get the actual frame
        xSemaphoreTake(buffer_mutex, portMAX_DELAY);
        frames[count] = cbuffer.get();
        xSemaphoreGive(buffer_mutex);
        
        // Signal that space is available
        xSemaphoreGive(space_available);
        
        // Only increment count if we actually got a frame
        if (frames[count] != NULL) {
            count++;
        } else {
            // If we got NULL for some reason, give back the data semaphore
            xSemaphoreGive(data_available);
            break;
        }
    }
    
    SerialMon.printf("[BUFFER] Batch sent: %lu ms \n", millis());
    return count;
}
#include "camera.h"

bool camera_initialized = false;
int current_jpeg_quality = 10; // Initial quality

bool camera_init() {
    pinMode(PWR_ON_PIN, OUTPUT);
    digitalWrite(PWR_ON_PIN, HIGH);
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAM_Y2_PIN;
    config.pin_d1 = CAM_Y3_PIN;
    config.pin_d2 = CAM_Y4_PIN;
    config.pin_d3 = CAM_Y5_PIN;
    config.pin_d4 = CAM_Y6_PIN;
    config.pin_d5 = CAM_Y7_PIN;
    config.pin_d6 = CAM_Y8_PIN;
    config.pin_d7 = CAM_Y9_PIN;
    config.pin_xclk = CAM_XCLK_PIN;
    config.pin_pclk = CAM_PCLK_PIN;
    config.pin_vsync = CAM_VSYNC_PIN;
    config.pin_href = CAM_HREF_PIN;
    config.pin_sccb_sda = CAM_SIOD_PIN;
    config.pin_sccb_scl = CAM_SIOC_PIN;
    config.pin_pwdn = CAM_PWDN_PIN;
    config.pin_reset = CAM_RESET_PIN;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST; // Always get newest frame
    
    // Optimize for size while maintaining reasonable quality
    if (psramFound())
    {
        SerialMon.println("[CAMERA] Using PSRAM");
        // Use CIF (352x288) - better than QVGA but smaller file size than VGA
        config.frame_size = FRAMESIZE_CIF;
        // Higher jpeg_quality number = more compression, smaller file
        current_jpeg_quality = 25; // Update our global variable
        config.jpeg_quality = current_jpeg_quality;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.fb_count = BUFFER_SIZE;
    }
    else
    {
        config.frame_size = FRAMESIZE_QQVGA; // 160x120 - very small
        current_jpeg_quality = 30; // Update our global variable
        config.jpeg_quality = current_jpeg_quality;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // Initialize camera
    SerialMon.println("[CAMERA] Initializing camera...");
    // Add debug info for the camera pins
    SerialMon.println("[CAMERA] Camera pins configuration:");
    SerialMon.printf("  SIOD: %d, SIOC: %d\n", CAM_SIOD_PIN, CAM_SIOC_PIN);
    SerialMon.printf("  VSYNC: %d, HREF: %d, PCLK: %d, XCLK: %d\n", CAM_VSYNC_PIN, CAM_HREF_PIN, CAM_PCLK_PIN, CAM_XCLK_PIN);
    SerialMon.printf("  D0-D7: %d, %d, %d, %d, %d, %d, %d, %d\n", CAM_Y2_PIN, CAM_Y3_PIN, CAM_Y4_PIN, CAM_Y5_PIN, CAM_Y6_PIN, CAM_Y7_PIN, CAM_Y8_PIN, CAM_Y9_PIN);

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        SerialMon.printf("[CAMERA] Camera initialization failed with error 0x%x\n", err);
        SerialMon.println("[CAMERA] Continuing without camera functionality");
        delay(3000);
        return false;
        // Don't restart, just continue without camera
    } else {
        SerialMon.println("[CAMERA] Camera initialized successfully");
        
        // Additional runtime optimization
        sensor_t * s = esp_camera_sensor_get();
        if (s) {
            // Reduce contrast and brightness slightly for better compression
            s->set_contrast(s, -1);
            s->set_brightness(s, -1);
            
            // Turn off features we don't need
            s->set_saturation(s, -2); // Reduce color saturation
            s->set_whitebal(s, 0);    // Turn off auto white balance
            s->set_gain_ctrl(s, 0);   // Turn off auto gain control
        }
        
        camera_initialized = true;
        delay(3000);        
        return true;
    }
}

camera_fb_t* take_image(){
    // Check if camera is initialized
    if (!camera_initialized) {
        SerialMon.println("[ERROR] Camera not initialized, cannot take picture");
        return NULL;
    }

    // Take a picture
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        SerialMon.println("[ERROR] Failed to capture image");
        return NULL;
    }
    return fb;
}
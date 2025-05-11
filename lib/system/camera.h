#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include <esp_camera.h>
#include "../../include/config.h"

bool camera_init();
void adjust_quality();
void adjust_resolution();
void adjust_buffersize();
camera_fb_t* take_image();

// Global variables
extern bool camera_initialized;


#endif // CAMERA_H
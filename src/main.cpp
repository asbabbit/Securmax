#include <Arduino.h>
#include "system.h"
#include "../../include/config.h"

void setup() {
  // Initialize the entire system
  system_init();
  Serial.println("!! SYSTEM SUCCESSFULLY INITIALIZED AND RUNNING !!");

}

void loop() {
  uint32_t start_time = millis();
  SerialMon.printf("[TIMING] Starting frame capture at %d\n", start_time);
  
  camera_fb_t* frame = take_image();
  uint32_t after_capture = millis();
  SerialMon.printf("[TIMING] Frame captured in %d ms\n", after_capture - start_time);
  
  send_frame(frame, after_capture);
  uint32_t after_send = millis();
  SerialMon.printf("[TIMING] Frame sent in %d ms\n", after_send - after_capture);
  SerialMon.printf("[TIMING] Total cycle time: %d ms\n", after_send - start_time);
}
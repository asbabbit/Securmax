#ifndef MODEM_H
#define MODEM_H

#define TINY_GSM_MODEM_ALMAB
#define TINY_GSM_USE_GPRS true

// Modem max size per message sent
#define MAX_BYTE_PER_MESSAGE    1500

#define ENDPOINT "/upload"

#include <TinyGsmClient.h>
#include <TinyGsmParser.h>
#include "esp_camera.h"

#include "../../include/config.h"
#include "../../include/utils.h"

// Power cycle modem
bool modem_on();

// 
bool sendATCommand(const char* command, const char* expectedResponse, unsigned long timeout);

// Setup modem
void setup_modem();

// Test modem to network connection
void test_modem();

// Reconnect modem to network
bool reconnect_modem(uint8_t max_attempts = 3);

bool send_frame(camera_fb_t* frame, uint32_t time);

bool send_batch(uint8_t batch, camera_fb_t** frames, uint8_t count);

// External declarations
extern TinyGsm modem;
extern TinyGsmClient client;
extern Parser<uint8_t> parse;

#endif //MODEM_H
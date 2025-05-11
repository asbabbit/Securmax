#include "system.h"

//TODO: Add synchronization with NTP server

void system_init() {
    SerialMon.begin(115200);
    delay(500);
    SerialMon.println("[SYSTEM] Starting system initialization...");
    delay(500);
    
    // Initialize hardware
    board_on();
    modem_on();
    setup_modem();
    test_modem();
    camera_init();

    // Create socket
    client.connect(SERVER_IP, SERVER_PORT);
        
}

void board_on(){
    pinMode(PWR_ON_PIN, OUTPUT);
    digitalWrite(PWR_ON_PIN, HIGH);
    SerialMon.println("[SYSTEM] Power on pin set");
    delay(3000); 
}
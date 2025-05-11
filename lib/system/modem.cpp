#include "modem.h"

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
Parser<uint8_t> parse;

bool modem_on() {
    
    // Try multiple baudrates in sequence
    const long baudrates[] = {921600};
    
    // Power cycle with more time
    pinMode(PCIE_PWR_PIN, OUTPUT);
    digitalWrite(PCIE_PWR_PIN, 1);  // Power OFF
    delay(3000);                     // Wait 3 seconds
    SerialMon.println("[MODEM] Power OFF");
    
    digitalWrite(PCIE_PWR_PIN, 0);  // Power ON
    SerialMon.println("[MODEM] Power ON");
    delay(10000);                    // Wait 10 seconds for stable boot
    
    // Try each baudrate
    for (int i = 0; i < sizeof(baudrates)/sizeof(baudrates[0]); i++) {
        SerialMon.print("[MODEM] Testing baudrate: ");
        SerialMon.println(baudrates[i]);
        
        SerialAT.end();
        delay(100);
        SerialAT.begin(baudrates[i], SERIAL_8N1, PCIE_RX_PIN, PCIE_TX_PIN);
        delay(500);
        
        // Clear buffer
        while (SerialAT.available()) {
            SerialAT.read();
        }
        
        // Try multiple AT commands
        for (int attempt = 0; attempt < 3; attempt++) {
            SerialMon.print("[MODEM] AT attempt #");
            SerialMon.println(attempt + 1);
            
            SerialAT.println("AT");
            delay(1000);
            
            String response = "";
            unsigned long start = millis();
            while (millis() - start < 2000) {
                if (SerialAT.available()) {
                    char c = SerialAT.read();
                    response += c;
                    SerialMon.write(c);  // Echo to serial monitor
                }
            }
            
            if (response.indexOf("OK") >= 0) {
                SerialMon.print("[MODEM] Success at ");
                SerialMon.print(baudrates[i]);
                SerialMon.println(" bps");
                
                // If we found a working baudrate but it's not what we want
                if (baudrates[i] != 921600) {
                    SerialMon.println("[MODEM] Setting baudrate to 921600");
                    SerialAT.println("AT+IPR=921600");
                    delay(1000);
                    
                    SerialAT.updateBaudRate(921600);
                    delay(500);
                    
                    // Verify new baudrate works
                    SerialAT.println("AT");
                    delay(1000);
                    response = "";
                    while (SerialAT.available()) {
                        char c = SerialAT.read();
                        response += c;
                        SerialMon.write(c);
                    }
                    
                    if (response.indexOf("OK") >= 0) {
                        SerialMon.println("[MODEM] Successfully set to 921600 bps");
                    } else {
                        SerialMon.println("[MODEM] Failed at 921600, staying at current baudrate");
                    }
                }
                
                return true;
            }
        }
    }
    
    SerialMon.println("[MODEM] Failed to communicate with modem at any baudrate");
    return false;
}

void setup_modem() {
    // Initialize modem
    SerialMon.println("[MODEM] Initializing modem...");
    modem.init();
    String modemInfo = modem.getModemInfo();
    delay(100);
    SerialMon.print("   Modem Info: ");
    SerialMon.println(modemInfo);
    
    // Set to LTE only mode (38)
    int res;
    do {
        res = modem.setNetworkMode(38);
        SerialMon.print("   Modem Set LTE: ");
        SerialMon.println(res);
        delay(500);
    } while (res != 1);
}

void test_modem(){
    // First check if modem is responsive
    SerialAT.println("AT");
    delay(100);
    if (SerialAT.available()) {
        String response = SerialAT.readString();
        if (response.indexOf("OK") < 0) {
            SerialMon.println("[MODEM] Modem not responding, attempting to reinitialize...");
            setup_modem();
            delay(5000);
            return;
        }
    } else {
        SerialMon.println("[MODEM] No response from modem, attempting to reinitialize...");
        setup_modem();
        delay(5000);
        return;  // Skip this loop iteration
    }

    SerialMon.print("[MODEM] Waiting for network...");
    if (!modem.waitForNetwork(60000L)) {  // Wait up to 60 seconds
        SerialMon.println(" fail");
        SerialMon.println("[MODEM] Network not available, will try again later");
        delay(10000);
        return;
    }
    SerialMon.println(" success");
    if(modem.isNetworkConnected()) {
        SerialMon.println("[MODEM] Network connected");
    }

    SerialMon.print(F("Connecting to "));
    SerialMon.print(APN);
    SerialMon.print(F("..."));
    if (!modem.gprsConnect(APN, GPRS_USERS, GPRS_PASS)) {
        SerialMon.println(" fail");
        SerialMon.println("[MODEM] GPRS connection failed, will try again later");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) { 
        SerialMon.println("[MODEM] GPRS connected"); 
    }
}

bool reconnect_modem(uint8_t maxAttempts) {
    SerialMon.println("Attempting to reconnect cellular connection...");
    
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        SerialMon.printf("Reconnection attempt %d of %d\n", attempt, maxAttempts);
        
        // First, check if modem is responsive
        SerialAT.println("AT");
        delay(500);
        
        bool modemResponsive = false;
        if (SerialAT.available()) {
            String response = SerialAT.readString();
            if (response.indexOf("OK") >= 0) {
                modemResponsive = true;
            }
        }
        
        // If modem is not responsive, try power cycling
        if (!modemResponsive) {
            SerialMon.println("Modem not responding, power cycling...");
            setup_modem();
            delay(5000);
        }
        
        // Try connecting to network
        SerialMon.print("Waiting for network...");
        if (!modem.waitForNetwork(30000L)) {  // 30 second timeout
            SerialMon.println(" fail");
            delay(5000);
            continue;  // Try next attempt
        }
        SerialMon.println(" success");
        
        if (modem.isNetworkConnected()) {
            SerialMon.println("Network connected");
            
            // Try GPRS connection
            SerialMon.printf("Connecting to %s...\n", APN);
            if (!modem.gprsConnect(APN, GPRS_USERS, GPRS_PASS)) {
                SerialMon.println(" fail");
                delay(5000);
                continue;  // Try next attempt
            }
            SerialMon.println(" success");
            
            if (modem.isGprsConnected()) {
                SerialMon.println("GPRS connected");
                return true;  // Successfully reconnected
            }
        }
    }
}

// TODO: Add logic for when packets are dropped
bool send_frame(camera_fb_t* frame, uint32_t time){
    if(!client.connected()){
        SerialMon.println("[SOCKET] Socket not connected, connecting...");
        if(!client.connect(SERVER_IP, SERVER_PORT)){
            SerialMon.println("[SOCKET] Connection failed!");
            return false;
        }
    }

    parse.new_msg(frame->buf, frame->len, time);

    const meta_t* meta = parse.get_meta();
    uint32_t timestamp = htonl(meta->timestamp);     
    uint8_t chunk_count = meta->chunk_count;
    uint16_t msg_size = htons(static_cast<uint16_t>(meta->msg_size));

    SerialMon.println("[SOCKET] Sending metadata packet...");    
    SerialMon.printf("[SOCKET] Metadata size of %dB\n", METADATA_HEADER_SIZE, time); 
    if(!client.begin_packet(METADATA_HEADER_SIZE, SERVER_IP, SERVER_PORT)){
        SerialMon.println("[SOCKET] Failed to begin metadata packet");
        client.stop();
        esp_camera_fb_return(frame);
        return false;
    }

    client.write((uint8_t*)&timestamp, 4);
    client.write((uint8_t*)&chunk_count, 1);
    client.write((uint8_t*)&msg_size, 2);

    int sent = client.end_packet();   
    SerialMon.printf("[SOCKET] %dB sent\n", sent);
    if(sent <= 0) {
        SerialMon.println("[SOCKET] Failed to send metadata packet");
        client.stop();
        esp_camera_fb_return(frame);
        return false;
    }

    while(parse.has_next()){
        const chunk_t* chunk = parse.get_chunk();
        if(!chunk){
            SerialMon.println("[SOCKET] Failed to get chunk");
            break;
        }
        
        //TODO: Change structure of parser, or create function for hton. I dont like
        //      how this code looks lol :P

        uint16_t data_size = htons(chunk->data_size);
        uint8_t seq = chunk->seq;

        SerialMon.printf("[SOCKET] Frame size of %dB at time %d\n", meta->msg_size, time); 
        SerialMon.printf("[SOCKET] Sending chunk %d/%d of size %dB\n", chunk->seq, meta->chunk_count-1, chunk->data_size);
        size_t packet_size = APP_HEADER_SIZE + chunk->data_size;

        if(!client.begin_packet(packet_size, SERVER_IP, SERVER_PORT)){
            SerialMon.println("[SOCKET] Failed to begin chunk packet");
            break;
        }

        client.write((uint8_t*)&data_size, 2);
        client.write((uint8_t*)&seq, 1);
        client.write(chunk->data, chunk->data_size);

        sent = client.end_packet();
        SerialMon.printf("[SOCKET] %dB sent\n", sent);
        if (sent <= 0) {
            SerialMon.printf("[SOCKET] Failed to send chunk %d\n", chunk->seq);
            break;
        }

    }
    // client.stop();
    esp_camera_fb_return(frame);
    return true;
}
// #include <Arduino.h>
// #include "TinyGsmParser.h"

// // Romeo and Juliet test message
// const size_t TEST_MSG_SIZE = 3595;  // 5KB test message
// char* romeo_and_juliet_text = nullptr; // Will be dynamically allocated

// // Function to initialize test message with Romeo and Juliet text
// const char* initRomeoAndJulietText() {
//   // Allocate memory for the text
//   romeo_and_juliet_text = new char[TEST_MSG_SIZE + 1]; // +1 for null terminator
  
//   // Shakespeare's Romeo and Juliet excerpt (prologue and beginning)
//   const char* text = 
// "In a world teetering on the edge of collapse, where chaos surged through the cracks of civilization and hope had long fled to the shadows, one man stood tall—well, not particularly tall—but *undeniably* bold. His name was Ted. Not Theodore. Not \"The Terrific Ted.\" Just Ted. And while he wore no cape, nor wielded any mythical sword, two things about him were universally, undeniably true: **Ted was the bravest** and **baldest** hero humanity had ever known.\n"
// "\n"
// "Ted’s baldness wasn’t the kind of quiet, reluctant baldness that snuck in with age or stress. No, Ted's scalp gleamed like a lighthouse in the darkness—shiny, defiant, and proud. He had been bald since age twelve, after saving an entire kindergarten class from a chemical spill in a rogue science lab incident. The explosion vaporized the follicles from his head, but from the ashes of that childhood accident rose a man of steel nerves, a diamond dome, and unmatched courage.\n"
// "\n"
// "As Earth faced its most catastrophic threat—a transdimensional rift spewing creatures of pure entropy into the sky—armies fell, governments crumbled, and every conventional hero faltered. But not Ted. He didn't falter. He didn’t even flinch. He marched into battle with nothing but his sheer willpower, his glaringly bald head, and his superpowers: telekinetic fists, quantum endurance, and the ability to shoot blinding beams of light from his smooth, radiant scalp. Scientists never fully understood how it worked, only that when the bald beams fired, entire fleets of invaders vaporized instantly, as if the universe itself bowed to Ted’s radiant defiance.\n"
// "\n"
// "Despite his power, it was Ted’s bravery that truly set him apart. He dove headfirst into danger when others turned away. When the last surviving city was encased in a cocoon of anti-matter vines, Ted broke through it with nothing but a determined scream and the glowing conviction of justice in his eyes. His battle cry echoed through space and time:\n"
// "\"By the bald and by the brave—I will save you!\"\n"
// "\n"
// "Legends speak of how he faced the Prime Wyrm of Despair—a serpent made of black holes and grief—without armor or backup. He didn’t hesitate. He simply cracked his knuckles, looked the monster in its infinite eyes, and whispered, \"You picked the wrong planet.\" The fight lasted seventeen seconds. Ted walked out with a smirk and a lollipop. He gave it to a child crying in the rubble. Because that’s what the bravest man alive does.\n"
// "\n"
// "Even the villains who survived their encounters with Ted—if any did—would speak in hushed, reverent tones of his fearless resolve and that hypnotically reflective dome of his. The mere sight of his bald head reflecting the flames of war became a symbol of hope. People would paint it on flags, shave their heads in solidarity, and whisper prayers not to gods, but to Ted.\n"
// "\n"
// "It wasn’t fame Ted sought. He never gave interviews, never signed autographs, and never ran for office. His only mission was to protect humanity, no matter the cost. When the final portal began to close and someone needed to stay behind to seal it from the inside, Ted didn’t hesitate. With one final salute, he stepped through, sealing the breach—and his fate—forever.\n"
// "\n"
// "Today, children learn of him in schools, adults speak of him in awe, and statues of his smooth, glimmering head rise in every city. He was not the fastest, nor the strongest, but he was, without a shred of doubt, **the bravest** and **the baldest** of them all. Humanity owes its survival to a man named Ted—a hero unlike any before or since.\n";

//   // Copy initial text
//   strcpy(romeo_and_juliet_text, text);
  
//   // Calculate initial length
//   size_t current_length = strlen(romeo_and_juliet_text);
  
//   // Repeat text until we reach at least TEST_MSG_SIZE characters
//   while (current_length < TEST_MSG_SIZE) {
//     size_t remaining = TEST_MSG_SIZE - current_length;
//     size_t copy_size = (remaining < current_length) ? remaining : current_length;
    
//     // Copy the text to fill the remaining space
//     memcpy(romeo_and_juliet_text + current_length, romeo_and_juliet_text, copy_size);
//     current_length += copy_size;
//   }
  
//   // Ensure null termination
//   romeo_and_juliet_text[TEST_MSG_SIZE] = '\0';
  
//   return text;
// }

// // Mock UDP client for testing
// class MockUdpClient {
// public:
//   MockUdpClient() : bytes_written(0), packets_sent(0) {}
  
//   // Write a single byte
//   size_t write(uint8_t b) {
//     bytes_written++;
//     return 1;
//   }
  
//   // Write a buffer
//   size_t write(const uint8_t* buf, size_t size) {
//     if (!buf || size == 0) return 0;
    
//     bytes_written += size;
    
//     // Log first few bytes for debugging
//     if (log_enabled) {
//       Serial.printf("Packet data (%d bytes): ", size);
//       for (size_t i = 0; i < min(size, (size_t)16); i++) {
//         Serial.printf("%02X ", buf[i]);
//       }
//       Serial.println(size > 16 ? "..." : "");
//     }
    
//     return size;
//   }
  
//   // Write 16-bit value
//   size_t write(uint16_t value) {
//     uint8_t buf[2];
//     buf[0] = (value >> 8) & 0xFF;  // High byte
//     buf[1] = value & 0xFF;         // Low byte
//     return write(buf, 2);
//   }
  
//   // Write 32-bit value
//   size_t write(uint32_t value) {
//     uint8_t buf[4];
//     buf[0] = (value >> 24) & 0xFF;
//     buf[1] = (value >> 16) & 0xFF;
//     buf[2] = (value >> 8) & 0xFF;
//     buf[3] = value & 0xFF;
//     return write(buf, 4);
//   }
  
//   // Send a complete packet
//   bool sendPacket(const chunk_t* chunk, uint8_t msg_id, uint8_t client_id, 
//                  uint8_t seq_num, uint32_t timestamp) {
//     if (!chunk) return false;
    
//     bool success = true;
    
//     // Write packet headers
//     success &= (write(chunk->checksum) == 2);      // Already in network byte order (16-bit)
//     success &= (write((uint8_t)msg_id) == 1);      // Single byte, no conversion needed
//     success &= (write((uint8_t)client_id) == 1);   // Single byte, no conversion needed
//     success &= (write((uint8_t)seq_num) == 1);     // Single byte, no conversion needed
//     success &= (write(chunk->size) == 2);          // Already in network byte order (16-bit)
//     success &= (write(htonl(timestamp)) == 4);     // Convert 32-bit value to network byte order
  
    
//     // Write data payload
//     uint16_t data_size = ntohs(chunk->size);
//     success &= (write(chunk->data, data_size) == data_size);
    
//     if (success) {
//       packets_sent++;
      
//       if (log_enabled) {
//         Serial.printf("Packet #%d sent successfully (seq=%d, size=%d)\n", 
//                       packets_sent, seq_num, data_size);
//       }
//     }
    
//     return success;
//   }
  
//   // Enable/disable detailed logging
//   void setLogging(bool enable) {
//     log_enabled = enable;
//   }
  
//   // Get statistics
//   size_t getBytesWritten() const { return bytes_written; }
//   size_t getPacketsSent() const { return packets_sent; }
//   void resetStats() { bytes_written = 0; packets_sent = 0; }
  
// private:
//   size_t bytes_written;
//   size_t packets_sent;
//   bool log_enabled = true;
// };

// // Message reassembly test class (simulating receiving side)
// class MessageReceiver {
// public:
//   MessageReceiver(size_t max_size) 
//     : buffer(new uint8_t[max_size]), 
//       buffer_size(max_size),
//       received_chunks(0),
//       total_chunks(0),
//       msg_id(0),
//       client_id(0),
//       timestamp(0),
//       is_complete(false) {
//     memset(buffer, 0, max_size);
//     memset(chunk_received, 0, sizeof(chunk_received));
//   }
  
//   ~MessageReceiver() {
//     delete[] buffer;
//   }
  
//   // Process an incoming packet
//   bool processPacket(uint16_t checksum, uint8_t msg_id_, uint8_t client_id_, 
//                     uint8_t seq_num, uint16_t size, uint32_t timestamp_, 
//                     const uint8_t* data) {
//     // First packet initialization
//     if (received_chunks == 0) {
//       msg_id = msg_id_;
//       client_id = client_id_;
//       timestamp = timestamp_;
//     }
    
//     // Verify packet belongs to current message
//     if (msg_id != msg_id_ || client_id != client_id_ || timestamp != timestamp_) {
//       Serial.println("Error: Packet doesn't match current message parameters");
//       return false;
//     }
    
//     // Verify sequence number
//     if (seq_num >= MAX_SEQ_NUM) {
//       Serial.printf("Error: Invalid sequence number: %d\n", seq_num);
//       return false;
//     }
    
//     // Check if this chunk was already received
//     if (chunk_received[seq_num]) {
//       Serial.printf("Warning: Duplicate chunk received (seq=%d)\n", seq_num);
//       return true; // Not an error, just a duplicate
//     }
    
//     // Convert size from network byte order
//     uint16_t actual_size = ntohs(size);
    
//     // Calculate buffer offset for this chunk
//     size_t offset = seq_num * PAYLOAD_SIZE;
    
//     // Check if this chunk would exceed the buffer
//     if (offset + actual_size > buffer_size) {
//       Serial.println("Error: Chunk would exceed buffer size");
//       return false;
//     }
    
//     // Copy data to buffer
//     memcpy(buffer + offset, data, actual_size);
    
//     // Mark chunk as received
//     chunk_received[seq_num] = true;
//     received_chunks++;
    
//     // Update total chunks if needed
//     if (seq_num >= total_chunks) {
//       total_chunks = seq_num + 1;
//     }
    
//     // Check if message is complete
//     updateCompletionStatus();
    
//     // Only print first 20 characters of the chunk
//     Serial.printf("Received chunk #%d (size=%d bytes)\n", seq_num, actual_size);
//     Serial.print("First 20 chars: ");
//     char preview[21];
//     int chars_to_copy = min(actual_size, (uint16_t)20);
//     memcpy(preview, data, chars_to_copy);
//     preview[chars_to_copy] = '\0';
//     Serial.println(preview);
    
//     return true;
//   }
  
//   // Check if all chunks have been received
//   bool isComplete() const {
//     return is_complete;
//   }
  
//   // Get received data
//   const uint8_t* getData() const {
//     return buffer;
//   }
  
//   // Get total size of received data
//   size_t getSize() const {
//     size_t total_size = 0;
//     for (uint8_t i = 0; i < total_chunks; i++) {
//       if (chunk_received[i]) {
//         total_size += (i < total_chunks - 1) ? PAYLOAD_SIZE : 
//                     (getLastChunkSize() > 0 ? getLastChunkSize() : PAYLOAD_SIZE);
//       }
//     }
//     return total_size;
//   }
  
//   // Get the size of the last chunk
//   uint16_t getLastChunkSize() const {
//     if (total_chunks == 0) return 0;
    
//     uint8_t last_seq = total_chunks - 1;
//     size_t offset = last_seq * PAYLOAD_SIZE;
    
//     // Find actual data size by looking for null terminator or using max size
//     for (size_t i = 0; i < PAYLOAD_SIZE; i++) {
//       if (buffer[offset + i] == 0) {
//         return i;
//       }
//     }
    
//     return PAYLOAD_SIZE;
//   }
  
//   // Get message statistics
//   uint8_t getReceivedChunks() const { return received_chunks; }
//   uint8_t getTotalChunks() const { return total_chunks; }
//   uint8_t getMsgId() const { return msg_id; }
//   uint8_t getClientId() const { return client_id; }
//   uint32_t getTimestamp() const { return timestamp; }
  
//   // Print the complete message when all chunks received
//   void printCompleteMessage() {
//     if (!is_complete) {
//       Serial.println("Message is not complete yet");
//       return;
//     }
    
//     Serial.println("\n=== COMPLETE MESSAGE ===\n");
    
//     // Calculate actual message size
//     size_t msg_size = getSize();
    
//     // Print the entire message
//     Serial.write(buffer, msg_size);
//     Serial.println("\n=======================\n");
//   }
  
//   // Reset receiver for new message
//   void reset() {
//     memset(buffer, 0, buffer_size);
//     memset(chunk_received, 0, sizeof(chunk_received));
//     received_chunks = 0;
//     total_chunks = 0;
//     msg_id = 0;
//     client_id = 0;
//     timestamp = 0;
//     is_complete = false;
//   }
  
// private:
//   uint8_t* buffer;
//   size_t buffer_size;
//   uint8_t received_chunks;
//   uint8_t total_chunks;
//   uint8_t msg_id;
//   uint8_t client_id;
//   uint32_t timestamp;
//   bool is_complete;
//   bool chunk_received[MAX_SEQ_NUM + 1];
  
//   // Update completion status
//   void updateCompletionStatus() {
//     if (total_chunks == 0) {
//       is_complete = false;
//       return;
//     }
    
//     is_complete = true;
//     for (uint8_t i = 0; i < total_chunks; i++) {
//       if (!chunk_received[i]) {
//         is_complete = false;
//         break;
//       }
//     }
    
//     // Don't automatically print the complete message
//     // This will now be done externally when specifically requested
//     if (is_complete) {
//       Serial.println("Message assembly complete!");
//     }
//   }
// };

// // Function to run sequential transmission test
// void testSequentialTransmission(const char* msg, size_t msg_size, 
//                                uint8_t msg_id, uint8_t client_id, uint32_t timestamp) {
//   Serial.println("\n=== SEQUENTIAL TRANSMISSION TEST ===\n");
  
//   // Create parser and client
//   Parser<uint8_t> parser((uint8_t*)msg, msg_id, msg_size, client_id, timestamp);
//   MockUdpClient client;
//   MessageReceiver receiver(msg_size + PAYLOAD_SIZE); // Extra space for safety
  
//   uint8_t chunk_count = parser.get_count();
//   Serial.printf("Message size: %u bytes\n", msg_size);
//   Serial.printf("Payload size per chunk: %u bytes\n", PAYLOAD_SIZE);
//   Serial.printf("Total chunks: %u\n", chunk_count);
  
//   // Send all chunks sequentially
//   Serial.println("\nSending chunks sequentially...");
//   size_t chunks_sent = 0;
  
//   // Disable detailed packet logging for large transfers
//   if (chunk_count > 10) {
//     client.setLogging(false);
//   }
  
//   while (parser.has_next()) {
//     const chunk_t* curr_chunk = parser.get_chunk();
    
//     if (curr_chunk) {
//       // Get current sequence number
//       uint8_t seq_num = parser.get_seq_num() == 0 ? chunk_count - 1 : parser.get_seq_num() - 1;
      
//       // Send packet
//       bool sent = client.sendPacket(curr_chunk, msg_id, client_id, seq_num, timestamp);
      
//       if (sent) {
//         chunks_sent++;
        
//         // Simulated reception
//         receiver.processPacket(
//           ntohs(curr_chunk->checksum),
//           msg_id,
//           client_id,
//           seq_num,
//           curr_chunk->size,  // Already in network byte order
//           timestamp,
//           curr_chunk->data
//         );
        
//         // Print progress periodically
//         if (chunks_sent % 10 == 0 || chunks_sent == chunk_count) {
//           Serial.printf("Progress: %u/%u chunks sent\n", chunks_sent, chunk_count);
//         }
//       } else {
//         Serial.println("Error: Failed to send chunk");
//         break;
//       }
//     } else {
//       Serial.println("Error: Failed to get chunk");
//       break;
//     }
//   }
  
//   // Re-enable logging
//   client.setLogging(true);
  
//   // Verify results
//   Serial.println("\nTest Results:");
//   Serial.printf("Total chunks sent: %u/%u\n", chunks_sent, chunk_count);
//   Serial.printf("Total bytes sent: %u\n", client.getBytesWritten());
//   Serial.printf("Message reassembly complete: %s\n", 
//                 receiver.isComplete() ? "YES" : "NO");
//   Serial.printf("Chunks received: %u/%u\n", 
//                 receiver.getReceivedChunks(), receiver.getTotalChunks());
                
//   // Verify data integrity
//   bool data_match = true;
//   const uint8_t* received_data = receiver.getData();
//   for (size_t i = 0; i < msg_size; i++) {
//     if (msg[i] != received_data[i]) {
//       Serial.printf("Data mismatch at offset %u: expected %02X, got %02X\n", 
//                    i, (uint8_t)msg[i], received_data[i]);
//       data_match = false;
//       break;
//     }
//   }
  
//   Serial.printf("Data integrity check: %s\n", data_match ? "PASSED" : "FAILED");
  
//   // Only print the complete message if all tests passed and specifically requested
//   if (data_match && receiver.isComplete() && false) { // Set to true to print message
//     receiver.printCompleteMessage();
//   }
// }

// // Function to test random access to chunks
// void testRandomAccess(const char* msg, size_t msg_size, 
//                      uint8_t msg_id, uint8_t client_id, uint32_t timestamp) {
//   Serial.println("\n=== RANDOM ACCESS TEST ===\n");
  
//   // Create parser and receiver
//   Parser<uint8_t> parser((uint8_t*)msg, msg_id, msg_size, client_id, timestamp);
//   MessageReceiver receiver(msg_size + PAYLOAD_SIZE);
  
//   uint8_t chunk_count = parser.get_count();
//   Serial.printf("Total chunks: %u\n", chunk_count);
  
//   // Test accessing chunks in random order
//   const int TEST_COUNT = min(chunk_count, (uint8_t)10); // Test up to 10 chunks
  
//   // Generate random chunk indices to test
//   uint8_t test_indices[TEST_COUNT];
//   for (int i = 0; i < TEST_COUNT; i++) {
//     test_indices[i] = random(0, chunk_count);
//     Serial.printf("Selected random chunk #%d for testing\n", test_indices[i]);
//   }
  
//   // Access chunks in random order
//   for (int i = 0; i < TEST_COUNT; i++) {
//     uint8_t idx = test_indices[i];
    
//     Serial.printf("\nRequesting chunk #%d...\n", idx);
//     const chunk_t* chunk = parser.get_chunk(idx);
    
//     if (chunk) {
//       uint16_t size = ntohs(chunk->size);
//       Serial.printf("Got chunk #%d, size: %u bytes\n", idx, size);
      
//       // Print the first 20 chars of data
//       Serial.print("Data sample (first 20 chars): ");
//       char preview[21];
//       int chars_to_copy = min(size, (uint16_t)20);
//       memcpy(preview, chunk->data, chars_to_copy);
//       preview[chars_to_copy] = '\0';
//       Serial.print(preview);
//       Serial.println("...");
      
//       // Process this chunk in the receiver
//       receiver.processPacket(
//         ntohs(chunk->checksum),
//         msg_id,
//         client_id,
//         idx,
//         chunk->size,
//         timestamp,
//         chunk->data
//       );
      
//       Serial.printf("Receiver status: %d/%d chunks received\n", 
//                     receiver.getReceivedChunks(), receiver.getTotalChunks());
//     } else {
//       Serial.printf("Error: Failed to get chunk #%d\n", idx);
//     }
//   }
  
//   // Print test result but don't automatically print the complete message
//   Serial.printf("\nRandom access test: %s\n", 
//                 (receiver.getReceivedChunks() == TEST_COUNT) ? "PASSED" : "FAILED");
                
//   // Only print the complete message if explicitly requested
//   if (receiver.isComplete() && false) { // Set to true to print message
//     receiver.printCompleteMessage();
//   }
// }

// // Function to test error conditions
// void testErrorConditions() {
//   Serial.println("\n=== ERROR HANDLING TEST ===\n");
  
//   // Test null message pointer
//   Serial.println("Testing null message pointer...");
//   Parser<uint8_t> null_parser(nullptr, 1, 100, 1, 0);
//   if (null_parser.get_count() == 0) {
//     Serial.println("PASSED: Null message pointer correctly handled");
//   } else {
//     Serial.println("FAILED: Null message pointer not detected");
//   }
  
//   // Test zero size message
//   Serial.println("\nTesting zero size message...");
//   uint8_t dummy_data = 0;
//   Parser<uint8_t> zero_parser(&dummy_data, 1, 0, 1, 0);
//   if (zero_parser.get_count() == 0) {
//     Serial.println("PASSED: Zero size message correctly handled");
//   } else {
//     Serial.println("FAILED: Zero size message not detected");
//   }
  
//   // Test out of bounds sequence number access
//   Serial.println("\nTesting out of bounds sequence number...");
//   uint8_t small_msg[10] = {0};
//   Parser<uint8_t> small_parser(small_msg, 1, sizeof(small_msg), 1, 0);
//   const chunk_t* invalid_chunk = small_parser.get_chunk(255); // MAX_SEQ_NUM
//   if (!invalid_chunk) {
//     Serial.println("PASSED: Out of bounds sequence number correctly handled");
//   } else {
//     Serial.println("FAILED: Out of bounds sequence number not detected");
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   while (!Serial) {
//     ; // Wait for serial port to connect
//   }
  
//   Serial.println("\n\n*** TinyGSM Parser Test Suite with Romeo and Juliet ***\n");
  
//   // Initialize Romeo and Juliet text
//   const char* text = initRomeoAndJulietText();
//   if (!romeo_and_juliet_text) {
//     Serial.println("Error: Failed to allocate memory for test message");
//     return;
//   }
  
//   // Initialize random number generator
//   randomSeed(analogRead(0));
  
//   // Test parameters
//   const uint8_t msg_id = 1;
//   const uint8_t client_id = 2;
//   const uint32_t timestamp = 1714473600; // Example timestamp
  
//   // Run the tests with Romeo and Juliet text
//   testSequentialTransmission(romeo_and_juliet_text, TEST_MSG_SIZE, msg_id, client_id, timestamp);
//   Serial.printf("=================COMPLETE==============:\n%s", text);
//   testRandomAccess(romeo_and_juliet_text, TEST_MSG_SIZE, msg_id, client_id, timestamp);
//   testErrorConditions();
  
//   // Clean up
//   delete[] romeo_and_juliet_text;
//   romeo_and_juliet_text = nullptr;
  
//   Serial.println("\n*** Test Suite Complete ***");
// }

// void loop() {
//   // Nothing to do in the loop for this test
//   delay(1000);
// }
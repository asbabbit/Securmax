/**
 * @file cbuffer.tpp
 * @brief Implementation of templated CircularBuffer methods
 *
 * This file contains the implementations of all template methods
 * defined in cbuffer.h. It is included at the end of the header
 * file rather than compiled separately.
 */

 template <typename T>
 void CircularBuffer<T>::init() {
     head = tail = 0;
     // Initialize all buffer pointers to nullptr
     for (size_t i = 0; i < BUFFER_SIZE; i++) {
         buffer[i] = nullptr;
     }
 }
 
 template <typename T>
 bool CircularBuffer<T>::empty() const {
     return head == tail;
 }
 
 template <typename T>
 void CircularBuffer<T>::add(T* item) {
    Serial.printf("   Before add: head=%d, tail=%d, full=%d\n", head, tail, full());
     if (full()) {
         // If buffer is full, advance tail to make room
         tail = ((tail + 1) & BUFFER_MASK);
         Serial.printf("   Buffer full, advancing tail to %d\n", tail);
     }
     buffer[head] = item;
     head = ((head + 1) & BUFFER_MASK);
     Serial.printf("    After add: head=%d, tail=%d, full=%d\n", head, tail, full());
 }
 
 template <typename T>
 T* CircularBuffer<T>::get() {
    Serial.printf("   Before get: head=%d, tail=%d, full=%d\n", head, tail, full());
     if (empty()) {
         return nullptr;
     }
     T* item = buffer[tail];
     tail = ((tail + 1) & BUFFER_MASK);
     Serial.printf("   After get: head=%d, tail=%d, full=%d\n", head, tail, full());
     return item;
 }
 
 /*
 * Examples with BUFFER_SIZE = 4, BUFFER_MASK = 3(011):
 * | Head | Tail | Diff | AND  | Full? |
 * |------|------|------|------|-------|
 * |  3   |  0   | 011  | 011  | true  |
 * |  0   |  1   | 111  | 011  | true  |
 * |  0   |  2   | 110  | 010  | false |
 * |  2   |  1   | 001  | 001  | false |
*/
 template <typename T>
 bool CircularBuffer<T>::full() const {
     return ((head - tail) & BUFFER_MASK) == BUFFER_MASK;
 }
 
 template <typename T>
 void CircularBuffer<T>::clear() {
     head = tail;
 }
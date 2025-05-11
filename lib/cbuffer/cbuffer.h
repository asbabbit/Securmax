#ifndef CBUFFER_H
#define CBUFFER_H

#include <cstdint>
#include <cstddef>
#include "HardwareSerial.h"

/**
 * @file cbuffer.h
 * @brief A power-of-2 sized circular buffer implementation with templated element type
 *
 * This circular buffer uses a power-of-2 size to optimize performance.
 * This allows the use of bitwise AND operations instead of modulo,
 * which is significantly faster.
 */

/**
 * @brief Default buffer size (must be a power of 2)
 */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE (8)
#endif

/**
 * @brief Verifies that BUFFER_SIZE is a power of 2
 */
#if (BUFFER_SIZE & (BUFFER_SIZE - 1)) != 0
#error "BUFFER_SIZE must be a power of two"
#endif

/**
 * @brief Bit mask for wrapping indices (equals BUFFER_SIZE-1)
 *
 * Used with bitwise AND to efficiently handle index wrapping.
 */
#define BUFFER_MASK (BUFFER_SIZE-1)

/**
 * @brief Templated circular buffer class
 * 
 * @tparam T The type of elements to store in the buffer
 */
template <typename T>
class CircularBuffer {
private:
    T* buffer[BUFFER_SIZE];  /**< Data storage array */
    size_t head;             /**< Write index (where new data goes) */
    size_t tail;             /**< Read index (where data comes from) */

public:
    /**
     * @brief Initialize a circular buffer
     *
     * Sets the head(write) and tail(read) indices to zero.
     */
    void init();

    /**
     * @brief Get the next item from the circular buffer
     *
     * Retrieves the value at the tail(read) index and advances the tail.
     * Call empty() first to ensure there is data available.
     *
     * @return The value from the tail(read) position
     */
    T* get();

    /**
     * @brief Add item to the circular buffer
     *
     * @param item Pointer to the item to add
     */
    void add(T* item);

    /**
     * @brief Check if the circular buffer is empty
     *
     * A buffer is empty when head(write) and tail(read) indices are equal,
     * meaning there are no items available to read.
     *
     * @return true if empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Check if the circular buffer is full
     *
     * A buffer is full when the next write would overwrite unread data.
     * This is efficiently determined using a bitwise AND with BUFFER_MASK.
     *
     * @return true if full, false otherwise
     */
    bool full() const;

    /**
     * @brief Clear all data from the circular buffer
     *
     * Resets the buffer by setting the head(write) index equal to
     * the tail(read) index, marking the buffer as empty.
     * Does not modify the actual data in the buffer.
     */
    void clear();

    /**
     * @brief Return head of buffer
     */
    size_t get_head() const { return head; }

    /**
     * @brief Return tail of buffer
     */
    size_t get_tail() const { return tail; }
};

#include "cbuffer.tpp"  // Include template implementation

#endif // CBUFFER_H
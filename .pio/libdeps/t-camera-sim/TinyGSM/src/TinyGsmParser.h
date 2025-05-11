#ifndef TINYGSM_PARSER_H
#define TINYGSM_PARSER_H

#include <Arduino.h>
#include <cstdint>
#include <type_traits>

// Network protocol constants
constexpr uint16_t UDP_HEADER_SIZE = 8;
constexpr uint16_t IP_HEADER_SIZE = 20;
constexpr uint16_t APP_HEADER_SIZE = 3;
constexpr uint16_t PROTOCOL_OVERHEAD = UDP_HEADER_SIZE + IP_HEADER_SIZE + APP_HEADER_SIZE;
constexpr uint16_t METADATA_HEADER_SIZE = 7;  // time 4B + chunk_count 1B + msg_size 2B

#if !defined(MTU)
#define MTU 1500
#endif

// Calculate actual payload size
constexpr uint16_t PAYLOAD_SIZE = MTU - PROTOCOL_OVERHEAD;

// Guesstimated max message size in bytes
#if !defined(MAX_SIZE)
#define MAX_SIZE 30720
#endif

// Maximum sequence number
constexpr uint8_t MAX_SEQ_NUM = 255;

// ESP32 error handling
#define PARSER_ERR_BASE 0x1000
#define PARSER_ERR_TOO_MANY_CHUNKS (PARSER_ERR_BASE + 1)
#define PARSER_ERR_NULL_MESSAGE (PARSER_ERR_BASE + 2)
#define PARSER_ERR_ZERO_SIZE (PARSER_ERR_BASE + 3)
#define PARSER_ERR_INVALID_CHUNK (PARSER_ERR_BASE + 4)
#define PARSER_ERR_MESSAGE_TOO_LARGE (PARSER_ERR_BASE + 5)
#define PARSER_ERR_OUT_OF_BOUNDS (PARSER_ERR_BASE + 6)
#define PARSER_ERR_INVALID_SEQ_NUM (PARSER_ERR_BASE + 7)

// TODO: Add checksum functionality, handle chunk lose over UDP
typedef struct
{
  uint16_t data_size;
  uint8_t seq;
  uint8_t *data;
} chunk_t;

typedef struct
{
  uint32_t timestamp;     // Timestamp used for unique identifier
  uint8_t chunk_count;
  size_t msg_size;
} meta_t;

// T must be contiguously assigned and copyable
template <typename T>
class Parser
{
private:
  const T *msg;
  uint16_t offset;
  uint8_t curr_seq;
  meta_t meta;
  chunk_t chunk;

  /**
   * @brief Calculate number of chunks needed for a message
   * @return number of chunks in message
   */
  uint8_t chunk_count() const
  {
    return static_cast<uint8_t>((meta.msg_size + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE);
  }

  /**
   * @brief Validate the message pointer and size
   * @return True if message is valid
   */
  bool valid_msg() const
  {
    return msg != nullptr && meta.msg_size > 0;
  }

  /**
   * @brief Checksum of chunk
   */
  // uint16 checksum()
  //{
  //
  //}

public:
  /**
   * @brief Default constructor
   */
  Parser()
      : msg(nullptr), offset(0), curr_seq(0)
  {
    meta = {};
    chunk = {};
  }

  /**
   * @brief Destructor
   */
  ~Parser() = default;

  // Delete copy constructor and assignment operator
  Parser(const Parser &) = delete;
  Parser &operator=(const Parser &) = delete;

  /**
   * @brief Initialize parser with a new message
   * @param msg_ Pointer to message data
   * @param msg_size_ Size of message in bytes
   * @param timestamp_ Message timestamp
   * @return ESP_OK if successful, error code otherwise
   */
  esp_err_t init(const T *msg_, size_t msg_size_, uint32_t timestamp_ = 0)
  {
    if (!msg_)
    {
      log_e("Null message pointer");
      return PARSER_ERR_NULL_MESSAGE;
    }

    if (msg_size_ == 0)
    {
      log_e("Message size cannot be 0");
      return PARSER_ERR_ZERO_SIZE;
    }

    if (msg_size_ > MAX_SIZE)
    {
      log_e("Message too large: %u > %u", msg_size_, MAX_SIZE);
      return PARSER_ERR_MESSAGE_TOO_LARGE;
    }

    // Initialize member variables
    msg = msg_;
    offset = 0;
    curr_seq = 0;

    // Reset
    meta = {};
    chunk = {};

    meta.timestamp = timestamp_;
    meta.msg_size = msg_size_;
    meta.chunk_count = chunk_count();

    return ESP_OK;
  }

  /**
   * @brief Parse new message (wrapper for init)
   * @param msg_ Pointer to message data
   * @param msg_size_ Size of message in bytes
   * @param timestamp_ Message timestamp
   * @return ESP_OK if successful, error code otherwise
   */
  esp_err_t new_msg(const T *msg_, size_t msg_size_, uint32_t timestamp_ = 0)
  {
    return init(msg_, msg_size_, timestamp_);
  }

  // Getters
  uint8_t get_count() const
  {
    return meta.chunk_count;
  }

  uint8_t get_seq() const
  {
    return curr_seq;
  }

  const meta_t *get_meta() const 
  {
    return &meta;
  }

  /**
   * @brief Used to check if more chunks need processing
   * @return True if curr_seq is less than chunk_count
   */
  bool has_next() const
  {
    return curr_seq < meta.chunk_count;
  }

  /**
   * @brief Get the next chunk for processing
   * @return Pointer to initialized chunk or nullptr on error
   */
  const chunk_t *get_chunk()
  {
    if (!valid_msg())
    {
      log_w("Invalid state: curr_seq=%u, chunk_count=%u, offset=%u, msg_size=%u",
            curr_seq, meta.chunk_count, offset, meta.msg_size);
      return nullptr;
    }

    // Calculate size for current chunk
    chunk.data_size = (curr_seq < meta.chunk_count - 1)
                          ? PAYLOAD_SIZE
                          : meta.msg_size - PAYLOAD_SIZE * (meta.chunk_count - 1);

    if (offset + chunk.data_size > meta.msg_size)
    {
      log_e("Chunk access would exceed message bounds: offset=%u, chunk_size=%u, msg_size=%u",
            offset, chunk.data_size, meta.msg_size);
      return nullptr;
    }

    // Set data pointer to current offset in message
    chunk.data = reinterpret_cast<uint8_t *>(const_cast<T *>(msg)) + offset;

    // Set sequence number
    chunk.seq = curr_seq;

    // Update state for next call
    offset += PAYLOAD_SIZE;
    curr_seq++;

    return &chunk;
  }

  /**
   * @brief Get a specific chunk by sequence number
   * @param seq_ Sequence number of chunk to get
   * @return Call to get_chunk()
   */
  const chunk_t *get_chunk(uint8_t seq_)
  {
    // Update sequence and offset
    curr_seq = seq_;
    offset = static_cast<uint16_t>(seq_) * PAYLOAD_SIZE;
    return get_chunk();
  }
};

#endif // TINYGSM_PARSER_H
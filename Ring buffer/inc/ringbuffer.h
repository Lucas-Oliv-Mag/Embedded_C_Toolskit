#pragma once

/**
 * @file ringbuffer.h
 * @author Lucas O. Magalhaes
 * 
 * @brief  containing the definitions of the ring buffer structure, 
 * enumeration for return messages, and prototypes for the library's functions.
 * 
 * @details This libraby is a implementation of a ring buffer
 *  algorithm, it is designed to be used in embedded systems 
 *  without RTOS, specifically in bare-metal applications,
 *  with low cost MCUs, and it is optimized for flexibily
 *  ever kind of data is supported, since raw bytes or 
 *  complex structures can be manipulated with the same APIS.
 * 
 * @note This library have a configurable behavior, if the user 
 *  define #define _RB_OPTIMIZED_VERSION before including the
 *  header file, the library will use an optimized version of 
 *  the push and pop functions that are faster but only work with
 *  buffer sizes that are a power of 2. Also the user can choose if the 
 *  buffer will overwrite old data when it is full, or if it will block 
 *  new data until there is space available.
 *  Also, the user can choose if the library will consider the padding of the data, 
 *  meaning that if the user tries to push a block of data that is larger than the 
 *  available space, the library will return an error instead of pushing only part of the data.
 * 
 * @warning If using the optimized version, the buffer size must be a power of 2, otherwise the library will return an error.
 * @version 0.1
 * @date 2026-06-04
*/


#pragma region includes

  #include <stdbool.h>
  #include <stdint.h>
  #include <stddef.h>

#pragma end region

#ifdef __cplusplus
  extern "C"{
#endif

/**
 * @brief Enumeration return mensages for ring buffer library operation results.
 */
enum rb_msg{

  RB_PARAM_ERR,                // returned when a required parameter is NULL or has an invalid value.
  RB_BASE_2_ERR,               // returned when the buffer size is not a power of 2 (optimized version only).
  RB_NULL_ERR,                 // returned when the ring_buffer pointer itself is NULL.
  RB_FULL_ERR,                 // returned when the buffer has no space left and overwrite is disabled.
  RB_WITHOUT_SPACE,            // returned when the requested push size exceeds the available free space.
  RB_BUF_LACKING,              // returned when a pop was partially fulfilled due to insufficient data in the buffer.
  RB_BLK_LACKING,              // returned when the buffer does not have enough data for a full block read (padding_matters enabled).
  RB_EMPY_ERR,                 // returned when a pop is attempted on an empty buffer.
  RB_WRAP_AROUND,              // returned when the tail pointer wraps around to the beginning of the buffer.
  RB_SUCESS,                   // returned when the operation completed successfully.
  RB_LOCKED                    // returned when the buffer is already locked by another operation.

};

/**
 * @brief Handler a ring buffer, and it's used with APIs to driven the buffer.
 * 
 */
struct ring_buffer{

  volatile bool lock; // indicate when the 

  bool overwrite; //wrap around enable.
  bool padding_matters; // block the operations that can desaligned the data on the buffer.

  uint16_t tail;
  uint16_t head;

  uint16_t size;
  uint16_t counter;

  uint8_t * buffer; // pointer for the raw buffer.

  void (*rb_callback)(void); // usefull when using multi-cores systems.

};

#pragma region Prototypes

  /**
   * @brief Initialize a ring buffer handler struct.
   * 
   * @param ptr pointer for the ring buffer handler
   * @param max_size the maximum size of the ring buffer in bytes.
   * @param buffer pointer to the raw buffer that the ring buffer will use for storage, it must be at least max_size bytes long.
   * @return enum rb_msg a mensage indicating the result of the operation, RB_SUCESS if the initialization was successful, or an error code if it failed.
   */
  enum rb_msg ringbf_init(struct ring_buffer * ptr, const uint16_t max_size, const uint8_t* buffer);

  
  /**
   * @brief 
   * 
   * @param ptr pointer to ring buffer handler
   * @param overwrite enable the ring buffler wrap around
   * @param padding_matters it means that the operations will comply with the parameters passed to them.
   * @param critical_section function pointer for callbacks, even a critical operation ill occurs.
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_config(struct ring_buffer * ptr, bool overwrite, bool padding_matters);

  
  /**
   * @brief Clear a ring buffer.
   * 
   * @param ptr pointer for the ring buffer to be clear
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_clear(struct ring_buffer * ptr);
    
  /**
   * @brief Push into a ring buffer data.
   * 
   * @param ptr Pointer for ring buffer handler
   * @param data_in pointer to the data, that ill put on ring buffer
   * @param lenght_in_bytes how many buffers your data have.
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_push(struct ring_buffer * ptr, const void * data_in, uint16_t lenght_in_bytes);

  
  /**
   * @brief read and exclude data from a ring buffer.
   * 
   * @param ptr pointer to the ring buffer handler.
   * @param data_out your array for store the data.
   * @param lenght_in_bytes how much bytes to retrieve.
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_pop(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes);

  /**
   * @brief read data from a ring buffer.
   * 
   * @param ptr pointer to the ring buffer handler.
   * @param data_out your array for store the data.
   * @param lenght_in_bytes how much bytes to retrieve.
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_peek(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes);
    
  /**
   * @brief Return the avaliable space on a ring buffer in bytes
   * 
   * @param ptr 
   * @param data_out 
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_avaliable(struct ring_buffer * ptr, uint16_t * data_out);

  /**
   * @brief Return the used space on a ring buffer in bytes
   * 
   * @param ptr 
   * @param data_out 
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_used(struct ring_buffer * ptr, uint16_t * data_out);

#pragma endregion


#pragma region Behavior definitions

  // These functions are defined as weak symbols, allowing users to provide their own implementations for critical section handling if needed. If not provided, these default empty implementations will be used.

  __attribute__((weak)) void rb_critical_section(void); // Default empty implementation for critical section entry.  
  __attribute__((weak)) void rb_critical_exit(void);

#pragma endregion

#ifdef __cplusplus
  }
#endif

// Ring buffer lib end.
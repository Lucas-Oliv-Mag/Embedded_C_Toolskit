#pragma once

/**
 * @file ringbuffer.h
 * @author Lucas O. Magalhaes
 * 
 * @brief  This file contains the definitions of the ring buffer structure, 
 *  enumeration for return messages, and prototypes for the library's functions.
 * 
 * @details This library is a implementation of a ring buffer
 *  it is designed to be used in embedded systems 
 *  without RTOS, specifically in bare-metal applications,
 *  with low cost MCUs, and it is optimized for flexibility
 *  every kind of data is supported, since raw bytes or 
 *  complex structures can be manipulated with the same APIs.
 * 
 * @note Use the ringbuffer_cfg.h file to configure the behavior of the library, 
 *  such as enabling optimized versions of functions and defining critical section
 *  handling for thread safety.
 * 
 * @note This library has a configurable behavior, if the user 
 *  defines #define _RB_OPTIMIZED_VERSION before including the
 *  header file, the library will use an optimized version of 
 *  the push and pop functions that are faster, but only work with
 *  buffer sizes that are a power of 2. Also the user can choose if the 
 *  buffer will overwrite old data when it is full, or if it will block 
 *  new data until there is space available.
 *  Also, the user can choose the behavior in edge cases:
 *    The library consider the data alignment when padding_matters is true, 
 *  meaning that if the user tries to push a block of data that is larger than the 
 *  available space, the library will return an error instead of pushing only part of the data.
 *  also when popping data, if the user tries to pop a block of data that is larger than the 
 *  available data, the library will return an error instead of popping only part of the data.
 * 
 * @warning If using the optimized version, the buffer size must be a power of 2, otherwise the library will return an error.
 * @version 0.1
 * @date 2026-06-19
*/


#pragma region includes

  #include <stdbool.h>
  #include <stdint.h>
  #include <stddef.h>
  #include "ringbuffer_cfg.h"

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
  RB_EMPTY_ERR,                 // returned when a pop is attempted on an empty buffer.
  RB_WRAP_AROUND,              // returned when the tail pointer wraps around to the beginning of the buffer.
  RB_SUCCESS,                   // returned when the operation completed successfully.
  RB_LOCKED                    // returned when the buffer is already locked by another operation.

};

/**
 * @brief Ring buffer handler, and it is used with APIs to drive the buffer.
 * 
 */
struct ring_buffer{

  volatile bool lock; // indicate when the wrap around enable with true.

  bool overwrite; //wrap around enable.
  bool padding_matters; // block the operations that can desaligned the data on the buffer.

  uint16_t tail; // index for the next write operation.
  uint16_t head; // index for the next read operation.

  uint16_t size;  // total size of the buffer in bytes.
  uint16_t counter; // number of bytes currently stored in the buffer, it is updated on push and pop operations.

  uint8_t * buffer; // pointer for the raw buffer.

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
   * @param lenght_in_bytes how many data to push, at the ent of the operation,
   *  this variable will be updated with the actual amount of bytes pushed 
   *  into the buffer, which can be less than the requested amount if there is
   *  not enough free space and overwrite is disabled.
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_push(struct ring_buffer * ptr, const void * data_in, uint16_t* lenght_in_bytes);

  
  /**
   * @brief read and exclude data from a ring buffer.
   * 
   * @param ptr pointer to the ring buffer handler.
   * @param data_out your array for store the data.
   * @param lenght_in_bytes how much bytes to retrieve, at the ent of the operation,
   *  this variable will be updated with the actual amount of bytes popped from the buffer,
   * @return enum rb_msg 
   */
  enum rb_msg ringbf_pop(struct ring_buffer * ptr, void * data_out, uint16_t* lenght_in_bytes);

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
  enum rb_msg ringbf_available(struct ring_buffer * ptr, uint16_t * data_out);

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
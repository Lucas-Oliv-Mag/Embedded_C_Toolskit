#pragma once

/**
 * @file ringbuffer_cfg.h
 * @author Lucas O. Magalhaes
 * @brief This file contains the configuration options for the ring buffer library,
 *  such as enabling optimized versions of functions and defining critical section handling for thread safety.
 * @version 0.1
 */


#ifdef __cplusplus
  extern "C"{
#endif

#pragma region Configuration options

  #ifndef _RB_CUSTOM_DATA_TYPES

    /// @brief Define the data type for size and length parameters, You can change this to uint32_t or another type if needed, this directly affects the ram usage per instance.
    typedef uint16_t rb_size_t; // Define the data type for size and length parameters. You can change this to uint32_t or another type if needed.

  #endif
  
    /// #define __rb_optimized_version // Uncomment this line to enable optimized push and pop functions that are faster but only work with buffer sizes that are a power of 2.

#pragma endregion
#ifdef __cplusplus
  } 
#endif

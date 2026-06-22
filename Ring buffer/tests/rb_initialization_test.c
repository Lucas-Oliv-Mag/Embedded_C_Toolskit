#include "ringbuffer.h"
#include <stdio.h>
#include <assert.h>


/**
 * @file rb_initialization_test.c
 * @brief This file contains a simple test for the ring buffer library initialization and configuration functions.
 * @date 2026-06-19
 * @author Lucas O. Magalhaes
 */
int main(void){

  struct ring_buffer rb;

  uint8_t buffer[16] = {0};

  printf("\nStarting ring buffer initialization tests...\n");

  assert(ringbf_init(&rb, 16, buffer) == RB_SUCCESS); /// test the initialization of the ring buffer, it should return RB_SUCCESS if the initialization was successful, or an error code if it failed.

  assert(rb.buffer == buffer); /// test if the buffer pointer was correctly set, it should point to the buffer array.
  assert(rb.size == 16); /// test if the size of the buffer was correctly set, it should be 16 bytes.
  assert(rb.head == 0); /// test if the head index was correctly initialized, it
  assert(rb.tail == 0); /// test if the tail index was correctly initialized, it should be 0.
  assert(rb.counter == 0); /// test if the counter was correctly initialized, it should be 0.

  assert(ringbf_config(&rb, false, false) == RB_SUCCESS); /// test the configuration of the ring buffer, it should return RB_SUCCESS if the configuration was successful, or an error code if it failed.
  assert(rb.overwrite == false); /// test if the overwrite flag was correctly set, it should be false.
  assert(rb.padding_matters == false); /// test if the padding_matters flag was correctly
  
  printf("All initialization tests passed successfully.\n");
  
  return 0;
}
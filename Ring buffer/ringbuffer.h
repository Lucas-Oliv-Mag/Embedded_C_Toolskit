#pragma once

#include <stdbool.h>
#include <stdint.h>

/*_______________________________________________________
*                                                        *
*    Data: 20/05/2026                                    *
*                                                        *
*    Title:  Ring buffer algorithm                       *
*                                                        *
*    Author: Lucas O. Magalh?es.                         *
*                                                        *
*   Version: 0.1v                                        *
*                                                        *
*________________________________________________________*
*/




#ifdef __cplusplus
  extern "C"{
#endif 


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
 * @brief Handle a ring buffer, and it's used with APIs to driven the buffer.
 * 
 */
struct ring_buffer{

  volatile bool lock; // indicate when the 

  bool overwrite; //wrap around.
  bool padding_matters; // 

  uint16_t tail;
  uint16_t head;

  uint16_t size;
  uint16_t counter;

  uint8_t * buffer; // pointer for 

  void (*rb_critical_section)(void); // usefull when using multi-cores systems.

};



enum rb_msg ring_buffer_init(struct ring_buffer * ptr, const uint16_t max_size, const uint8_t* buffer);
enum rb_msg ring_buffer_clear(struct ring_buffer * ptr);
enum rb_msg ring_buffer_push(struct ring_buffer * ptr, const void * data_in, uint16_t lenght_in_bytes);
enum rb_msg ring_buffer_pop(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes);




enum rb_msg ring_buffer_init(struct ring_buffer * ptr, const uint16_t max_size, const uint8_t* buffer){

  enum rb_msg ret;

  ret = ring_buffer_clear(ptr);

  if(ret != RB_SUCESS) return ret;

  ptr->lock = true;

  ptr->size = max_size;
  ptr->buffer = (uint8_t *)buffer;

  ptr->lock = false;

  return RB_SUCESS;
}

/**
 * @brief 
 * 
 * @param ptr pointer to ring buffer handler
 * @param overwrite enable the ring buffler wrap around
 * @param padding_matters it means that the operations will comply with the parameters passed to them.
 * @param critical_section function pointer for callbacks, even a critical operation ill occurs.
 * @return enum rb_msg 
 */
enum rb_msg ring_buffer_config(struct ring_buffer * ptr, bool overwrite, bool padding_matters, void (*critical_section)(void)){
  
  if(ptr == NULL){ return  RB_NULL_ERR; }
  if(ptr->lock == false){ ptr->lock = true; }else{ return RB_LOCKED; }

  ptr->overwrite = overwrite;
  ptr->padding_matters = padding_matters;
  
  if( critical_section != NULL ) ptr->rb_critical_section = critical_section;

  ptr->lock = false;

  return RB_SUCESS;

}
/**
 * @brief Clear a ring buffer.
 * 
 * @param ptr pointer for the ring buffer to be clear
 * @return enum rb_msg 
 */
enum rb_msg ring_buffer_clear(struct ring_buffer * ptr){

  if(ptr == NULL){ return  RB_NULL_ERR; }
  if(ptr->lock == false){ ptr->lock = true; }else{ return RB_LOCKED; }

  ptr->tail = 0;
  ptr->head = 0;
  ptr->counter = 0;

  ptr->lock = false;

  return RB_SUCESS;
}

/**
 * @brief Push into a ring buffer data.
 * 
 * @param ptr Pointer for ring buffer handler
 * @param data_in pointer to the data, that ill put on ring buffer
 * @param lenght_in_bytes how many buffers your data have.
 * @return enum rb_msg 
 */
enum rb_msg ring_buffer_push(struct ring_buffer * ptr, const void * data_in, uint16_t lenght_in_bytes){

  if(ptr == NULL || lenght_in_bytes == 0) return RB_PARAM_ERR;

  if(ptr->lock == false){ ptr->lock = true;}else{ return RB_LOCKED;}

  if(ptr->overwrite == false && (lenght_in_bytes + ptr->counter) > ptr->size){

    ptr->lock = false;
    return RB_WITHOUT_SPACE;

  } 

  for(uint16_t index = 0U; index < lenght_in_bytes; index++){
  
      ptr->buffer[ ptr->tail ] = ((uint8_t *)data_in)[index];    
      ptr->tail = (ptr->tail + 1) % ptr->size;

  }

  ptr->counter = (ptr->counter + lenght_in_bytes > ptr->size) ? ptr->size : ptr->counter + lenght_in_bytes;

  ptr->lock = false;

  return RB_SUCESS;
}


/**
 * @brief read and exclude data from a ring buffer.
 * 
 * @param ptr pointer to the ring buffer handler.
 * @param data_out your array for store the data.
 * @param lenght_in_bytes how much bytes to retrieve.
 * @return enum rb_msg 
 */
enum rb_msg ring_buffer_pop(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes){


  if(ptr == NULL || lenght_in_bytes == 0 || data_out == NULL){ return RB_PARAM_ERR; }
  
  if(ptr->lock == false){ ptr->lock = true; }else{ return RB_LOCKED; }
  
  uint16_t pops_to_do = 0;
  bool lacking_flag = false;

  if(lenght_in_bytes > ptr->counter){

    if(ptr->padding_matters == true){
      
      ptr->lock = false;
      return RB_BLK_LACKING;
       
    }
    else{

      pops_to_do = ptr->counter;
      lacking_flag = true;
       
    }

  }else{ pops_to_do = lenght_in_bytes; }

  for(uint16_t index = 0U; index < pops_to_do; index++){

   *((uint8_t *)(((uint8_t*)data_out) + index)) = (ptr->buffer[ptr->head]);
    ptr->head = (ptr->head + 1) % ptr->size;

  }

  ptr->counter -= pops_to_do;

  ptr->lock = false;

  if(lacking_flag == true){ return RB_BUF_LACKING;}

  return RB_SUCESS;
}


/**
 * @brief read data from a ring buffer.
 * 
 * @param ptr pointer to the ring buffer handler.
 * @param data_out your array for store the data.
 * @param lenght_in_bytes how much bytes to retrieve.
 * @return enum rb_msg 
 */
enum rb_msg ring_buffer_peak(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes){


  if(ptr == NULL || lenght_in_bytes == 0 || data_out == NULL){ return RB_PARAM_ERR; }
  
  if(ptr->lock == false){ ptr->lock = true; }else{ return RB_LOCKED; }
  
  uint16_t pops_to_do = 0;
  bool lacking_flag = false;

  if(lenght_in_bytes > ptr->counter){

    if(ptr->padding_matters == true){
      
      ptr->lock = false;
      return RB_BLK_LACKING;
       
    }
    else{

      pops_to_do = ptr->counter;
      lacking_flag = true;
       
    }

  }else{ pops_to_do = lenght_in_bytes; }

  uint16_t temp_head = ptr->head;

  for(uint16_t index = 0U; index < pops_to_do; index++){

   *((uint8_t *)(((uint8_t*)data_out) + index)) = (ptr->buffer[temp_head]);
    temp_head = (temp_head + 1) % ptr->size;

  }

  ptr->lock = false;

  if(lacking_flag == true){ return RB_BUF_LACKING;}

  return RB_SUCESS;
}

/**
 * @brief Return the avaliable space on a ring buffer in bytes
 * 
 * @param ptr 
 * @param data_out 
 * @return enum rb_msg 
 */
enum rb_msg ring_buffer_avaliable(struct ring_buffer * ptr, uint16_t * data_out){

  if(ptr == NULL){ return RB_PARAM_ERR; }
  
  if(ptr->lock == false){ ptr->lock = true; }else{ return RB_LOCKED; }

  * data_out = ptr->size - ptr->counter;

  ptr->lock = false;

  return RB_SUCESS;
}





#ifdef __cplusplus
  }
#endif

// Ring buffer lib end.


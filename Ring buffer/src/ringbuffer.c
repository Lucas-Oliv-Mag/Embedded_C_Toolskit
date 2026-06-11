#include "ringbuffer.h"

#ifdef __cplusplus
  extern "C"{
#endif

#define _RB_LOCK_() do{rb_critical_section(); \
  if(ptr->lock == false){ ptr->lock = true; }else{ rb_critical_exit(); return RB_LOCKED; } \
  rb_critical_exit();}while(0)
// Ring buffer lock check and set.

#define _RB_POWER_OF_2_CHECK(x) ((x & (x - 1)) == 0)
// Check if a number is a power of 2.


//_____________________________________________________________________________________________

#pragma region Standart Fx


enum rb_msg ringbf_init(struct ring_buffer * ptr, const uint16_t max_size, const uint8_t* buffer){

  enum rb_msg ret;

  ret = ring_buffer_clear(ptr);

  #ifdef _RB_OPTIMIZED_VERSION // optimized version only works with buffer sizes that are a power of 2, because it uses bitwise specificaly operations.
    if(_RB_POWER_OF_2_CHECK(ptr->size)){ ptr->lock = false; return RB_BASE_2_ERR; }
  #endif

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
enum rb_msg ringbf_config(struct ring_buffer * ptr, bool overwrite, bool padding_matters){
  
  if(ptr == NULL){   rb_critical_exit();  return  RB_NULL_ERR; }
  
  _RB_LOCK_();
  
  ptr->overwrite = overwrite;
  ptr->padding_matters = padding_matters;
  
  ptr->lock = false;


  return RB_SUCESS;
}


/**
 * @brief Clear a ring buffer.
 * 
 * @param ptr pointer for the ring buffer to be clear
 * @return enum rb_msg 
 */
enum rb_msg ringbf_clear(struct ring_buffer * ptr){

  if(ptr == NULL){ return  RB_NULL_ERR; }

  _RB_LOCK_();


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
enum rb_msg ringbf_push(struct ring_buffer * ptr, const void * data_in, uint16_t lenght_in_bytes){

  if(ptr == NULL || lenght_in_bytes == 0) return RB_PARAM_ERR;

  _RB_LOCK_();

  #ifdef _RB_OPTIMIZED_VERSION // optimized version only works with buffer sizes that are a power of 2, because it uses bitwise specificaly operations.
    if(_RB_POWER_OF_2_CHECK(ptr->size)){ ptr->lock = false; return RB_BASE_2_ERR; }
  #endif


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
enum rb_msg ringbf_pop(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes){


  if(ptr == NULL || lenght_in_bytes == 0 || data_out == NULL){ return RB_PARAM_ERR; }
  
  _RB_LOCK_();

  
  #ifdef _RB_OPTIMIZED_VERSION // optimized version only works with buffer sizes that are a power of 2, because it uses bitwise specificaly operations.
    if(_RB_POWER_OF_2_CHECK(ptr->size)){ ptr->lock = false; return RB_BASE_2_ERR; }
  #endif

  uint16_t pops_to_do = 0;
  bool lacking_flag = false;

  if(lenght_in_bytes > ptr->counter){

    if(ptr->padding_matters == true){
      
      ptr->lock = false;
      rb_critical_exit();

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
enum rb_msg ringbf_peek(struct ring_buffer * ptr, void * data_out, uint16_t lenght_in_bytes){


  if(ptr == NULL || lenght_in_bytes == 0 || data_out == NULL){ return RB_PARAM_ERR; }
  
  _RB_LOCK_(); 
  

  #ifdef _RB_OPTIMIZED_VERSION // optimized version only works with buffer sizes that are a power of 2, because it uses bitwise specificaly operations.
    if(_RB_POWER_OF_2_CHECK(ptr->size)){ ptr->lock = false; return RB_BASE_2_ERR; }
  #endif

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
enum rb_msg ringbf_avaliable(struct ring_buffer * ptr, uint16_t * data_out){

  if(ptr == NULL){ return RB_PARAM_ERR; }
  
  _RB_LOCK_();

  * data_out = ptr->size - ptr->counter;

  ptr->lock = false;

  return RB_SUCESS;
}


#pragma end region

//____________________________________________________________________________________________



#pragma endregion




#ifdef __cplusplus
  }
#endif
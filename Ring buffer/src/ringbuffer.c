#include "ringbuffer.h"

#pragma region Macros

#define _RB_LOCK_() do{rb_critical_section(); \
  if(ptr->lock == false){ ptr->lock = true; }else{ rb_critical_exit(); return RB_LOCKED; } \
  rb_critical_exit();}while(0)
// Ring buffer lock check and set.

#define _RB_POWER_OF_2_CHECK(x) ((x & (x - 1)) == 0)
// Check if a number is a power of 2.

#pragma endregion

//_____________________________________________________________________________________________

#pragma region Standart Fx

enum rb_msg ringbf_init(struct ring_buffer * ptr, uint16_t max_size, const uint8_t* buffer){

  if(ptr == NULL || buffer == NULL || max_size == 0){ return RB_PARAM_ERR;  }

  rb_critical_section();

  ptr->lock = true;

  #ifdef _RB_OPTIMIZED_VERSION // optimized version only works with buffer sizes that are a power of 2, because it uses bitwise specificaly operations.
    if(!_RB_POWER_OF_2_CHECK(max_size)){ ptr->lock = false; return RB_BASE_2_ERR; }
  #endif
  
  ptr->size = max_size;
  ptr->buffer = (uint8_t *)buffer;
  ptr->tail = 0;
  ptr->head = 0;
  ptr->counter = 0;
  
  ptr->lock = false;

  rb_critical_exit();


  return RB_SUCCESS;
}



enum rb_msg ringbf_config(struct ring_buffer * ptr, bool overwrite, bool padding_matters){
  
  if(ptr == NULL){ return  RB_NULL_ERR; }
  
  _RB_LOCK_();

  
  ptr->overwrite = overwrite;
  ptr->padding_matters = padding_matters;
  
  ptr->lock = false;


  return RB_SUCCESS;
}



enum rb_msg ringbf_clear(struct ring_buffer * ptr){

  if(ptr == NULL){ return  RB_NULL_ERR; }

  _RB_LOCK_();


  ptr->tail = 0;
  ptr->head = 0;
  ptr->counter = 0;

  ptr->lock = false;


  return RB_SUCCESS;
}



enum rb_msg ringbf_push(struct ring_buffer * ptr, const void * data_in, uint16_t * length_in_bytes){

  if(ptr == NULL || lenght_in_bytes == NULL || data_in == NULL){ return RB_PARAM_ERR; }
  if(*lenght_in_bytes > ptr->size || *lenght_in_bytes == 0){ return RB_PARAM_ERR; }

  _RB_LOCK_();

  if(ptr->overwrite == false && (*lenght_in_bytes + ptr->counter) > ptr->size){

    if(ptr->padding_matters == true){
      
      ptr->lock = false;
      return RB_WITHOUT_SPACE;
       
    }else{

      *lenght_in_bytes = ptr->size - ptr->counter; // adjust the push size to fit the remaining space.

    }
    

  }else if(ptr->overwrite == true && (*lenght_in_bytes + ptr->counter) > ptr->size){

    uint16_t bytes_to_overwrite = (*lenght_in_bytes + ptr->counter) - ptr->size;

    #ifdef _RB_OPTIMIZED_VERSION
      ptr->head = (ptr->head + bytes_to_overwrite) & (ptr->size - 1); // optimized wrap-around using bitwise AND for power of 2 sizes.
    #else
      ptr->head = (ptr->head + bytes_to_overwrite) % ptr->size;
    #endif

    ptr->counter = ptr->size; 

  } 

  for(uint16_t index = 0; index < *lenght_in_bytes; index++){
  
      ptr->buffer[ ptr->tail ] = ((uint8_t *)data_in)[index];    
      #ifdef _RB_OPTIMIZED_VERSION
        ptr->tail = (ptr->tail + 1) & (ptr->size - 1); // optimized wrap-around using bitwise AND for power of 2 sizes.
      #else
        ptr->tail = (ptr->tail + 1) % ptr->size;
      #endif

  }

  ptr->counter = (ptr->counter + *lenght_in_bytes > ptr->size) ? ptr->size : ptr->counter + *lenght_in_bytes;

  ptr->lock = false;
  

  return RB_SUCESS;
}



enum rb_msg ringbf_pop(struct ring_buffer * ptr, void * data_out, uint16_t* length_in_bytes){


  if(ptr == NULL || length_in_bytes == NULL || data_out == NULL){ return RB_PARAM_ERR; }
  
  if(*length_in_bytes > ptr->size || *length_in_bytes == 0){ return RB_PARAM_ERR; }

  if(ptr->counter == 0){ return RB_EMPTY_ERR; }

  _RB_LOCK_();

  uint16_t pops_to_do = 0;
  bool lacking_flag = false;

  if(*length_in_bytes > ptr->counter){

    if(ptr->padding_matters == true){
      
      ptr->lock = false;

      return RB_BLK_LACKING;
       
    }
    else{

      *length_in_bytes = ptr->counter;
      lacking_flag = true;
       
    }
  }
    
  pops_to_do = *length_in_bytes; 

  for(uint16_t index = 0U; index < pops_to_do; index++){

   *((uint8_t *)(((uint8_t*)data_out) + index)) = (ptr->buffer[ptr->head]);
    #ifdef _RB_OPTIMIZED_VERSION
      ptr->head = (ptr->head + 1) & (ptr->size - 1); // optimized wrap-around using bitwise AND for power of 2 sizes.
    #else
      ptr->head = (ptr->head + 1) % ptr->size;
    #endif
  }

  ptr->counter -= pops_to_do;

  ptr->lock = false;

  if(lacking_flag == true){ return RB_BUF_LACKING;}


  return RB_SUCCESS;
}



enum rb_msg ringbf_peek(struct ring_buffer * ptr, void * data_out, uint16_t length_in_bytes){


  if(ptr == NULL || length_in_bytes == 0 || data_out == NULL){ return RB_PARAM_ERR; }
  
  _RB_LOCK_(); 
  

  uint16_t pops_to_do = 0;
  bool lacking_flag = false;

  if(length_in_bytes > ptr->counter){

    if(ptr->padding_matters == true){
      
      ptr->lock = false;
      return RB_BLK_LACKING;
       
    }
    else{

      pops_to_do = ptr->counter;
      lacking_flag = true;
       
    }

  }else{ pops_to_do = length_in_bytes; }

  uint16_t temp_head = ptr->head;

  for(uint16_t index = 0U; index < pops_to_do; index++){

   *((uint8_t *)(((uint8_t*)data_out) + index)) = (ptr->buffer[temp_head]);
    
    #ifdef _RB_OPTIMIZED_VERSION
      temp_head = (temp_head + 1) & (ptr->size - 1); // optimized wrap-around using bitwise AND for power of 2 sizes.
    #else
      temp_head = (temp_head + 1) % ptr->size;
    #endif
  }

  ptr->lock = false;

  if(lacking_flag == true){ return RB_BUF_LACKING;}

  return RB_SUCCESS;
}



enum rb_msg ringbf_available(struct ring_buffer * ptr, uint16_t * data_out){

  if(ptr == NULL || data_out == NULL){ return  RB_NULL_ERR;}
  
  _RB_LOCK_();

  * data_out = ptr->size - ptr->counter;

  ptr->lock = false;

  return RB_SUCCESS;
}


enum rb_msg ringbf_used(struct ring_buffer * ptr, uint16_t * data_out){

  if(ptr == NULL || data_out == NULL){ return RB_PARAM_ERR; }
  
  _RB_LOCK_();

  * data_out = ptr->counter;

  ptr->lock = false;

  return RB_SUCCESS;
}




#pragma endregion

#pragma region Critical Section Stubs

__attribute__((weak)) void rb_critical_section(void){ } // Default empty implementation for critical section entry.  


__attribute__((weak)) void rb_critical_exit(void)   { } // Default empty implementation for critical section exit.


#pragma endregion
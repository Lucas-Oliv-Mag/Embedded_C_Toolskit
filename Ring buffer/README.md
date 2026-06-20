# RING BUFFER ALGORITHM

Usefull for bare-metal envioriments, with deterministic behavior
and flexibility for any kind of data.

_can be utillized for build anothers data sctruct algorithms like Queues._


## Standard APIs
 * **Init** - Initialize the ring buffer handler (mandatory first step).
 * **Config** - Configue the behavior of the funcions when edge cases occurs.
 * **Push** - insert data into a buffer ring.
 * **Pop**  - read and exclude data from ring buffer.
 * **Peek** - read data without pop them.
 * **Avaliable** - return how many bytes in use now.
  
## warnings to avoid undefined behaviors

  - Have certain of the behavior of the ring buffer api's when they are next to full with the
    function _ringbf_config()_.

  - Only change the size of the buffer or any variable in the circular buffer handler until you  have a certain that no others function ill be executed, especially if the Optimized version is enabled.
  
  - Always check if the value of the parameter _Lenght_in_bytes_ changed after a pop or 
    push call, it means that your requisition partial occurs.

  - Don't try push in a the buffer data large than they lenght.


## Optimized version

 ### when using **#define _RB_OPTIMIZED_VERSION**, a power of two optimization with binary masks will be enable (be careful, the buffer size must match with power of two, or all optimized functions will return a error in wich return).

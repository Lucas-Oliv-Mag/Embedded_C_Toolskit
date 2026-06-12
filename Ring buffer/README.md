# RING BUFFER ALGORITHM

Usefull for bare-metal envioriments, with deterministic behavior
and flexibility for any kind of data.


## Standard APIs
 * **Init** - Initialize the ring buffer handler (mandatory first step).
 * **Push** - insert data into a buffer ring.
 * **Pop**  - read and exclude data from ring buffer.
 * **Peek** - read data without pop them.
 * **Avaliable** - return how many bytes in use now.
  
## Optimized version

 ### when using **#define _RB_OPTIMIZED_VERSION**, a power of two optimization with binary masks will be enable (be careful, the buffer size must match with power of two, or all optimized functions will return a error in wich return).

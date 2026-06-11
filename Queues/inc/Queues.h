#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum queue_report {

    QUEUE_UNINTIALIZED,
    QUEUE_BLOCKED,
    QUEUE_FULL,
    QUEUE_EMPYT,
    QUEUE_SUCESS,
    QUEUE_PARAM_ERR,
    QUEUE_PROCESS_ERR,
    QUEUE_POISONED

} queue_report_t;



typedef enum queue_msg_enum {

    QUEUE_BLANK     = 0x00U,      // Qeue nao existe
    QUEUE_DATA_8B   = 0x01U,      // Data tem um valor imediato de 8 bits.
    QUEUE_DATA_16B  = 0x02U,    // Data tem um valor imediato de 16 bits.
    QUEUE_DATA_32B  = 0x03U,    // Data tem um valor imediato de 32 bits.
    QUEUE_POINTER   = 0x04U,    // Data aponta para um endereco de memoria.
    QUEUE_MENSSAGE  = 0x05U,     // Data eh uma mensagem
    QUEUE_CUSTOM    = 0x06U     //  Data eh uma mensagem customizada.

} queue_msg_t;



typedef struct queue_msg_holder {

    struct {

        uint8_t  type:8; // Handler de queue_msg_t (indica do que se trata a queue)
        uint8_t  custom:8;   // costumizado para sua queue
        uint16_t lenght:16; // Indica o tamanho da memoria no ponteiro quando type for pointer (opcional)
    
    } header;

    union {
        uint32_t raw;      // Valor imediato.
        void *ptr;         // Endereco de mem®ria para POINTER ou MESSAGE
    } data;

} queue_t;


typedef struct queues_list_holder {



    uint8_t status;

    uint8_t block_callback   :1;
    uint8_t unblock_callback :1;


    uint8_t lenght; // Tamanho total da list[]
    uint8_t  news; // Quantas mensagems na queue.

    uint8_t  head; // indice de leitura.
    uint8_t  tail; // indice de escrita.

    void (*callback_blocked)(); // Handler de call back quando a fila encher.
    void (*callback_unblock)(); // Handler de call back quando a fila voltar a ter espaco sobrando.

    queue_t * list[];

} queues_list_t;


queue_report_t queue_init(queues_list_t* holder, uint16_t queues_lenght, queue_t * list);
queue_report_t queue_callback(queues_list_t* holder, void (*fn_blocked)(), void (*fn_unblock)());
queue_report_t queue_push(queues_list_t* holder, queue_t * queue);
queue_report_t queue_pop(queues_list_t* holder, queue_t * receive);
queue_report_t queue_clear(queues_list_t* holder);
queue_report_t queue_receive(queues_list_t* holder, queue_t * queue);


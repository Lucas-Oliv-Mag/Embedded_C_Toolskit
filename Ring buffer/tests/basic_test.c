/**
 * test_ringbuffer_simple.c
 *
 * Primeiro teste da lib ringbuffer, escrito SEM nenhum framework de teste.
 * Usa apenas <assert.h>, que já vem com o compilador C.
 *
 * Como funciona um assert:
 *   assert(condicao);
 *   -> se "condicao" for verdadeira, o programa continua normalmente.
 *   -> se "condicao" for falsa, o programa para imediatamente e imprime
 *      no terminal o arquivo e a linha onde o assert falhou.
 *
 * Ou seja: cada assert() abaixo é uma "verificação". Se TODOS passarem,
 * o programa termina silenciosamente e imprime "TODOS OS TESTES PASSARAM!".
 * Se algum falhar, ele trava ali e te diz exatamente qual linha falhou.
 *
 * Isso ainda não é um framework de teste "de verdade" (não conta quantos
 * passaram, não roda um teste depois do outro se um falhar, etc.), mas é
 * o jeito mais simples de entender a MECÂNICA de um teste automatizado:
 * você define o que é "certo" em código, e deixa o computador checar
 * por você, em vez de você olhar manualmente.
 */
 
#include <assert.h>
#include <string.h>   // memcmp, pra comparar blocos de bytes
#include <stdio.h>    // printf, só pra mensagens de progresso
#include "ringbuffer.h"
 
// ---------------------------------------------------------------------
// TESTE 1: ringbf_init deve aceitar parâmetros válidos e configurar
// corretamente o estado inicial da struct.
// ---------------------------------------------------------------------
void test_init_com_parametros_validos(void) {
 
    printf("Rodando: test_init_com_parametros_validos... ");
 
    struct ring_buffer rb;          // a struct "handler" do buffer
    uint8_t storage[16];            // o array que vai guardar os dados de fato
 
    enum rb_msg resultado = ringbf_init(&rb, sizeof(storage), storage);
 
    // Verificação 1: a função deve retornar sucesso
    assert(resultado == RB_SUCESS);
 
    // Verificação 2: head e tail devem comecar zerados
    assert(rb.head == 0);
    assert(rb.tail == 0);
 
    // Verificação 3: counter (quantidade de bytes usados) deve ser zero
    assert(rb.counter == 0);
 
    // Verificação 4: o tamanho guardado deve ser o que passamos
    assert(rb.size == sizeof(storage));
 
    printf("OK\n");
}
 
// ---------------------------------------------------------------------
// TESTE 2: ringbf_init deve REJEITAR parâmetros inválidos (ptr nulo,
// buffer nulo, tamanho zero) e retornar o erro correto, sem travar.
// ---------------------------------------------------------------------
void test_init_com_parametros_invalidos(void) {
 
    printf("Rodando: test_init_com_parametros_invalidos... ");
 
    uint8_t storage[16];
    struct ring_buffer rb;
 
    // ptr (handler) nulo
    assert(ringbf_init(NULL, sizeof(storage), storage) == RB_PARAM_ERR);
 
    // buffer de armazenamento nulo
    assert(ringbf_init(&rb, sizeof(storage), NULL) == RB_PARAM_ERR);
 
    // tamanho zero
    assert(ringbf_init(&rb, 0, storage) == RB_PARAM_ERR);
 
    printf("OK\n");
}
 
// ---------------------------------------------------------------------
// TESTE 3: ciclo básico de push + pop, sem dar volta no buffer
// (sem "wraparound"). Empurra 4 bytes, lê os mesmos 4 bytes de volta,
// e confirma que são idênticos.
// ---------------------------------------------------------------------
void test_push_pop_basico(void) {
 
    printf("Rodando: test_push_pop_basico... ");
 
    struct ring_buffer rb;
    uint8_t storage[16];
    ringbf_init(&rb, sizeof(storage), storage);
    ringbf_config(&rb, false, false); // overwrite=false, padding_matters=false
 
    uint8_t dados_entrada[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t dados_saida[4]   = {0};
 
    enum rb_msg r_push = ringbf_push(&rb, dados_entrada, sizeof(dados_entrada));
    assert(r_push == RB_SUCESS);
 
    // depois do push, o buffer deve reportar 4 bytes em uso
    uint16_t usados = 0;
    ringbf_used(&rb, &usados);
    assert(usados == 4);
 
    enum rb_msg r_pop = ringbf_pop(&rb, dados_saida, sizeof(dados_saida));
    assert(r_pop == RB_SUCESS);
 
    // os bytes que saíram devem ser EXATAMENTE os que entraram
    assert(memcmp(dados_entrada, dados_saida, sizeof(dados_entrada)) == 0);
 
    // depois do pop, o buffer deve estar vazio de novo
    ringbf_used(&rb, &usados);
    assert(usados == 0);
 
    printf("OK\n");
}
 
// ---------------------------------------------------------------------
// TESTE 4: pop em buffer vazio, com padding_matters=true, deve falhar
// com o código de erro correto, e NÃO deve escrever nada em data_out.
// ---------------------------------------------------------------------
void test_pop_buffer_vazio(void) {
 
    printf("Rodando: test_pop_buffer_vazio... ");
 
    struct ring_buffer rb;
    uint8_t storage[16];
    ringbf_init(&rb, sizeof(storage), storage);
    ringbf_config(&rb, false, true); // padding_matters=true
 
    uint8_t saida[4] = {0};
    enum rb_msg r = ringbf_pop(&rb, saida, sizeof(saida));
 
    // buffer vazio + padding_matters=true -> deve recusar o pop
    assert(r == RB_BLK_LACKING);
 
    printf("OK\n");
}
 
// ---------------------------------------------------------------------
// main: roda todos os testes em sequência.
// Se chegar até o final sem nenhum assert falhar, todos passaram.
// ---------------------------------------------------------------------
int main(void) {
 
    test_init_com_parametros_validos();
    test_init_com_parametros_invalidos();
    test_push_pop_basico();
    test_pop_buffer_vazio();
 
    printf("\nTODOS OS TESTES PASSARAM!\n");
    return 0;
}
// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.5 -- Março de 2023

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <ucontext.h>		// biblioteca POSIX de trocas de contexto

#define DEFAULT_PRIORITY     0
#define AGING                1     
#define PRIORITY_UPPER_BOUND 20
#define PRIORITY_LOWER_BOUND -20
#define START_TICK_COUNT     20
#define TICK_INTERVAL        1 // miliseconds

typedef enum task_status
{ 
  READY, 
  FINISHED, 
  SUSPENDED,
  SLEEPING
} task_status;

typedef enum task_type
{
  SYSTEM, 
  USER,
} task_type;

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
  struct task_t *prev, *next ;		// ponteiros para usar em filas
  int id ;				// identificador da tarefa
  ucontext_t context ;			// contexto armazenado da tarefa
  task_status status ;			// pronta, rodando, suspensa, ...
  int vg_id ; // ID da pilha da tarefa no Valgrind
  int static_priority; // prioridade estática
  int dynamic_priority; // prioridade dinâmica
  int tick_counter; // contador de tick
  task_type type; // tipo de tarefa (sistema ou usuário)
  unsigned int start_time;    // tempo de criação da tarefa
  unsigned int cpu_time;      // tempo de cpu da tarefa
  unsigned int activations;   // numero de ativações da tarefa
  struct task_t *wait_for_task;   // task que está sendo aguardada
  int exit_code;
  unsigned int wake_time;
  // ... (outros campos serão adicionados mais tarde)
} task_t ;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif
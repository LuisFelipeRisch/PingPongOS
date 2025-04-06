#include "ppos.h"
#include "ppos_data.h"
#include "stdio.h"
#include "stdlib.h"

#define STACK_SIZE 64 * 1024

task_t main_task,
       *current_task = &main_task;
int    task_id_counter = 0;

void ppos_init() {
  main_task.id = task_id_counter++;
  setvbuf(stdout, 0, _IONBF, 0);
}

/* retorno: o ID (>0) da nova tarefa ou um valor negativo, se houver erro */
int task_init(task_t *task, void (*start_routine)(void *), void *arg) {
  char *stack;

  getcontext(&task->context);

  stack = malloc(STACK_SIZE);
  if (stack) {
    task->context.uc_stack.ss_sp    = stack;
    task->context.uc_stack.ss_size  = STACK_SIZE;
    task->context.uc_stack.ss_flags = 0;
    task->context.uc_link           = 0;
  } else {
    fprintf(stderr, "task_init: falha durante a criação da pilha!\n");
    return -1;
  }

  makecontext(&task->context, (void *)start_routine , 1, arg);

  task->id = task_id_counter++;

  #ifdef DEBUG
    fprintf(stdout, "task_init: iniciada tarefa %d\n", task->id);
  #endif

  return task->id;
}

/* retorno: 1 se pode trocar de task ou 0 se não pode */
int can_switch_to_task(task_t *task) {
  if (!task) {
    fprintf(stderr, "can_switch_to_task: task não pode ser nula!\n"); 
    return 0;
  }

  if (current_task == task) {
    fprintf(stderr, "can_switch_to_task: task não pode ser igual a task atual!\n"); 
    return 0;
  }

  return 1;
}

/* retorno: valor negativo se houver erro, ou zero */
int task_switch(task_t *task) {
  if (!can_switch_to_task(task))
    return -1;

  task_t *aux_task;
  
  aux_task     = current_task;
  current_task = task;

  #ifdef DEBUG
    fprintf(stdout, "task_switch: trocando contexto %d -> %d\n", aux_task->id, task->id);
  #endif
  
  swapcontext(&aux_task->context, &task->context);

  return 0;
}

void task_exit(int exit_code) {
  #ifdef DEBUG
    fprintf(stdout, "task_exit: tarefa %d sendo encerrada\n", current_task->id);
  #endif

  if (current_task == &main_task)
    exit(EXIT_SUCCESS);

  task_switch(&main_task);
}

/* retorno: Identificador numérico (ID) da tarefa corrente, que deverá ser 0 para main, ou um valor positivo para as demais tarefas */
int task_id() {
  #ifdef DEBUG
    fprintf(stdout, "task_id: tarefa atual é a de id %d\n", current_task->id);
  #endif

  return current_task->id;
}

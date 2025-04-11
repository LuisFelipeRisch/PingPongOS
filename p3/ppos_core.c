#include "ppos.h"
#include "ppos_data.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <valgrind/valgrind.h>

#define STACK_SIZE 64 * 1024

queue_t *ready_tasks;
task_t  main_task, dispatcher_task,
        *current_task = &main_task;
int     task_id_counter = 0;

void free_task_stack(task_t *task) {
  free(task->context.uc_stack.ss_sp);
  VALGRIND_STACK_DEREGISTER(task->vg_id);
}

void task_yield() {
  current_task->status = READY;
  queue_append((queue_t **) &ready_tasks, (queue_t *) current_task);

  task_switch(&dispatcher_task);
}

task_t *scheduler() {
  if (!queue_size(ready_tasks))
    return NULL; 
  
  task_t *next_task;
  
  next_task = (task_t *)ready_tasks; 

  queue_remove((queue_t **) &ready_tasks, (queue_t *) next_task);

  return next_task;
}

void dispatcher() {
  task_t *next_task; 

  while ((next_task = scheduler())){
    task_switch(next_task);

    switch (next_task->status)
    {
    case FINISHED:
      free_task_stack(next_task);
      break;
    default:
      break;
    }
  }

  task_exit(0);
}

void ppos_init() {
  setvbuf(stdout, 0, _IONBF, 0);

  task_init(&dispatcher_task, dispatcher, NULL);

  main_task.id     = task_id_counter++;
  main_task.status = READY;
  queue_append((queue_t **) &ready_tasks, (queue_t *) &main_task);

  task_switch(&dispatcher_task);
}

/* retorno: o ID (>0) da nova tarefa ou um valor negativo, se houver erro */
int task_init(task_t *task, void (*start_routine)(void *), void *arg) {
  char *stack;

  getcontext(&task->context);

  stack = malloc(STACK_SIZE);
  if (stack) {
    task->vg_id                     = VALGRIND_STACK_REGISTER(stack, stack + STACK_SIZE);
    task->context.uc_stack.ss_sp    = stack;
    task->context.uc_stack.ss_size  = STACK_SIZE;
    task->context.uc_stack.ss_flags = 0;
    task->context.uc_link           = 0;
  } else {
    fprintf(stderr, "task_init: falha durante a criação da pilha!\n");
    return -1;
  }

  makecontext(&task->context, (void *)start_routine , 1, arg);

  task->id     = task_id_counter++;
  task->status = READY; 

  if (task != &dispatcher_task)
    queue_append((queue_t **) &ready_tasks, (queue_t *) task);

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

  if (current_task == &dispatcher_task) {
    free_task_stack(current_task);
    exit(EXIT_SUCCESS);
  } else {
    current_task->status = FINISHED; 
    task_switch(&dispatcher_task);
  }
}

/* retorno: Identificador numérico (ID) da tarefa corrente, que deverá ser 0 para main, ou um valor positivo para as demais tarefas */
int task_id() {
  #ifdef DEBUG
    fprintf(stdout, "task_id: tarefa atual é a de id %d\n", current_task->id);
  #endif

  return current_task->id;
}
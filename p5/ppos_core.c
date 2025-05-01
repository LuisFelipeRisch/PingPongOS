/*

  NOME: Luis Felipe Risch
  GRR: 20203940

*/

#include "ppos.h"
#include "ppos_data.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <valgrind/valgrind.h>
#include <signal.h>
#include <sys/time.h>

#define STACK_SIZE 64 * 1024

queue_t *ready_tasks;
task_t main_task, dispatcher_task,
    *current_task = &main_task;
int task_id_counter = 0;
struct itimerval tick_clock;
struct sigaction tick_action;

void free_task_stack(task_t *task)
{
  free(task->context.uc_stack.ss_sp);
  VALGRIND_STACK_DEREGISTER(task->vg_id);
}

void tick_handler()
{
  if (!current_task)
    return;
  if (current_task->type == SYSTEM)
    return;

  if (current_task->tick_counter == 0)
    task_yield();
  else
    current_task->tick_counter--;
}

void init_tick_timer()
{
  tick_action.sa_handler = tick_handler;
  sigemptyset(&tick_action.sa_mask);
  tick_action.sa_flags = 0;
  if (sigaction(SIGALRM, &tick_action, 0) < 0)
  {
    perror("init_tick_timer: Error setting tick action: ");
    exit(EXIT_FAILURE);
  }

  tick_clock.it_value.tv_usec = TICK_INTERVAL * 1000;
  tick_clock.it_value.tv_sec = 0;
  tick_clock.it_interval.tv_usec = TICK_INTERVAL * 1000;
  tick_clock.it_interval.tv_sec = 0;

  if (setitimer(ITIMER_REAL, &tick_clock, 0) < 0)
  {
    perror("init_tick_timer: Error setting timer: ");
    exit(EXIT_FAILURE);
  }
}

void task_yield()
{
  current_task->status = READY;
  queue_append((queue_t **)&ready_tasks, (queue_t *)current_task);

  task_switch(&dispatcher_task);
}

task_t *scheduler()
{
  if (!queue_size(ready_tasks))
    return NULL;

  task_t *next_task,
      *cur_task;
  queue_t *cur_queue_node;

  cur_queue_node = ready_tasks;
  next_task = (task_t *)cur_queue_node;

  do
  {
    cur_task = (task_t *)cur_queue_node;

    if (cur_task->dynamic_priority < next_task->dynamic_priority)
      next_task = cur_task;

    cur_queue_node = cur_queue_node->next;

  } while (cur_queue_node != ready_tasks);

  cur_queue_node = ready_tasks;
  do
  {
    cur_task = (task_t *)cur_queue_node;

    if (cur_task != next_task)
      cur_task->dynamic_priority -= AGING;

    cur_queue_node = cur_queue_node->next;

  } while (cur_queue_node != ready_tasks);

  next_task->dynamic_priority = next_task->static_priority;
  queue_remove((queue_t **)&ready_tasks, (queue_t *)next_task);

  return next_task;
}

void dispatcher()
{
  task_t *next_task;

  while ((next_task = scheduler()))
  {
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

void ppos_init()
{
  setvbuf(stdout, 0, _IONBF, 0);
  init_tick_timer();

  main_task.id = task_id_counter++;
  main_task.status = READY;
  queue_append((queue_t **)&ready_tasks, (queue_t *)&main_task);

  task_init(&dispatcher_task, dispatcher, NULL);
  dispatcher_task.type = SYSTEM;

  task_switch(&dispatcher_task);
}

/* retorno: o ID (>0) da nova tarefa ou um valor negativo, se houver erro */
int task_init(task_t *task, void (*start_routine)(void *), void *arg)
{
  char *stack;

  getcontext(&task->context);

  stack = malloc(STACK_SIZE);
  if (stack)
  {
    task->vg_id = VALGRIND_STACK_REGISTER(stack, stack + STACK_SIZE);
    task->context.uc_stack.ss_sp = stack;
    task->context.uc_stack.ss_size = STACK_SIZE;
    task->context.uc_stack.ss_flags = 0;
    task->context.uc_link = 0;
  }
  else
  {
    fprintf(stderr, "task_init: falha durante a criação da pilha!\n");
    return -1;
  }

  makecontext(&task->context, (void *)start_routine, 1, arg);

  task->id = task_id_counter++;
  task->status = READY;
  task->static_priority = DEFAULT_PRIORITY;
  task->dynamic_priority = DEFAULT_PRIORITY;
  task->type = USER;

  if (task != &dispatcher_task)
    queue_append((queue_t **)&ready_tasks, (queue_t *)task);

#ifdef DEBUG
  fprintf(stdout, "task_init: iniciada tarefa %d\n", task->id);
#endif

  return task->id;
}

/* retorno: 1 se pode trocar de task ou 0 se não pode */
int can_switch_to_task(task_t *task)
{
  if (!task)
  {
    fprintf(stderr, "can_switch_to_task: task não pode ser nula!\n");
    return 0;
  }

  if (current_task == task)
  {
    fprintf(stderr, "can_switch_to_task: task não pode ser igual a task atual!\n");
    return 0;
  }

  return 1;
}

/* retorno: valor negativo se houver erro, ou zero */
int task_switch(task_t *task)
{
  if (!can_switch_to_task(task))
    return -1;

  task_t *aux_task;

  task->tick_counter = START_TICK_COUNT;
  aux_task = current_task;
  current_task = task;

#ifdef DEBUG
  fprintf(stdout, "task_switch: trocando contexto %d -> %d\n", aux_task->id, task->id);
#endif

  swapcontext(&aux_task->context, &task->context);

  return 0;
}

void task_exit(int exit_code)
{
#ifdef DEBUG
  fprintf(stdout, "task_exit: tarefa %d sendo encerrada\n", current_task->id);
#endif

  if (current_task == &dispatcher_task)
  {
    free_task_stack(current_task);
    exit(EXIT_SUCCESS);
  }
  else
  {
    current_task->status = FINISHED;
    task_switch(&dispatcher_task);
  }
}

/* retorno: Identificador numérico (ID) da tarefa corrente, que deverá ser 0 para main, ou um valor positivo para as demais tarefas */
int task_id()
{
#ifdef DEBUG
  fprintf(stdout, "task_id: tarefa atual é a de id %d\n", current_task->id);
#endif

  return current_task->id;
}

int priority_between_lower_and_upper_bounds(int prio)
{
  return prio >= PRIORITY_LOWER_BOUND &&
         prio <= PRIORITY_UPPER_BOUND;
}

void task_setprio(task_t *task, int prio)
{
  if (!priority_between_lower_and_upper_bounds(prio))
  {
    fprintf(stderr,
            "task_setprio: prioridade informada (%d) não está dentro dos limites permitidos: %d - %d\n",
            prio,
            PRIORITY_LOWER_BOUND,
            PRIORITY_UPPER_BOUND);

    exit(EXIT_FAILURE);
  }

  task_t *actual_task = task ? task : current_task;

  actual_task->static_priority = prio;
  actual_task->dynamic_priority = prio;

#ifdef DEBUG
  printf("task_setprio: task %d priority set to %d\n",
         actual_task->id,
         prio);
#endif
}

int task_getprio(task_t *task)
{
  task_t *actual_task = task ? task : current_task;
  int static_priority = actual_task->static_priority;

#ifdef DEBUG
  printf("task_getprio: task %d has %d as priority\n",
         actual_task->id,
         static_priority);
#endif

  return static_priority;
}
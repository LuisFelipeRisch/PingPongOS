#include "queue.h"
#include "stdio.h"

// Retorno: 0 se não existe, 1 se existe
int element_exists_in_queue(queue_t *queue, queue_t *element) {
  if (!queue)
    return 0;
  
  unsigned int element_exists;
  queue_t      *current_queue_element; 

  current_queue_element = queue->next;
  element_exists        = current_queue_element == element;

  while (!element_exists && current_queue_element != queue){
    current_queue_element = current_queue_element->next;
    element_exists        = current_queue_element == element;
  }
  
  return element_exists;
}

int queue_size (queue_t *queue){
  if (!queue) return 0;

  unsigned int counter; 
  queue_t      *current_queue_element, *first_queue_element;

  counter               = 1;
  first_queue_element   = queue->next;
  current_queue_element = first_queue_element->next;

  while (current_queue_element != first_queue_element){
    counter++; 
    current_queue_element = current_queue_element->next;
  }

  return counter;
};

void queue_print(char *name, queue_t *queue, void (*print_elem)(void *)) {
  if (!queue) {
    printf("%s: []\n", name);
    return;
  }

  printf("%s: [", name);

  queue_t *current = queue;
  do {
    print_elem(current);
    current = current->next;
    if (current != queue) {
      printf(" ");
    }
  } while (current != queue);

  printf("]\n");
}

int queue_append (queue_t **queue, queue_t *elem){
  if (!queue) {
    fprintf(stderr, "The given queue (queue) is NULL\n");
    return -1;
  }

  if (!elem) {
    fprintf(stderr, "The given element (elem) is NULL\n");
    return -1;
  }

  if (element_exists_in_queue((*queue), elem)) {
    fprintf(stderr, "The given element (elem) already belongs to queue\n");
    return -1;
  }

  if (elem->next || elem->prev) {
    fprintf(stderr, "The given element (elem) already belongs to a another queue!\n");
    return -1;
  }

  if (!(*queue)) {
    elem->next = elem; 
    elem->prev = elem;
    *queue     = elem;
  } else {
    queue_t *last_queue_element = (*queue)->prev;

    elem->next                     = last_queue_element->next;
    elem->prev                     = last_queue_element;
    last_queue_element->next->prev = elem;
    last_queue_element->next       = elem;
  }

  return 0;
}

int queue_remove (queue_t **queue, queue_t *elem){
  if (!queue) {
    fprintf(stderr, "The given queue (queue) is NULL\n");
    return -1;
  }

  if (!(*queue)) {
    fprintf(stderr, "The given queue (queue) is empty!\n");
    return -1;
  }

  if (!elem) {
    fprintf(stderr, "The given element (elem) is NULL\n");
    return -1;
  }

  if (!element_exists_in_queue(*queue, elem)) {
    fprintf(stderr, "The given element (elem) does not belongs to the given queue!\n");
    return -1;
  }

  if (queue_size(*queue) > 1) {
    elem->prev->next = elem->next; 
    elem->next->prev = elem->prev;

    if (*queue == elem)
      *queue = elem->next;
  } else *queue = NULL; 
 
  elem->next       = NULL; 
  elem->prev       = NULL;

  return 0;
}
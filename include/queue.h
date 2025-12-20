#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "point.h"

typedef struct Queue_ {
  unsigned capacity, size;
  unsigned front, rear;
  PointObject **elements;
} Queue;

static inline void queue_init(Queue *const queue, const unsigned capacity) {
  queue->capacity = capacity;
  queue->size = queue->front = queue->rear = 0;
  queue->elements = malloc(sizeof(PointObject *) * capacity);
}

static inline void enqueue(Queue *const queue, PointObject *const element) {
  queue->elements[queue->rear] = element;
  if (++queue->rear == queue->capacity) queue->rear = 0;
  queue->size++;
}

static inline PointObject *dequeue(Queue *const queue) {
  PointObject *const front = queue->elements[queue->front];
  if (++queue->front == queue->capacity) queue->front = 0;
  queue->size--;
  return front;
}

static inline void queue_make_empty(Queue *const queue) {
  queue->front = queue->rear = queue->size = 0;
}

static inline int queue_empty(const Queue *const queue) {
  return queue->size == 0;
}

static inline void queue_resize(Queue *const queue, const unsigned new_cap) {
  if (new_cap <= queue->capacity) return;
  queue->capacity = new_cap + queue->capacity; // * 2
  void *buff = realloc(queue->elements, sizeof(PointObject *) * new_cap);
  if (buff == NULL) return;
  queue->elements = buff;
}

static inline void queue_free(const Queue *const queue) {
  free(queue->elements);
}

#endif //QUEUE_H
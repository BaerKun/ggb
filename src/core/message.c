#include "message.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MESSAGE_CAPACITY 6

typedef struct {
  size_t size, front, rear;
  Message msgs[MESSAGE_CAPACITY];
} MessageQueue;

static MessageQueue queue = {};

void message_push_back(const MessageLevel type, const char *format, ...) {
  va_list args;
  va_start(args, format);

  queue.size++;
  if (queue.rear == MESSAGE_CAPACITY) queue.rear = 0;

  Message *msg = queue.msgs + queue.rear++;
  msg->level = type;
  vsprintf(msg->content, format, args);

  va_end(args);
}

void message_push_front(const MessageLevel type, const char *format, ...) {
  va_list args;
  va_start(args, format);

  queue.size++;
  if (queue.front == 0) queue.front = MESSAGE_CAPACITY - 1;

  Message *msg = queue.msgs + --queue.front;
  msg->level = type;
  vsprintf(msg->content, format, args);

  va_end(args);
}

const Message *message_pop() {
  if (queue.size == 0) return NULL;
  queue.size--;

  if (queue.front == MESSAGE_CAPACITY) queue.front = 0;
  return queue.msgs + queue.front++;
}

void message_make_empty() {
  queue.size = queue.front = queue.rear = 0;
}
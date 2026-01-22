#include "message.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MESSAGE_STACK_CAPACITY 4

typedef struct {
  size_t size, top;
  Message msgs[MESSAGE_STACK_CAPACITY];
} MessageStack;

static MessageStack stack = {};

void message_push(const MessageLevel type, const char *format, ...) {
  va_list args;
  va_start(args, format);

  if (stack.top == MESSAGE_STACK_CAPACITY) stack.top = 0;
  if (stack.size != MESSAGE_STACK_CAPACITY) stack.size++;
  Message *msg = stack.msgs + stack.top++;
  msg->level = type;
  vsprintf(msg->content, format, args);
  va_end(args);
}

const Message *message_pop() {
  if (stack.size == 0) return NULL;
  if (stack.top == 0) stack.top = MESSAGE_STACK_CAPACITY;
  stack.size--;
  return &stack.msgs[--stack.top];
}

void message_make_empty() {
  stack.size = stack.top = 0;
}
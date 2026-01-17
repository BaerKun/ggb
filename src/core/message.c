#include "message.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned size;
  Message msgs[8];
} MessageStack;

static MessageStack stack = {};

void message_push(const MessageType type, const char *format, ...) {
  va_list args;
  va_start(args, format);
  Message *msg = stack.msgs + stack.size++;
  vsprintf(msg->content, format, args);
  va_end(args);
}

const Message *message_pop() {
  if (stack.size == 0) return NULL;
  return &stack.msgs[--stack.size];
}
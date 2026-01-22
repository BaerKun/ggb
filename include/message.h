#ifndef GGB_MESSAGE_H
#define GGB_MESSAGE_H

typedef enum {
  MSG_NULL = 0,
  MSG_INFO,
  MSG_WARN,
  MSG_ERROR,
  MSG_EXIT,
} MessageLevel;

typedef struct {
  MessageLevel level;
  char content[256];
} Message;

#define throw_error(content)                                             \
  return message_push(MSG_ERROR, content), MSG_ERROR
#define throw_error_fmt(format, ...)                                     \
  return message_push(MSG_ERROR, format, __VA_ARGS__), MSG_ERROR
#define push_error(content) message_push(MSG_ERROR, content)
#define push_error_fmt(format, ...) message_push(MSG_ERROR, format, __VA_ARGS__)

#define propagate_error(code) if (code == MSG_ERROR) return MSG_ERROR

void message_push(MessageLevel type, const char *format, ...);
const Message *message_pop();
void message_make_empty();

#endif // GGB_MESSAGE_H

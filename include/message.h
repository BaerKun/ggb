#ifndef GGB_MESSAGE_H
#define GGB_MESSAGE_H

typedef enum {
  MSG_NULL,
  MSG_INFO,
  MSG_WARNING,
  MSG_ERROR,
  UNKNOWN_COMMAND,
  MISS_PARAMETER,
  UNKNOWN_PARAMETER,
  INVALID_PARAMETER,
  NAME_TOO_LONG,
  NAME_EXISTS,
  OBJECT_NOT_EXISTS,
  MSG_EXIT
} MessageType;

typedef struct {
  MessageType type;
  char content[256];
} Message;

#define throw_error(type, content) return message_push(type, content), type
#define throw_error_fmt(type, format, ...)                                  \
  return message_push(type, format, __VA_ARGS__), type

void message_push(MessageType type, const char *format, ...);
const Message *message_pop();

#endif // GGB_MESSAGE_H

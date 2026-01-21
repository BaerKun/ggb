#ifndef GGB_MESSAGE_H
#define GGB_MESSAGE_H

typedef enum {
  MSG_NULL = 0,

  FILE_CANT_OPEN,
  COMMANDLINE_TOO_LONG,
  UNKNOWN_COMMAND,
  MISS_PARAMETER,
  UNKNOWN_PARAMETER,
  INVALID_PARAMETER,
  NAME_TOO_LONG,
  NAME_EXISTS,
  OBJECT_NOT_EXISTS,

  MSG_EXIT,

  MSG_LEVEL_MASK = 1111 << 12,
  MSG_INFO = 1 << 12,
  MSG_WARN = 1 << 13,
  MSG_ERROR = 1 << 14
} MessageType;

typedef struct {
  MessageType type;
  char content[256];
} Message;

#define throw_error(type, content)                                             \
  return message_push(MSG_ERROR | type, content), MSG_ERROR | type
#define throw_error_fmt(type, format, ...)                                     \
  return message_push(MSG_ERROR | type, format, __VA_ARGS__), MSG_ERROR | type

void message_push(MessageType type, const char *format, ...);
const Message *message_pop();
void message_make_empty();

#endif // GGB_MESSAGE_H

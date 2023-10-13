#ifndef __ARGHANDLER_H__
#define __ARGHANDLER_H__

enum Operation {
  OPERATION_CAPTURE,
  OPERATION_REPLAY,
  OPERATION_UNKNOWN,
};

enum Argument_Error {
  AERROR_NONE,
  AERROR_EXCESS_ELEMENTS,
  AERROR_MISSING_ELEMENTS,
  AERROR_INVALID_OPERATION,
  AERROR_IO_FAILURE,
  AERROR_IDK,
  AERROR_ERROR_NUM,
};

extern const char error_messages[AERROR_ERROR_NUM][128];

struct Arguments {
  enum Operation operation;
  const char *replay_infile;

  enum Argument_Error aerror;
};

struct Arguments handle_args(int ac, const char **av);

void help(const char *av0);

#endif /* __ARGHANDLER_H__ */

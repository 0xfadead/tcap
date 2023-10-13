#include "logging.h"
#include <stdarg.h>
#include <stdio.h>

void LOG_ERROR(const char *s, ...) {
  va_list args;

  va_start(args, s);

  fputs("ERROR: ", stderr);
  vfprintf(stderr, s, args);
  fputs("\n", stderr);

  va_end(args);

  return;
}

void puterr(const char *s) {
  fputs(s, stderr);
  fputs("\n", stderr);
  return;
}

void printerr(const char *s, ...) {
  va_list args;

  va_start(args, s);

  vfprintf(stderr, s, args);

  va_end(args);

  return;
}

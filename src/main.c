#include "arghandler.h"
#include "logging.h"
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

void capture_func(void) {
  struct timeval ltime_strc, ctime_strc;

  char line_input_buffer[1024];
  time_t ltime, ctime;

  char char_hashmap[256];

  for (unsigned int i = 0; i < 256; i++) {
    char_hashmap[i] = (char)i;
  }

  char_hashmap['\033'] = '.';

  while (1) {
    gettimeofday(&ltime_strc, NULL);

    if (!fgets(line_input_buffer, 1024, stdin)) {
      return;
    }

    gettimeofday(&ctime_strc, NULL);

    ltime = ltime_strc.tv_sec * 1000000 + ltime_strc.tv_usec;
    ctime = ctime_strc.tv_sec * 1000000 + ctime_strc.tv_usec;
    ctime -= ltime;

    printf("%016lx %s", ctime, line_input_buffer);

    if (line_input_buffer[strlen(line_input_buffer) - 1] != '\n')
      puts("");

    for (unsigned long i = 0; i < 1024; i++) {
      line_input_buffer[i] = char_hashmap[(unsigned char)line_input_buffer[i]];
    }

    fprintf(stderr, "%016lx %s", ctime, line_input_buffer);

    if (line_input_buffer[strlen(line_input_buffer) - 1] != '\n')
      puts("");
  }
}

struct time {
  unsigned long time;
  const char *content;
};

int replay_func(FILE *ofptr) {
  /* Get file size */
  fseek(ofptr, 0, SEEK_END);
  unsigned long fsize = ftell(ofptr);

  fseek(ofptr, 0, SEEK_SET);
  /* ofptr is a global variable */
  fsize -= ftell(ofptr);
  /* ------------- */

  /* Read file content */
  char *fcontent = (char *)malloc(fsize * sizeof(char));
  if (!fcontent) {
    perror("Error: Couldn't allocate memory for input file");

    fclose(ofptr);

    return 1;
  }

  unsigned long read = fread(fcontent, sizeof(char), fsize, ofptr);
  fclose(ofptr);

  if (read < fsize) {
    printerr("Error: Could only read %lu of %lu bytes!\n", read, fsize);
    if (!read) {
      perror("Error: Couldn't read input file");
    }

    free(fcontent);

    return 1;
  }
  /* ----------------- */

  /* Convert newlines to null chars */
  unsigned char char_hashmap[256];

  for (unsigned int i = 0; i < 256; i++)
    char_hashmap[i] = i;

  char_hashmap['\n'] = '\0';

  for (unsigned long i = 0; i < fsize; i++) {
    fcontent[i] = char_hashmap[(unsigned char)fcontent[i]];
  }
  /* ------------------------------ */

  /* Get time offsets */
  unsigned long newline_num = 0;
  struct time *times = (struct time *)malloc(1 * sizeof(struct time));

  char *endptr = NULL;
  unsigned long cchar = 0;
  unsigned long cline = 0;
  while (cchar < fsize) {
    newline_num++;
    times = realloc(times, newline_num * sizeof(struct time));
    if (!times) {
      perror("realloc failed");

      free(times);
      free(fcontent);
    }

    times[cline] =
        (struct time){.time = strtol((char *)fcontent + cchar, &endptr, 16),
                      .content = endptr};

    cline++;

    if (endptr < fcontent + cchar + 16) {
      fprintf(
          stderr,
          "Error: Couldn't convert character %lu in line %lu to a number!\n",
          fcontent + cchar - endptr, cline);

      free(times);
      free(fcontent);
      return 1;
    }
    cchar += strlen(fcontent + cchar) + 1;
  }

  /* ---------------- */

  cchar = 0;

  for (unsigned long i = 0; i < newline_num; i++) {
    usleep(times[i].time);
    printf("%s\n", times[i].content + 1);
  }

  /* Cleanup */
  free(times);
  free(fcontent);
  /* ------- */

  return 0;
}

void LOG_BUG(int ac, char **av, struct Arguments args) {
  LOG_ERROR("Bug detected in function handler_args() or function main()!");
  LOG_ERROR("Please open an issue if you read this!\n");

  LOG_ERROR("Error info: ");
  for (int i = 0; i < ac; i++) {
    LOG_ERROR("\tArgument #%i: `%s`", i, av[i]);
  }
  LOG_ERROR("\tOPERATION: %i", args.operation);
  LOG_ERROR("\tOPERATION: %s", args.replay_infile);

  puterr("");

  LOG_ERROR("Initiating core dump with signal %i...", SIGABRT);
  kill(getpid(), SIGABRT);

  while (1) {
    LOG_ERROR("!! BUG DETECTED !!");
    LOG_ERROR("!! SIGNAL DID NOT KILL PROCESS !!");
    kill(getpid(), SIGSEGV);
  }
}

int main(int ac, char *av[]) {
  struct Arguments args = handle_args(ac, (const char **)av);

  if (args.aerror != AERROR_NONE) {
    return args.aerror;
  }

  if (args.operation == OPERATION_UNKNOWN) {
    LOG_BUG(ac, av, args);
  }

  if (args.operation == OPERATION_CAPTURE) {
    capture_func();
    goto main_return;
  } else {
    if (!args.replay_infile)
      LOG_BUG(ac, av, args);

    FILE *replay_file = fopen(args.replay_infile, "rb");
    if (!replay_file) {
      LOG_ERROR("Could not open file `%s`: %s", args.replay_infile,
                strerror(errno));
      return AERROR_IO_FAILURE;
    }

    return replay_func(replay_file);
  }

main_return:
  return 0;
}

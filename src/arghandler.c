#include "arghandler.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char error_messages[AERROR_ERROR_NUM][128] = {
    /* AERROR_NONE */
    "Success",
    /* AERROR_EXCESS_ELEMENTS */
    "Excess Element(s)",
    /* AERROR_MISSING_ELEMENTS */
    "Missing Element(s)",
    /* AERROR_INVALID_OPERATION */
    "Unknown Operation",
    /* AERROR_IO_FAILURE */
    "[IF YOU SEE THIS, IT IS A BUG]",
    /* AERROR_IDK */
    "Unknown error",
};

void pad_space(unsigned long number) {
  /* Same as number % 2 */
  unsigned long norm_spaces_required = number & 3;
  unsigned long quad_space_num = (number - norm_spaces_required) >> 2;

  while (quad_space_num > 0) {
#ifdef debug
    printerr("####");
#else
    printerr("    ");
#endif
    quad_space_num--;
  }

  while (norm_spaces_required > 0) {
#ifdef debug
    printerr("+");
#else
    printerr(" ");
#endif
    norm_spaces_required--;
  }

  return;
}

void help(const char *av0) {

  printf("Usage for %s: %s [--help] <operation> [<file>]\n", av0, av0);

  puts("\tOperations:");
  puts("\t\tc / capture: Input lines into stdio and print them with a "
       "timestamp to stdout (with all extra characters) and stderr (without "
       "extra characters).");
  puts("\t\tr / replay:  Replay the lines in the specified file.");

  puts("\tExamples:");
  printf("\t\techo \"Hello\" | %s c > test.txt\n", av0);
  printf("\t\t%s r test.txt\n", av0);

  return;
}

struct Arguments handle_args(int ac, const char **av) {
  struct Arguments args = {
      .operation = OPERATION_UNKNOWN,
      .replay_infile = NULL,
      .aerror = AERROR_NONE,
  };

  /* Check for a minimum of arguments */
  if (ac < 2) {
    LOG_ERROR("Too few arguments!");
    printerr("-> %s \n", av[0]);

    /* Print arrow for the error */
    printerr("-> ");
    unsigned long arr_offset = strlen(av[0]) + 1;
    pad_space(arr_offset);
    puterr("^");

    /* Print error message */
    printerr("-> ");
    unsigned long msg_offset =
        strlen(av[0]) - (strlen(error_messages[AERROR_MISSING_ELEMENTS]) >> 1) +
        1;
    pad_space((msg_offset > arr_offset ? 0 : msg_offset));
    puterr(error_messages[AERROR_MISSING_ELEMENTS]);

    args.aerror = AERROR_MISSING_ELEMENTS;
    goto main_return;
  }

  if (av[1][0] != '-')
    goto c_check;

  if (!strcmp(av[1], "-h") || !strcmp(av[1], "--help")) {
    help(av[0]);
    exit(0);
  }

c_check:
  /* speedier check to discard completely unknown operations */
  if (av[1][0] != 'c' && av[1][0] != 'r')
    goto unknown_operation_err;

  /* speedier check to skip the slow strcmp() in the `capture_check` */
  if (av[1][0] == 'r')
    goto replay_check;

  /* Check if operation is capture */
  if (!strcmp(av[1], "c") || !strcmp(av[1], "capture")) {
    if (ac > 2) {
      puterr("Error: Excess Element(s) at end of argument list!");

      /* Fancy shmancy Formatting stuff */
      printerr("-> %s %s ", av[0], av[1]);
      for (int i = 2; i < ac; i++) {
        printerr("%s ", av[i]);
      }
      puterr("");

      unsigned long abs_offset = strlen(av[0]) + 1 + strlen(av[1]);

      printerr("-> ");
      pad_space(abs_offset);

      unsigned long excess_abs_offset = 0;
      /* formatting badness */
      for (int i = 2; i < ac; i++) {
        /* Get offset for the amount of arrows under the argument */
        unsigned long carr_offset = strlen(av[i]);
        excess_abs_offset += carr_offset + 1;

        /* Move to current argument */
        printerr(" ");
        while (carr_offset) {
          printerr("^");
          carr_offset--;
        }
      }
      puterr("");

      printerr("-> ");

      const char errmsg[] = "Excess elements.";
      excess_abs_offset = excess_abs_offset >> 1;
      abs_offset += excess_abs_offset;
      abs_offset -= strlen(errmsg) >> 1;
      pad_space(abs_offset++);
      puterr(errmsg);

      args.aerror = AERROR_EXCESS_ELEMENTS;
      goto main_return;
    }

    args.operation = OPERATION_CAPTURE;
    goto main_return;
  }

replay_check:
  if (!strcmp(av[1], "r") || !strcmp(av[1], "replay")) {
    if (ac < 3) {
      LOG_ERROR("Too few arguments for operation `%s`!", av[1]);
      printerr("-> %s %s \n", av[0], av[1]);

      printerr("-> ");
      unsigned long arr_offset = strlen(av[0]) + 1 + strlen(av[1]) + 1;
      pad_space(arr_offset);
      puterr("^");

      printerr("-> ");
      unsigned long errmsg_offset =
          arr_offset - (strlen(error_messages[AERROR_MISSING_ELEMENTS]) >> 1);
      pad_space((errmsg_offset > arr_offset ? 0 : errmsg_offset));
      puterr(error_messages[AERROR_MISSING_ELEMENTS]);

      args.aerror = AERROR_MISSING_ELEMENTS;
      goto main_return;
    }

    if (ac > 3) {
      LOG_ERROR("Excess Element(s) at end of argument list!");

      printerr("-> %s %s %s ", av[0], av[1], av[2]);
      for (int i = 3; i < ac; i++) {
        printerr("%s ", av[i]);
      }
      puterr("");

      unsigned long abs_offset =
          strlen(av[0]) + 1 + strlen(av[1]) + 1 + strlen(av[2]);

      printerr("-> ");
      pad_space(abs_offset);

      unsigned long excess_abs_offset = 0;
      /* formatting badness */
      for (int i = 3; i < ac; i++) {
        /* Get offset for the amount of arrows under the argument */
        unsigned long carr_offset = strlen(av[i]);
        excess_abs_offset += carr_offset + 1;

        /* Move to current argument */
        printerr(" ");
        while (carr_offset) {
          printerr("^");
          carr_offset--;
        }
      }
      puterr("");

      printerr("-> ");

      excess_abs_offset = excess_abs_offset >> 1;
      abs_offset += excess_abs_offset;
      const unsigned long excess_errmsg_strlen_div_2 =
          strlen(error_messages[AERROR_MISSING_ELEMENTS]) >> 1;
      if (abs_offset > excess_errmsg_strlen_div_2)
        abs_offset -= excess_errmsg_strlen_div_2;
      else
        abs_offset = 0;

      /* Magically the + 1 makes it look better */
      pad_space(abs_offset + 1);
      puterr(error_messages[AERROR_EXCESS_ELEMENTS]);

      args.aerror = AERROR_EXCESS_ELEMENTS;
      goto main_return;
    }

    args.operation = OPERATION_REPLAY;
    args.replay_infile = av[2];
    goto main_return;
  }

unknown_operation_err:
  LOG_ERROR("Unknown operation `%s`!", av[1]);
  printerr("-> %s %s ...\n", av[0], av[1]);

  printerr("-> ");
  unsigned long abs_offset = strlen(av[0]) + 1;
  pad_space(abs_offset);
  unsigned long av1_strlen = strlen(av[1]);
  for (unsigned long i = 0; i < av1_strlen; i++) {
    printerr("^");
  }
  puterr("");

  printerr("-> ");
  abs_offset += av1_strlen >> 1;
  const unsigned long invalid_op_errmsg_strlen_div_2 =
      strlen(error_messages[AERROR_INVALID_OPERATION]) >> 1;
  abs_offset = (abs_offset > invalid_op_errmsg_strlen_div_2
                    ? abs_offset - invalid_op_errmsg_strlen_div_2
                    : 0);
  pad_space(abs_offset);
  puterr(error_messages[AERROR_INVALID_OPERATION]);
  args.aerror = AERROR_INVALID_OPERATION;
  goto main_return;

main_return:
  return args;
}

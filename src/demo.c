// SPDX-License-Identifier: CC0-1.0

#include "fixed_line_endings.h"
#include "wrong_line_endings.h"

#include <stdio.h> // puts, printf
#include <string.h> // strcmp
#include <stdlib.h> // exit
#include <stdbool.h> // bool

char *help =
   "Usage: %s [-h] [-v]\n"
   "\n"
   "Do some stuff!\n"
   "\n"
   "Options:\n"
   " -h, --help        Print this help text and exit.\n"
   " -v, --verbose     Enable verbose mode.\n";

bool verbose = false;

static bool is_arg(const char* arg, const char* short_name, const char* long_name)
      {
      return (strcmp(arg, short_name) == 0) || (strcmp(arg, long_name) == 0);
      }

static void parse_args(const int argc, const char* const argv[])
      {
      for (int i = 1; i < argc; ++i) {
            if (is_arg(argv[i], "-h", "--help")) {
                  printf(help, argv[0]);
                  exit(0);
                  }
            else if (is_arg(argv[i], "-v", "--verbose")) {
                  verbose = true;
                  }
            }
      print_args(argc, argv);
      if (verbose)
            puts("Verbose mode enabled!");
      }

int main(int argc, char* argv[])
      {
      print_hello();
      parse_args(argc, argv);
      print_goodbye();
      return 0;
      }

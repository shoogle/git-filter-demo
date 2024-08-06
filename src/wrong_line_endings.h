/* SPDX-License-Identifier: CC0-1.0 */

/*
 * Oh dear! Somebody accidentally committed this file into the repository with CRLF line endings.
 * We will forevermore see pointless ^M characters when we use `git diff` on this file!
 *
 * This could have been avoided if the repository had a .gitattributes file that specified 'text',
 * 'text=auto', 'eol=lf', or even 'eol=crlf' for this file back when it was originally committed.
 *
 * Since that wasn't the case, you can use this command to stop seeing ^M characters in `git diff`,
 * but note that this makes it harder to spot real problems with line endings:
 *
 *    $ git config core.whitespace cr-at-eol
 *
 * Alternatively, you could try to fix the line endings as discussed in fixed_line_endings.h.
 */

#pragma once

#include <stdio.h> // puts, printf

static void print_args(const int argc, const char* const argv[])
      {
      printf("%s:", argv[0]);
      for (int i = 1; i < argc; ++i)
            printf(" %s", argv[i]);
      puts("");
      }

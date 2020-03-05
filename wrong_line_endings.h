/* SPDX-License-Identifier: CC0-1.0 */

/*
 * Oh dear! Somebody accidentally committed this file into the repository with
 * CRLF line endings and now we can't fix it without ruining `git blame`. We
 * will forevermore see pointless ^M characters when we try to diff this file.
 * This could have been avoided if the repository had a .gitattributes file
 * that specified 'text', 'text=auto', 'eol=lf', or even 'eol=crlf' for this
 * format at the time that this file was originally committed.
 *
 * P.S. You can use this command to stop seeing ^M characters in `git diff`:
 *    $ git config core.whitespace cr-at-eol
 * However, that makes it harder to spot real problems with line endings.
 */
static void print_args(const int argc, const char* const argv[])
      {
      printf("%s:", argv[0]);
      for (int i = 1; i < argc; ++i)
            printf(" %s", argv[i]);
      puts("");
      }

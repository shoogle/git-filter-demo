/* SPDX-License-Identifier: CC0-1.0 */

/*
 * Oh dear! Somebody accidentally committed this file into the repository with CRLF line endings.
 *
 * Fortunately, Git has a feature that enables us to fix this without ruining `git blame`. Better
 * yet, since March 2022, GitHub also supports this feature when viewing the blame online. See
 * https://github.blog/changelog/2022-03-24-ignore-commits-in-the-blame-view-beta/.
 *
 * 1. Ensure this file is correctly tagged with 'text', 'text=auto', 'eol=lf', or even 'eol=crlf'
 *    in .gitattributes, otherwise Git may ignore or overwrite the change in line endings.
 *
 * 2. Change the line endings (e.g. with `dos2unix`) and commit the change. This commit can remove
 *    lines if desired, but it shouldn't add anything new other than the new line endings.
 *
 * 3. Add the commit's SHA to a .git-blame-ignore-revs file at the root of the repository. Commit
 *    this change as well. You can't combine these two commits as that would invalidate the SHA.
 *
 * 4. Protect the SHA. If you rebase the first commit, update the SHA in .git-blame-ignore-revs to
 *    match. If both commits are in the same pull request, it must be merged via "Create a merge
 *    commit" because the other options ("Squash and merge", "Rebase and merge") would invalidate
 *    the SHA. If in doubt, wait for the first commit to be merged to a protected branch ("master",
 *    "main", etc.) before adding its SHA to .git-blame-ignore-revs in a second commit.
 *
 * Once the newline-resolving commit's SHA is mentioned in .git-blame-ignore-revs, GitHub will skip
 * that commit when assigning blame and defer to the previous commit that altered each line.
 */

#pragma once

#include <stdio.h> // puts

static void print_hello()
      {
      puts("Hello, world!");
      }

static void print_goodbye()
      {
      puts("Goodbye, world!");
      }

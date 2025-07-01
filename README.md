# git-filter-demo

Demonstrate use of Git filters to format source files during checkout, staging, and diffing.

## Introduction

The C source files use 6-space banner-style indentation, which was the standard used in
[MuseScore Studio]'s source code prior to version 4. This isn't a popular style, but the point is
to demonstrate how you can use a different coding style on your machine without affecting how the
code looks for other developers.

[MuseScore Studio]: https://github.com/musescore/MuseScore

You don't actually need to compile the C code in order to understand and use this demo.

## Instructions

### Install dependencies

You need these in `PATH`:

-   Git
-   Bash (version 4.2 or higher to enable `shopt -s lastpipe`)
-   Uncrustify (ideally version 0.73, see [instructions][Download Uncrustify]).

[Download Uncrustify]: https://github.com/musescore/MuseScore/wiki/Formatting-the-source-code#download-uncrustify

Uncrustify's output can change between versions, so it's best to use a fixed version.

### Get the demo code

```Bash
git clone https://github.com/shoogle/git-filter-demo.git
cd git-filter-demo
```

Or fork the project on GitHub and clone your fork.

### Define alias

Let's define two [Git Aliases] to list files and their [attributes] as declared in [`.gitattributes`].

[Git Aliases]: https://git-scm.com/book/en/v2/Git-Basics-Git-Aliases
[attributes]: https://git-scm.com/docs/gitattributes
[`.gitattributes`]: .gitattributes

```Bash
git config --global alias.ls-attrs '!f() { git ls-files "$@" | git check-attr --all --stdin ;}; f'
git config --global alias.ls-attr '!f() {
    a="${1%%=*}" p=""
    case "$a" in
        --*) a="${a#--}" v=unspecified;;
        -*) a="${a#-}" v=unset;;
        "$1") v=set;;
        *) v="${1#*=}" p="/ (set|unset|unspecified)$/! ";;
    esac
    shift
    git ls-files "$@" | git check-attr "$a" --stdin | sed -nE "${p}s :\ $a:\ $v$  p"
}; f'
```

<details>
<summary><strong>Usage</strong></summary>

```Bash
# List all attributes for:
git ls-attrs                                    # All files
git ls-attrs [globs...]                         # All matching files

# Example globs:
git ls-attrs '*.h'                              # All C header files
git ls-attrs '*.c' '*.h'                        # All C source and header files
git ls-attrs src/demo.c                         # Just one file

# Note: Special characters in globs must be 'quoted' or \escaped to avoid expansion by the shell.
```

```Bash
# List files for which attribute 'ATTR' is:
git ls-attr ATTR         [paths...] [globs...]   # True (i.e. set but not to a value)
git ls-attr -ATTR        [paths...] [globs...]   # False (i.e. explicitly unset)
git ls-attr --ATTR       [paths...] [globs...]   # Unspecified (i.e. neither set nor unset)
git ls-attr ATTR=PATTERN [paths...] [globs...]   # Set to a matching value

# Pattern can be any POSIX Extended Regular Expression (ERE), such as:
git ls-attr ATTR=bar                             # Exactly 'bar'
git ls-attr ATTR='(bar|baz)'                     # Exactly 'bar' or 'baz'
git ls-attr ATTR='[0-9]+'                        # Just one or more digits
git ls-attr ATTR='.*'                            # Any string value
git ls-attr ATTR='ba.*'                          # Begins 'ba'

# Note: Pattern is always matched against an attribute's entire value. Don't try to include a
# leading ^ (match start-of-line) or trailing $ (match end-of-line) in the pattern. Any special
# characters in pattern must be 'quoted' or \escaped to protect them from the shell.
```

See also:

-   [Git Attributes](https://git-scm.com/docs/gitattributes)
-   [`git check-attr`](https://git-scm.com/docs/git-check-attr)
-   [`git ls-files`](https://git-scm.com/docs/git-ls-files)

</details>

<details>
<summary><strong>Learn more about aliases and how Git runs shell commands</strong></summary>

Git assumes that most aliased commands contain arguments for Git itself. Thus you could make
`git changed` an alias for `git diff --name-only`:

```Bash
git config --global alias.changed 'diff --name-only'

# Usage:
git changed                     # List files with unstaged changes (i.e. changed files)
git changed --cached            # List files with staged changes (i.e. added files)
git changed --cached '*.h'      # List C header files with staged changes
```

However, when an aliased command starts with `!`, Git assumes it's a shell command, and runs it
inside a POSIX shell (`/bin/sh`) along with any arguments given after the alias.

```Bash
git config --global alias.print-args '!printf "%s\n"'

# Usage:
git print-args                  # Print an empty line
git print-args foo              # Print 'foo'
git print-args foo bar          # Print 'foo' and 'bar' on different lines

# Note: If you want to suppress the empty line for zero arguments, set the alias like this instead:
git config --global alias.print-args '![ $# -eq 0 ] || printf "%s\n"'
```

You can use this Bash function to simulate Git's handling of alias commands:

```Bash
function run_as_alias() (
    set -euo pipefail
    local cmd="$1" i=1
    shift
    if [[ "${cmd:0:1}" == '!' ]]; then
        cmd="${cmd:1}"
        dash -c "${cmd} \"\$@\"" "${cmd}" "$@"  # change 'dash' to 'sh' if you don't have dash
    else
        while [[ "${cmd: -${i}:1}" == '\' ]]; do
            ((++i))
        done
        if ((i % 2 == 0)); then
            echo >&2 'fatal: bad alias.run_as_alias string: cmdline ends with \'
            return 128
        fi
        cmd="$(printf '%s' "${cmd}" | xargs printf '%q ')" # split on unquoted spaces
        dash -c "git ${cmd} \"\$@\"" git "$@"
    fi
)

# Usage:
run_as_alias '[!]YOUR_COMMAND [ARGS...]' [args...]

# Examples:
run_as_alias 'diff --name-only'                         # Test a Git command
run_as_alias 'diff --name-only' --cached '*.h'          # Test a Git command with extra arguments
run_as_alias '!echo one' two three                      # Test a shell command with extra arguments
run_as_alias "$(git config alias.print-args)" 'foo bar' # Test an alias you defined earlier
```

Whenever Git runs a command inside a shell, it does so in the standard shell `/bin/sh`, and it sets
the shell variable `$0` to be the command itself (hence `${cmd}` appears twice one one line in the
function above). If extra arguments are passed in after the command, these are set as the shell
parameter variables `$1`, `$2`, `$3`, etc.

On many systems, `/bin/sh` is a symlink to a simple POSIX shell like `dash`, or an outdated version
of `bash`. This means it may not support all the modern "bashisms" (i.e. advanced features) that
you may be accustomed to using in your normal interactive shell.

If you want to use advanced shell features (e.g. arrays), your command needs to specify a more
advanced shell, or load a script that specifies a more advanced shell on the shebang (`!#`) line.

```Bash
run_as_alias '!bash -c "YOUR_ADVANCED_COMMAND"' [args...]
run_as_alias '!bash path/to/script.sh' [args...]
run_as_alias '!path/to/script.sh' [args...] # if script.sh begins `#!/usr/bin/env bash`
```

<details>

### Define smudge and clean filters

A [filter] called `tidy_c` is declared in [`.gitattributes`].

[filter]: https://git-scm.com/docs/gitattributes#_filter

Let's define `smudge` and `clean` commands for this filter. Git will run these commands during
checkout and staging respectively.

```Bash
# Optional: Checkout K&R style (or whatever style you prefer to edit in):
git config filter.tidy_c.smudge "lint/uncrustify/wrapper.sh -l C -c lint/uncrustify/kr.cfg"
git ls-attr | sed -n "s|: filter: tidy_c$||p" | xargs touch             # mark affected files dirty
git ls-attr | sed -n "s|: filter: tidy_c$||p" | xargs git checkout --   # apply smudge

# Required: Check-in MuseScore legacy style:
git config filter.tidy_c.clean "lint/uncrustify/wrapper.sh -l C -c lint/uncrustify/musescore.cfg"
git ls-attr | sed -n "s|: filter: tidy_c$||p" | xargs git add --renormalize --  # apply clean
```

You can define the `smudge` command to be whatever you want, or you can leave it undefined if you
prefer to use the internal style shown in the online preview (i.e. when viewing files on GitHub).

You **must** define the `clean` command _exactly_ as specified above. This ensures the internal
style remains consistent, which keeps code and diffs clean in the online preview.

<details>
<summary><strong>Learn more about the <code>smudge</code> and <code>clean</code> commands</strong></summary>

The command defined for `smudge` or `clean` is run by Git inside a POSIX shell (`/bin/sh`). The
command must read data from `STDIN`, process it somehow, and then write data to `STDOUT`. Git will
not supply any arguments to the command besides those given in the definition.

If the command includes `%f`, Git will replace this with the 'quoted' path to the file currently
being processed. The command could display this path in a status message sent to `STDERR`, or use
it to determine how to process the file, e.g. via:

-   `uncrustify --assume=%f` (see `uncrustify --help`).
-   `prettier --stdin-filepath=%f` (see [docs](https://prettier.io/docs/en/options.html#file-path))
-   Etc.

However, the command **must not** attempt to read from the `%f` file, because this file _may not
exist_, or its contents may differ from `STDIN`. This happens if the filter is processing the
_staged_ version of the file, and the file also has unstaged changes.

You can simulate this behavior as follows:

```Bash
function git_filter_staged() {
    local file="$1" cmd="${2//%f/\'${1//\'/\'\\\'\'}\'}"
    # Change 'dash' to 'sh' if you don't have dash.
    git show ":${file}" | dash -c "${cmd}" "${cmd}"
}

# Test an arbitrary command:
git_filter_staged src/demo.c 'YOUR_COMMAND' # e.g. 'echo >&2 "Processing:" %f; cat' or just 'cat'

# Test the commands you defined earlier:
git_filter_staged src/demo.c "$(git config filter.tidy_c.smudge)"
git_filter_staged src/demo.c "$(git config filter.tidy_c.clean)"
```

As always, when Git runs a command in a shell, it does so in the standard shell (`/bin/sh`), and
sets the shell variable `$0` to be the command itself (hence the repeated `"${cmd}"` in the
function above).

On many systems, `/bin/sh` is a symlink to a simple POSIX shell like `dash`, or an outdated version
of `bash`. This means it may not support all the modern "bashisms" (i.e. advanced features) that
you're used to using in your normal, interactive shell.

If you want to use advanced shell features (e.g. arrays), your command needs to specify a more
advanced shell, or load a script that specifies a more advanced shell on the shebang (`!#`) line.

```Bash
git_filter_staged src/demo.c 'bash -c "YOUR_ADVANCED_COMMAND"'
git_filter_staged src/demo.c 'bash path/to/script.sh'
git_filter_staged src/demo.c 'path/to/script.sh' # if script.sh begins `#!/usr/bin/env bash`
```

See also:

-   [Git Attributes: filter](https://git-scm.com/docs/gitattributes#_filter)
-   [Git Attributes: Keyword expansion](https://git-scm.com/book/en/v2/Customizing-Git-Git-Attributes#_keyword_expansion) (look for `indent`)

</details>

### Edit some files

Make some changes to the `.c` or `.h` source files in the repository and see how your changes are
reported by `git status` and `git diff`.

Try making some whitespace changes (e.g. move curly braces `{}` to a new line), and also try making
some semantic changes (e.g. add another `puts()`, or change some words in a C string).

Notice that `git diff` ignores whitespace changes because they don't survive the filter.

### Define diff filter

If you defined a `smudge` command earlier, you may have noticed that `git diff` displays C code in
the internal style rather than in your checked-out style. To remedy this, [`.gitattributes`] also
declares a diff filter called `tidy_c`.

Let's define the `textconv` command for this filter. Git will run this command when you diff files
with this attribute.

```Bash
# Set the diff filter to match the smudge filter (may not work with all smudge filters):
git config diff.tidy_c.textconv "$(git config filter.tidy_c.smudge) <"

# Alternatively, set the diff filter explicitly:
git config diff.tidy_c.textconv "uncrustify -l C -c lint/uncrustify/kr.cfg -f"
```

Now diffs will use your preferred style. Note that this is purely a visual change. It doesn't
affect what happens with `git add` or `git commit`.

<details>
<summary><b>Learn more about the <code>textconv</code> command</b></summary>

Unlike the `smudge` and `clean` commands, the command defined for `textconv` doesn't receive data
from `STDIN`. Instead, Git provides the path to a single file, which the `textconv` command must
read, process somehow, and then write to `STDOUT`. This path is provided as an extra argument after
all arguments in the command definition, and is also exposed to the command as the shell variable
`$1`. Although the path is 'quoted' to preserve space characters, these quotes are stripped by the
shell so they are not visible to your command.

You can simulate this with:

```Bash
file='src/demo.c'
sh -c 'YOUR_COMMAND_DEFINITION '"'${file}'" '' "${file}" | less
sh -c "$(git config diff.tidy_c.textconv) '${file}'" '' "${file}" | less
```

Try substituting `echo >&2 "Diffing: <$1>"` as `YOUR_COMMAND_DEFINITION` and see what happens!

When you perform a diff (e.g. `git diff src/demo.c`), Git runs the staged and unstaged versions of
the file through your filter and compares them using the ordinary `git diff` algorithm.

```Bash
git show HEAD:src/demo.c >/tmp/staged
diff -u --color=always <(sh -c 'YOUR_COMMAND_DEFINITION /tmp/staged' '' /tmp/staged) <(sh -c 'YOUR_COMMAND_DEFINITION src/demo.c' '' src/demo.c) | less -R
diff -u --color=always <(sh -c "$(git config diff.tidy_c.textconv) /tmp/staged" '' /tmp/staged) <(sh -c "$(git config diff.tidy_c.textconv) src/demo.c" '' src/demo.c) | less -R
```

The `diff` filter is only used for the visual diff. When committing changes, Git calculates deltas
based on the output of the `clean` filter. If no `clean` filter is defined then it uses the actual
file contents.

See also:

-   [Git Attributes: Generating diff text](https://git-scm.com/docs/gitattributes#_generating_diff_text)
-   [Git Attributes: Performing text diffs of binary files](https://git-scm.com/docs/gitattributes#_performing_text_diffs_of_binary_files)
-   [Customizing Git Attributes: Diffing binary files](https://git-scm.com/book/en/v2/Customizing-Git-Git-Attributes#_diffing_binary_files)

</details>

### Optional: Define more filters!

You could define auto-format rules for other types of files, such as Markdown `README.md` files or
build scripts like `CMakeLists.txt`.

Rules declared in `.gitattributes` will affect all developers, whereas rules declared in
`.git/info/attributes` are personal to you.

## Conclusion

My personal view is it's definitely worth defining a `clean` filter for source projects. Doing so
ensures the internal code style remains consistent, which makes for easy code review on GitHub. It
also unlocks the possibility of developers defining `smudge` filters on their local machines,
because you can't `smudge` code unless there's a consistent target to `clean` back to.

I would define `smudge` and `diff` filters for binary files because it's difficult to inspect these
files otherwise.

I would _declare_ `smudge` and `diff` filters for text files, however I personally would not bother
to define commands for them on my local machine. I prefer to work with content directly rather than
with a _representation_ of the content. This means getting used to a different coding style in each
project I contribute to, but at least this way the local files on my machine, as well as the output
of `git diff` and `git show`, always match what you see online in the GitHub preview.

_Peter._

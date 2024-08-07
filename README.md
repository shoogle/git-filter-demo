# git-filter-demo

Demonstrate use of Git filters to auto-format source files during checkout, staging, and diffing.

## Introduction

The C source files use 6-space banner-style indentation, which was the standard used in
[MuseScore Studio]'s source code prior to version 4. This is not a popular style, but the point is
to show how you can change it to something that you are more familiar with without affecting how
the code looks for other developers.

[MuseScore Studio]: https://github.com/musescore/MuseScore

You don't actually need to compile the C code in order to understand and use this demo.

## Instructions

### Install dependencies

You need these in `PATH`:

- Git
- Bash (version 4.2 or higher to enable `shopt -s lastpipe`)
- Uncrustify (ideally version 0.73, see [instructions][Download Uncrustify]).

[Download Uncrustify]: https://github.com/musescore/MuseScore/wiki/Formatting-the-source-code#download-uncrustify

Uncrustify's output can change between versions, so it's usually best to use a fixed version.

### Get the demo code

```Bash
git clone https://github.com/shoogle/git-filter-demo.git
cd git-filter-demo
```

Or fork the project on GitHub and clone your fork.

### Define alias

Let's define a Git alias to list files and their attributes as declared in [`.gitattributes`]:

[`.gitattributes`]: .gitattributes

```Bash
git config --global alias.ls-attr '!f() { git ls-files "$@" | git check-attr --stdin --all ;}; f'

# Usage
git ls-attr                                 # list all attributes for all files
git ls-attr [paths...] [globs...]           # list all attributes for specific files
git ls-attr | sed -n "s|: attr: value$||p"  # list all files that have attr=value
```

### Define smudge and clean filters

One of the attributes declared in [`.gitattributes`] is a filter called `tidy_c` (look for
`filter=tidy_c`). Let's define `smudge` and `clean` commands for this filter. Git will run these
commands during checkout and staging respectively.

```Bash
# Checkout K&R style (optional):
git config filter.tidy_c.smudge "lint/uncrustify/wrapper.sh -l C -c lint/uncrustify/kr.cfg"
git ls-attr | sed -n "s|: filter: tidy_c$||p" | xargs touch             # mark affected files dirty
git ls-attr | sed -n "s|: filter: tidy_c$||p" | xargs git checkout --   # apply smudge

# Check-in MuseScore legacy style (required):
git config filter.tidy_c.clean "lint/uncrustify/wrapper.sh -l C -c lint/uncrustify/musescore.cfg"
git ls-attr | sed -n "s|: filter: tidy_c$||p" | xargs git add --renormalize --  # apply clean
```

You can define the `smudge` command to be whatever you want, or you can leave it undefined if you
prefer to use the internal style as shown in the GitHub preview.

You must **always** define the `clean` command _exactly as specified above_. This ensures the
internal style remains consistent, which keeps code and diffs clean in the online preview.

<details>
<summary><b>Learn more about the <code>smudge</code> and <code>clean</code> commands</b></summary>

The command defined for `smudge` or `clean` will be run by Git inside a shell. It can use shell
features such as pipes, variables, and redirects, providing these are quoted or escaped correctly.
The command must read data from `STDIN`, process it somehow, and then write to `STDOUT`. Git will
not supply any arguments to the command besides those given in the definition.

You can simulate this with:

```Bash
git show HEAD:src/demo.c | sh -c 'YOUR_COMMAND_DEFINITION' | less
git show HEAD:src/demo.c | sh -c "$(git config filter.tidy_c.smudge)" | less
```

If the command definition includes `%f`, Git will replace this with the 'quoted' path to the file
currently being processed. This could be useful to display in error messages, or to customize the
filtering based on file extension (see `--assume` option for Uncrustify). However, the command
**must not** attempt to read from the `%f` file because _it may not exist_ or its contents may
differ from `STDIN`. This happens if the filter is processing the staged version of the file.

```Bash
git show HEAD:src/demo.c | sh -c "$(echo 'YOUR_COMMAND_DEFINITION' | sed "s|%f|'src/demo.c'|g")" | less
git show HEAD:src/demo.c | sh -c "$(git config filter.tidy_c.smudge | sed "s|%f|'src/demo.c'|g")" | less
```

Try substituting `echo >&2 "Cleaning: <%f>"` or `echo >&2 "Smudging: <%f>"` as
`YOUR_COMMAND_DEFINITION` and see what happens!

See also:

- [Git Attributes: filter](https://git-scm.com/docs/gitattributes#_filter)
- [Git Attributes: Keyword expansion](https://git-scm.com/book/en/v2/Customizing-Git-Git-Attributes#_keyword_expansion) (look for `indent`)

</details>

### Edit some files

Make some changes to the `.c` or `.h` source files in the repository and see how your changes are
reported by `git status` and `git diff`.

Try making some whitespace changes (e.g. move `{}` curly braces to a new line), and also try making
some semantic changes (e.g. add another `puts()`, or change some words in a string).

Notice that `git diff` ignores whitespace changes because they don't survive the filter.

### Optional: Define more filters!

You could define auto-format rules for other types of files, such as Markdown `README.md` files or
build scripts like `CMakeLists.txt`.

Rules declared in `.gitattributes` will affect all developers, whereas rules declared in
`.git/info/attributes` are personal to you.

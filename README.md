# git-filter-demo

Demonstrate use of Git filters to auto-format code to your preferred
indentation style during checkout and staging.

The C source files use [MuseScore]'s [coding rules], which specify 6 space
indentation with banner style braces. This is not a popular code style, but
the point is to demonstrate how you can change it to something that you are
more familiar with without affecting other developers.

[MuseScore]: https://musescore.org/
[coding rules]: https://musescore.org/en/handbook/developers-handbook/finding-your-way-around/musescore-coding-rules

## Instructions

### 1. Get the code

```
git clone https://github.com/shoogle/git-filter-demo.git
cd git-filter-demo
```

Or fork the project on GitHub and clone your fork.

### 2. Setup filter to autoformat C code

A filter named `indent_cpp` is declared in `.gitattributes`. You need to
define the commands it should execute during checkout and checkin:

```
# Checkout K&R style:
git config filter.indent_cpp.smudge "lint/uncrustify/wrapper.sh -c lint/uncrustify/kr.cfg"

# Checkin MuseScore style:
git config filter.indent_cpp.clean "lint/uncrustify/wrapper.sh -c lint/uncrustify/musescore.cfg"
```

You could checkout to a different style if you prefer, but you should always
checkin with the MuseScore style to match the existing code.

### 3. Edit some files

Make some changes to one of the `.c` or `.h` files and see how utilities like
`git status` and `git diff` are affected by the filter. You need to install
[uncrustify] for the filter to work.

[uncrustify]: http://uncrustify.sourceforge.net/

### Optional: Define more filters!

If you want other types of file to be autoformatted, such as the CMake build
scripts (`CMakeLists.txt`, `*.cmake`, etc.), then you need to declare more
filters in `.gitattributes` or `.git/info/attributes`. Rules declared in
`.gitattributes` affect all developers, while rules declared in
`.git/info/attributes` are personal to you.

#!/usr/bin/env bash
# SPDX-License-Identifier: CC0-1.0

# Wrapper script for Uncrustify - http://uncrustify.sourceforge.net/
# Fixes a bug where Uncrustify fails to preserve CRLF line endings when the
# newlines config option is set to 'auto' and input is from STDIN.
# See https://github.com/uncrustify/uncrustify/issues/2686.

IFS="" # don't strip leading or trailing spaces when reading lines

read -r line # read first line from STDIN

if [[ "${line: -1}" == $'\r' ]]; then
  # first line ends with CR (assume all lines do)
  newline="\r\n" # CRLF - Dos
else
  newline="\n" # LF - Unix
fi

{
  # send first line to uncrustify
  echo "${line}"

  # read remaining lines from STDIN and send to uncrustify
  while read -r line; do
    echo "${line}"
  done
} |
  uncrustify "$@" |
{
  # read first line of Uncrustify's output
  read -r line

  if [[ "${line: -1}" == $'\r' ]]; then
    # first line ends with CR already (assume all lines do)
    newline="\n" # don't output extra CR characters
  fi

  # fix first line and send to STDOUT
  printf "%s${newline}" "${line}"

  # read and fix remaining lines and send to STDOUT
  while read -r line; do
    printf "%s${newline}" "${line}"
  done
}

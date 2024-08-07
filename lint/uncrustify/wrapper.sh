#!/usr/bin/env bash
# SPDX-License-Identifier: CC0-1.0

# Wrapper script to modify the input and output of Uncrustify - http://uncrustify.sourceforge.net/

# Provides workaround for a bug in Uncrustify versions prior to 0.71 (released May 2020) where CRLF
# line endings were not preserved when reading from STDIN on Windows with newlines=auto in the
# Uncrustify config file. This bug was fixed in https://github.com/uncrustify/uncrustify/pull/2700
# so the workaround is no longer required, but the script is retained to demonstrate how you can
# modify Uncrustify's input and output via pipes (i.e. without creating temporary files).

((${BASH_VERSION%%.*} >= 4)) || { echo >&2 "$0: Error: Please upgrade Bash."; exit 1; }

if ! which uncrustify &>/dev/null; then
    echo >&2 "$0: Error: Please install Uncrustify in PATH. Version 0.73 is recommended."
    exit 1
fi

set -euo pipefail # exit on errors, error on unassigned variables, preserve errors in pipelines
shopt -s lastpipe # preserve variables set in the final section of a pipeline

IFS='' # don't strip leading or trailing spaces when reading lines

read -r input_line # read first line from STDIN, excluding the \n (LF, linefeed) character

if [[ "${input_line:(-1)}" == $'\r' ]]; then
    # first line of input ends with CR (assume all lines do)
    input_cr=$'\r' # CR at EOL (DOS-style CRLF)
else
    input_cr='' # No CR at EOL (Unix-style LF)
fi

output_cr="${input_cr}" # desired

{
    printf '%s\n' "${input_line}" # pipe first line to uncrustify, including possible CR character

    # read remaining lines from STDIN and pipe to uncrustify, including possible CR characters
    while read -r input_line; do
        printf '%s\n' "${input_line}"
    done
} |
    uncrustify "$@" | # call uncrustify with script arguments and connect to input and output pipes
{
    read -r output_line # read first line of Uncrustify's output

    if [[ "${output_line:(-1)}" == $'\r' ]]; then
        # first line of output already ends with CR (assume all lines do)
        output_cr='' # don't output extra CR character (avoid CRCRLF)
    fi

    printf '%s\n' "${output_line}${output_cr}" # send first line to STDOUT, with added CR if needed

    # read remaining lines and send to STDOUT, with added CR characters if needed
    while read -r output_line; do
        printf '%s\n' "${output_line}${output_cr}"
    done
}

# Print success message to console. Do this last to avoid mangling Uncrustify's own messages.
if [[ "${input_cr}" ]]; then
    # Check the value of ${output_cr}. It may have been modified within the pipeline, hence we
    # enabled "shopt -s lastpipe" in order to see the modified value here.
    if [[ "${output_cr}" ]]; then
        echo >&2 "$0: Restored CRLF line endings in Uncrustify output."
    else
        echo >&2 "$0: CRLF line endings were preserved by Uncrustify."
    fi
else
    echo >&2 "$0: Input file did not contain CRLF line endings."
fi

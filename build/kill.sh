#!/bin/sh
set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <application-pid>" >&2
    exit 2
fi
case "$1" in
    ''|*[!0-9]*|0|1|0*) echo "Supply one application PID greater than 1" >&2; exit 2 ;;
esac
command_name=$(ps -p "$1" -o comm=) || exit 1
command_name=${command_name##*/}
case "$command_name" in
    app|app_d) kill -TERM "$1" ;;
    *) echo "PID $1 is not an app or app_d process; refusing to stop it" >&2; exit 1 ;;
esac

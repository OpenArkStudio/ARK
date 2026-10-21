#!/bin/sh
# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
# Request shutdown only after the real sample reaches its post-init callback.
set -eu
pipe=$1
deadline=$2
shift 2
child=
watchdog=
owner=$$

cancel_timer() {
    kill -TERM "$sleeper" 2>/dev/null || :
    wait "$sleeper" 2>/dev/null || :
    exit 0
}

# Defer cancellation until the sleep PID is captured, then always reap it.
timer() {
    trap - EXIT HUP INT ALRM PIPE
    cancelled=0
    trap 'cancelled=1' TERM
    sleep "$1" &
    sleeper=$!
    trap cancel_timer TERM
    if [ "$cancelled" -ne 0 ]; then
        cancel_timer
    fi
    if wait "$sleeper"; then
        kill -"$2" "$3" 2>/dev/null || :
    fi
}

cleanup() {
    result=$?
    trap - EXIT
    trap '' HUP INT TERM ALRM PIPE
    if [ -n "$watchdog" ]; then
        kill -TERM "$watchdog" 2>/dev/null || :
        wait "$watchdog" 2>/dev/null || :
    fi
    if [ -n "$child" ]; then
        kill -TERM "$child" 2>/dev/null || :
        timer 1 KILL "$child" &
        killer=$!
        wait "$child" 2>/dev/null || :
        child=
        kill -TERM "$killer" 2>/dev/null || :
        wait "$killer" 2>/dev/null || :
    fi
    rm -f -- "$pipe"
    exit "$result"
}
trap cleanup EXIT
# Signals during launch must not escape between spawning and capturing a PID.
pending=0
trap 'pending=129' HUP
trap 'pending=130' INT
trap 'pending=143' TERM
trap 'pending=124' ALRM
trap 'exit 141' PIPE

rm -f -- "$pipe"
mkfifo "$pipe"
"$@" >"$pipe" 2>&1 &
child=$!
timer "$deadline" ALRM "$owner" &
watchdog=$!
trap 'exit 129' HUP
trap 'exit 130' INT
trap 'exit 143' TERM
trap 'echo "Application preset helper timed out" >&2; exit 124' ALRM
if [ "$pending" -ne 0 ]; then
    exit "$pending"
fi
while IFS= read -r line; do
    printf '%s\n' "$line"
    case "$line" in
        *sample1_module*PostInit*) kill -TERM "$child" 2>/dev/null || : ;;
    esac
done <"$pipe"
if wait "$child"; then
    result=0
else
    result=$?
fi
child=
exit "$result"

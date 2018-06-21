#!/bin/bash

(cd build && ninja)

if [[ $? -ne 0 ]]; then
    exit $?
fi

tmux setw remain-on-exit on
tmux split-window -v 'cd host && valgrind --leak-check=full --show-leak-kinds=all ../build/server'
tmux split-window -h 'valgrind --leak-check=full --show-leak-kinds=all build/client -f -s mount'
tmux select-pane -t 0

bash

fusermount -u mount
tmux kill-pane -a

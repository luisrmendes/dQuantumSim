#!/bin/bash
if [[ $# -ne 2 ]] ; then
    echo "Usage: run.sh <np> <myqasmFile>"
    exit 1
fi

/usr/bin/mpirun --mca opal_warn_on_missing_libcuda 0 --oversubscribe -np \
    $1 ./dqsim $2

exit 0
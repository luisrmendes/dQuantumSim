#!/bin/bash
if [[ $# -ne 2 ]] ; then
    echo "Usage: run.sh <np> <myqasmFile>"
    exit 1
fi

/usr/bin/mpirun --mca opal_warn_on_missing_libcuda 0 -np \
    $1 ~/work/bolsa-prace/quantum-simulator-sandbox/simulator $2

exit 0
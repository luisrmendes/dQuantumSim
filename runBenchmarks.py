#!/usr/bin/python

import subprocess
import statistics
import sys


def main():

    time_collections = []

    for i in range(20):
        print("Run " + str(i) + " . . .")
        # Send command

        # cmd = "TIMEFORMAT=%R && time sh scripts/run.sh " + sys.argv[1] + " " + sys.argv[2] + " 2>&1 >/dev/null | tail -n 1 || true"
        # cmd = "TIMEFORMAT=%R && (time sh scripts/run.sh " + sys.argv[1] + " " + sys.argv[2] + ";) 2>&1 >/dev/null | tail -n 1 || true"
        cmd = "TIMEFORMAT=%R && (time mpirun -np " + sys.argv[1] + " ./simulator " + sys.argv[2] + ";) 2>&1 >/dev/null | tail -n 1 || true"
        cmdOutput = subprocess.check_output(cmd, shell=True)
        print(cmdOutput)
        str_cmdOutput = cmdOutput.decode('utf-8')
        l = len(str_cmdOutput)

        str_cmdOutput = str_cmdOutput[:l-1]

        time = float(str_cmdOutput)
        time_collections.append(time)

    print("Media: " + str(statistics.mean(time_collections)))
    print("Mediana: " + str(statistics.median(time_collections)))


main()

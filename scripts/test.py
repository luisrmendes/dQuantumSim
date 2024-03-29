#!/usr/bin/python

import sys
import subprocess

W = '\033[0m'   # white (normal)
R = '\033[31m'  # red
G = '\033[32m'  # green
O = '\033[33m'  # orange
B = '\033[34m'  # blue
P = '\033[35m'  # purple

def main():

    if len(sys.argv) != 4:
        print("Usage: python test.py <openqasmFile> <np> <arrayOfExpectedResults>")
        exit(-1)

    myqasmFile = sys.argv[1]
    np = int(sys.argv[2])
    expectedResult = sys.argv[3]

    print(W + "\nTesting " + myqasmFile)

    # parse expected result
    expectedResult = bytes(expectedResult, 'utf-8')
    expectedResult = expectedResult.split()

    for i in range(1, np+1):
        # Send command
        cmd = "mpirun --mca opal_warn_on_missing_libcuda 0 --oversubscribe -np " + str(i+1) + " ./dqsim " + \
            myqasmFile + \
            " | awk '{print $4}' | sed -r 's/[^0-9]*//g' | tr '\n' ' ' "
        cmdOutput = subprocess.check_output(cmd, shell=True)

        # parse results
        results = cmdOutput.split()

        # compare results
        errors = []
        for j in range(len(expectedResult)):
            if results[j] != expectedResult[j]:
                errors.append(j)

        for j in range(len(errors)):
            print(R + "Error in qubit " + str(errors[j]) + " np = " + str(i) + W)

        if len(errors) == 0:
            print(G + "No errors np = " + str(i) + W)

    if len(errors) != 0:
        exit(1)
    else:
        exit(0)

main()

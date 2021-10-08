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
    myqasmFile = sys.argv[1]
    numQubits = int(sys.argv[2])
    expectedResult = sys.argv[3]

    print(W + "\nTesting " + myqasmFile)

    # parse expected result
    expectedResult = bytes(expectedResult, 'utf-8')
    expectedResult = expectedResult.split()

    # # recompile
    # os.system("cd .. && make clean && make")

    if numQubits == 1:
        np = 1
    elif numQubits == 2:
        np = 2
    elif numQubits == 3:
        np = 4
    elif numQubits > 3:
        np = 6
    else:
        print("Invalid numQubits!")
        exit(1)


    for i in range(np):
        # Send command
        cmd = "mpirun --mca opal_warn_on_missing_libcuda 0 -np " + str(i+1) + " ./simulator " + \
            myqasmFile + \
            " | awk '{print $4}' | sed -r 's/[^0-9]*//g' | tr '\n' ' ' "
        cmdOutput = subprocess.check_output(cmd, shell=True)

        # parse results
        results = cmdOutput.split()

        # compare results
        errors = []
        for j in range(numQubits):
            if results[j] != expectedResult[j]:
                errors.append(j)

        for j in range(len(errors)):
            print(R + "Error in qubit " + str(errors[j]) + " np = " + str(i+1) + W)

        if len(errors) == 0:
            print(G + "No errors np = " + str(i+1) + W)


main()

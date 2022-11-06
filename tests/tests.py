
"""
    File: tests.py
    Author: Devin Arena
    Purpose: Test the functionality of the CHTML.
    Since: 2022-11-06
"""

import sys
import os

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_REV = "a"

def printSplash() -> None:
    print("CHTML Test Suite")
    print(f"Version v{VERSION_MAJOR}.{VERSION_MINOR}{VERSION_REV}")
    print("Written by Devin Arena\n")
    print("Options: python [tests.py] [all|compile|run] [case]")


def compileAllTests() -> None:
    pass

def runTest(case) -> None:
    if not os.path.exists(case):
        print(f"Invalid test case {case}")
        exit(1)
    if not os.path.exists(os.path.join("..", "chtml")):
        print("CHTML executable not found")
        exit(1)
    

def runAllTests() -> None:
    pass


def main(args) -> None:
    if len(args) == 1:
        if args[0] == "all":
            runAllTests()
        elif args[0] == "compile":
            compileAllTests()
        else:
            print("Options: python [tests.py] [all|compile|run] [case]")
            exit(1)
    elif len(args) == 2:
        if args[0] == "run":
            runTest(args[1])
        else:
            print("Options: python [tests.py] [all|compile|run] [case]")
            exit(1)
    pass

if __name__ == "__main__":
    if len(sys.argv) == 1:
        printSplash()
        exit(0)

    main(sys.argv[1:])
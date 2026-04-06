#!/bin/bash

clear
cd ..
echo "Compiling output.c..."
gcc -o generated/output full_transpiler_code/output.c
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check your code for errors."
    exit 1
fi

echo "Executing output..."
./generated/output
#!/bin/bash

INPUT_FILE="test_lexer.la"

clear
cd ..

# Ensure the "generated" directory exists
mkdir -p generated

echo "Running Flex..."

flex -o generated/lex.yy.c only_lexer_code/lexer_alone.l

if [ $? -ne 0 ]; then
    echo "Flex encountered an error. Stopping script."
    exit 1
fi

echo "Compiling..."

gcc -o generated/mylexer generated/lex.yy.c -lfl

if [ $? -ne 0 ]; then
    echo "Compilation failed. Stopping script."
    exit 1
fi

echo "Running the program with "$INPUT_FILE"..."

./generated/mylexer < only_lexer_code/"$INPUT_FILE"

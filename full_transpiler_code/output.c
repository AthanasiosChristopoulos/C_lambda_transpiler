#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lambdalib.h"
int fib_results[15];
int isOdd[15];
int isNumberOdd(int n){
    return ((int) n % (int) 2 != 0);
}

int fibonacci(int n){
    if (n <= 1) {
        return n;
    }
    
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void calculateFibonacciSequence(int count){
    for (int i = 0; i < count; i++) {
        fib_results[i] = fibonacci(i);
        isOdd[i] = isNumberOdd(fib_results[i]);
    }
}

void printFibonacciAnalysis(int count){
    writeStr("Fibonacci Sequence Analysis:\n");
    writeStr("Index\tNumber\tIs Odd?\n");
    int sum;
    sum = 0;
    for (int i = 0; i < count; i++) {
        writeInteger(i);
        writeStr("\t");
        writeInteger(fib_results[i]);
        writeStr("\t");
        if (isOdd[i]) {
            writeStr("Yes");
        }
        else {
            writeStr("No");
        }
        writeStr("\n");
        sum = sum + fib_results[i];
    }
    writeStr("\nSum of sequence: ");
    writeInteger(sum);
    writeStr("\n");
}

int main() {
    const int SEQUENCE_LENGTH = 10;
    writeStr("Generating Fibonacci sequence...\n");
    calculateFibonacciSequence(SEQUENCE_LENGTH);
    printFibonacciAnalysis(SEQUENCE_LENGTH);
}



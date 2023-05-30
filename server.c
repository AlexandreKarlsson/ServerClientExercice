#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>
#include "messages.h"
#include "server.h"

void checkValidity(int validity, char sentence[]) {
    if (validity < 0) {
        printf("ERROR: %s \n", sentence);
    }
}

void printNumbers(int numbers[], int length) {
    printf("Output:");
    for (int i = 0; i < length; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}

void bubbleSort(int numbers[], int length) {
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
    printNumbers(numbers, length);
}
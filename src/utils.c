/* utils.c
 * provide string APIs, generic array APIs
 * Huan Chen, 2/21/2017
 */
#include"utils.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>   /* isdigit, isalpha, etc. */
#include<assert.h>
#include<string.h>
#include<stdint.h>  /* uintptr_t */

void my_itoa(int num, char *buffer, int base);

Array* newArray(int itemSize) {
    Array *array = malloc(sizeof(Array));
    array->capacity = MAX_CAPACITY;
    array->items = malloc(itemSize * array->capacity);
    array->size = 0;    // no items currently
    array->itemSize = itemSize;
    return array;
}

void deleteArray(Array *array) {
    assert(array);
    free(array->items);
    free(array);
}

int addItem(Array *array, void *item) {
    if (array->size + 1 > array->capacity) {
        // need to expand the capasity
        int *newItems = realloc(array->items, 2 * array->capacity * array->itemSize);
        if (!newItems) {
            // grow capacity failed
            perror("No space, add items failed.\n");
            return 1;
        }
        // success
        //free(array->items);
        array->items = newItems;
        // update capacity 
        array->capacity *= 2;
    }    
    // copy the item into array
    memcpy((char *) array->items + array->size * array->itemSize, item, array->itemSize);
    array->size++;
    return 0;   // success
}

void printArray(const Array *array, void *printFuncPtr(void *)) {
    assert(array);
    int i;
    for (i = 0; i < array->size; i++) {
        (*printFuncPtr)((char *) array->items + array->itemSize * i);
    }
}

void setMapStreetNames(const char *str, const char *delim, int *names) {
    int i = 0;  // index of names
    int offset = 0;  // current offset
    int nameLen = 0;
    int delimLen = strlen(delim);
    // ignore the trailing '\n' if any
    int strLen = strlen(str);
    if (str[strLen - 1] == '\n') {
        strLen--;
    }
    char *curr = malloc(strLen + 1);
    strncpy(curr, str, strLen);
    curr[strLen] = '\0';
    char *next = strstr(curr, delim); // next name, ", "
    while (next) {
        nameLen = next - (curr + offset);
        char *name = malloc(nameLen * sizeof(char) + 1);
        strncpy(name, curr + offset, nameLen);
        name[nameLen] = '\0';
        names[i] = (int) (uintptr_t) name;  // heap address
        offset += (nameLen + delimLen); // move over the delim
        next = strstr(curr + offset, delim);
        i++;
    } 
    nameLen = strlen(str) - offset;
    char *name = malloc(nameLen * sizeof(char) + 1);
    strncpy(name, curr + offset, nameLen);
    name[nameLen] = '\0';
    names[i] = (int) (uintptr_t) name;
    free(curr);
}

void setDefaultStreetNames(int *names, int size, int base) {
    // row names base: 26, col names base: 10
    int i = 0; 
    for (i = 0; i < size; i++) {
        char buffer[LEN];
        my_itoa(i + 1, buffer, base);
        //printf("%d -> %s\n", i + 1, buffer);
        char *name = malloc(strlen(buffer) + 1);
        strncpy(name, buffer, strlen(buffer));
        name[strlen(buffer)] = '\0';
        names[i] = (int) (uintptr_t) name;
    }
}


void printStrsVerbose(const int *strs, int size) {
    int i = 0;
    printf("%s", (char *) (uintptr_t) strs[i]);
    for (i = 1; i < size; i++) {
        printf(", %s", (char *) (uintptr_t) strs[i]);
    }
    printf("\n");
}

void printStrings(const int *strs, int size) {
    if (size == 0) {
        return;
    }
    int i;
    /* if size > 5, only print the first two and last two elements */
    if (size > 5) {
        i = 0;
        printf("%s", (char *) (uintptr_t) strs[i]);
        for (i = 1; i < size; i++) {
            if (i <= 1 || i >= size - 2) {
                printf(", %s", (char *) (uintptr_t) strs[i]);
            }
            else {
                printf(".");    
            }
        }
        printf("\n");
    }
    else {
        printStrsVerbose(strs, size);
    }
}

/* delete strings */
void deleteStrings(int *strs, int size) {
    if (!strs) return;
    int i;
    for (i = 0; i < size; i++) {
        free((char *) (uintptr_t) strs[i]);
    }
    free(strs);
}

/* utility function used by my_itoa, reverse a string */
void reverse(char buffer[], int length) {
    int left = 0;
    int right = length - 1;
    while (left < right) {
        char tmp = buffer[left];
        buffer[left] = buffer[right];
        buffer[right] = tmp;
        //swap(*(buffer + left), *(buffer + right));
        left++;
        right--;
    }
}

void my_itoa(int num, char *buffer, int base) {
    assert(num >= 0);
    int i = 0;  // index of buffer
    if (base == COL_BASE) {   // col base
        while (num != 0) {
            int digit = num % base;
            buffer[i++] = digit + '0';
            num /= base;
        }
    }
    else if (base == ROW_BASE) {  // 1 -> 'A', 'B', row name base
        // 1 -> A, 2->B, 26->Z
        // 27->AA, 52->AZ
        // 53->BA

        while (num >= 1) {  
            int digit = num % base;
            //buffer[i++] = digit + 'A' - 1;
            if (digit == 0) {
                buffer[i++] = 'Z';
                num = num / base - 1;
            }
            else {
                buffer[i++] = (digit == 0 ? 'Z' : digit + 'A' - 1);
                num /= base;
            }
        }
    }
    else {
        // this case will be ignored
    }
    buffer[i] = '\0';
    reverse(buffer, i);
}


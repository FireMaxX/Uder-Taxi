
#ifndef UTILS_H
#define UTILS_H

#define MAX_CAPACITY 20
#define DEFAULT_ROWS 26
#define DEFAULT_COLS 42
#define ROW_BASE 26 /* row street names's base, used by my_itoa */
#define COL_BASE 10
#define LEN 80

/* generic array */
typedef struct Array_t {
    /*int *items;*/    
    void *items;     
    int itemSize; 
    int size;      
    int capacity;   
} Array;

/* Array API */
Array* newArray(int itemSize);

void deleteArray(Array *array);

int addItem(Array *array, void *item);

/* given array and print function pointer, print its items */
void printArray(const Array *array, void *printFuncPtr(void *));

/* String pointer API */
/* split string str with delim and copy the substring pointer to names */
void setMapStreetNames(const char *str, const char *delim, int *names);

void setDefaultStreetNames(int *names, int size, int base);

/* free space allocated for the string array */
void deleteStrings(int *strs, int size);

void printStrsVerbose(const int *strs, int size);

/* print strings quietly if size is greater than 5 */
void printStrings(const int *strs, int size);
#endif

/* map.c
 * parse the map configuration file
 *
 * Huan Chen, 2/21/2017
 */

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>   /* isdigit, isalpha, etc. */
#include<assert.h>
#include<string.h>
#include<stdint.h>  /* uintptr_t */

#include"utils.h"

#include"map.h"

#define DEFAULT_ROWS 26
#define DEFAULT_COLS 42
#define ROW_BASE 26 /* row street names's base, used by my_itoa */
#define COL_BASE 10


// function implementations
void setPoint(Point *point, int x, int y) {
    point->x = x;
    point->y = y;
}

int cmpPoint(const Point *point1, int x, int y) {
    assert(point1);
    return (point1->x == x && point1->y == y) ? 0 : 1;
}

TaxiStand* newTaxiStand() {
    TaxiStand *taxiStand = malloc(sizeof(TaxiStand));
    return taxiStand;
}

void deleteTaxiStand(TaxiStand *taxiStand) {
    assert(taxiStand);
    free(taxiStand);
}

Landmark* newLandmark() {
    Landmark *landmark = malloc(sizeof(Landmark));
    return landmark;
}
void deleteLandmark(Landmark *landmark) {
    assert(landmark);
    free(landmark);
}



/* utility function: given string, get integer part of it as index of map */
int getIntegerIndex(const char *str, int *pIndex) {
    int len = strlen(str);
    int sum = 0;
    int i = *pIndex;
    while (i < len && !isdigit(str[i])) {
        i++;
    }

    while (i < len && isdigit(str[i])) {
        sum = sum * 10 + str[i] - '0'; 
        i++;
    }
    *pIndex = i;
    return sum - 1;
}

/* utility function: given string, get alphabetic index from given position */
int getAlphabeticIndex(const char *str, int *pIndex) {
    int len = strlen(str);
    int sum = 0;
    int i = *pIndex;
    while (i < len && !isalpha(str[i])) {
        i++;
    }
    while (i < len && isalpha(str[i])) {
        sum = sum * 26 + str[i] - 'A';
        i++;
    }
    *pIndex = i;
    return sum;
}

/* copy the last remaining string to name */
void setObjName(const char *str, int *pIndex, char *name) {
    int len = strlen(str);
    int i = *pIndex;
    while (i < len && !isalnum(str[i])) {
        i++;
    }

    int nameLen = len - i;
    strncpy(name, str + i, nameLen);
    if (name[nameLen - 1] == '\n') {
        name[nameLen - 1] = '\0';
    }
    else { /* add the trailing terminator*/
        name[nameLen] = '\0';
    }
}

/* parse the given string, set landmark value */
void setLandmark(const char *line, Landmark *landmark) {
    // line = "N27(E27,T35) Grand Park"
    // if there is only a mark point in the landmark, then top left and bottom right keep -1 indices
    // rectangular landmark
    int i = 0;
    landmark->mark.x = getAlphabeticIndex(line, &i);  // x index
    landmark->mark.y = getIntegerIndex(line, &i);  // y index of mark

    if (line[i] != '(') {
        setPoint(&(landmark->topLeft), -1, -1);
        setPoint(&(landmark->botRight), -1, -1);
        //landmark->topLeft.x = -1;
        //landmark->topLeft.y = -1;
        //landmark->botRight.x = -1;
        //landmark->botRight.y = -1;
    }
    else {
        landmark->topLeft.x = getAlphabeticIndex(line, &i);
        landmark->topLeft.y = getIntegerIndex(line, &i);// y index of top left

        landmark->botRight.x = getAlphabeticIndex(line, &i);
        landmark->botRight.y = getIntegerIndex(line, &i); // y index of bottom right
    }

    // set name of the landmark
    setObjName(line, &i, landmark->name);
}

/* parse the input string, set taxi stand value */
void setTaxiStand(const char *line, TaxiStand *taxiStand) {
    int i = 0; 
    taxiStand->mark.x = getAlphabeticIndex(line, &i); 
    taxiStand->mark.y = getIntegerIndex(line, &i); 
    taxiStand->numTaxis = getIntegerIndex(line, &i) + 1;   // not zero based, thus increment it
    setObjName(line, &i, taxiStand->name);
}

void printTaxiStand(const TaxiStand *taxiStand) {
    assert(taxiStand);
    printf("TAXI_STAND: %s at [%d][%d], ", taxiStand->name, taxiStand->mark.x, taxiStand->mark.y);
    printf("num of taxis: %d\n", taxiStand->numTaxis);
}

void printLandmark(const Landmark *landmark) {
    assert(landmark);
    printf("LANDMARK: %s at [%d][%d], ", landmark->name, landmark->mark.x, landmark->mark.y);
    if (landmark->topLeft.x != -1) {
        printf("from [%d][%d] to [%d][%d]", landmark->topLeft.x, landmark->topLeft.y, landmark->botRight.x, landmark->botRight.y);
    }
    printf("\n");
}

Map* newMap() {
    Map *map = malloc(sizeof(Map));
    map->rowNames = NULL;
    map->colNames = NULL;
    map->landmarks = newArray(sizeof(Landmark));
    map->stands = newArray(sizeof(TaxiStand));
    return map;
}

void deleteMap(Map *map) {
    assert(map);
    deleteStrings(map->rowNames, map->row);
    deleteStrings(map->colNames, map->col);
    deleteArray(map->landmarks);
    deleteArray(map->stands);
    free(map);
}


/* set up default map */
int loadDefaultMap(Map *map) {
    strcpy(map->title, "Default Map");
    map->row = DEFAULT_ROWS;
    map->col = DEFAULT_COLS;
    map->rowNames = malloc(sizeof(int) * map->row);
    map->colNames = malloc(sizeof(int) * map->col);
    // to set names for rows and columns
    setDefaultStreetNames(map->rowNames, map->row, ROW_BASE);
    setDefaultStreetNames(map->colNames, map->col, COL_BASE);
    return 0;
}

/* load map from the file */
int loadMap(const char *filename, Map *map) {
    FILE *file = NULL;
    file = fopen(filename, "r");
    if (!file) {
        printf("No such file: %s\n", filename);
        return 2;
    }

    // expected headers in map file
    const char *EW_STREETS = "STREET_NAMES_EAST_WEST";
    const char *NS_STREETS = "STREET_NAMES_NORTH_SOUTH";
    const char *LANDMARK = "LANDMARK";
    const char *TAXI_STAND = "TAXI_STAND";
    const char *delim = ", ";

    int hasEW = 0;  // flag: if the file has EW street names
    int hasNS = 0;  // flag: if the file has NS street names 

#ifdef DEBUG
    printf("Loading map configuration file '%s'\n", filename);
#endif
    const int MAX_LINE_LEN = 500;
    char *line = NULL;
    size_t maxLen = MAX_LINE_LEN; // max line length
    int readSize = 0;   // number of chars returned from getline()
    while ((readSize = getline(&line, &maxLen, file)) != -1) {
        // printf("Retrieved line of length: %d\n", readSize);
        //        printf("%s\n", line);
        // parse current line
        if (strncmp(line, "MAP", 3) == 0) { // set map title
            int i = 0;
            setObjName(line, &i, map->title);
#ifdef DEBUG
            printf("Title: %s\n", map->title);
#endif
        }
        // assume line "GRID row col" is the 2nd line
        else if (strncmp(line, "GRID", 4) == 0 && sscanf(line, "%*s %d %d", &(map->row), &(map->col)) == 2) {
#ifdef DEBUG
            printf("Grid size: %d x %d\n", map->row, map->col);
#endif
            map->colNames = malloc(sizeof(int) * map->col);
            map->rowNames = malloc(sizeof(int) * map->row);
        }
        else if (strncmp(line, EW_STREETS, strlen(EW_STREETS)) == 0) {
            hasEW = 1;
            // parse street names separated by ", ", "A", "B", ...
            setMapStreetNames(line + strlen(EW_STREETS) + 1, delim, map->rowNames);
#ifdef DEBUG
            printStrings(map->rowNames, map->row);
            printf("EW_streetNames parsed\n");
#endif
        }
        else if (strncmp(line, NS_STREETS, strlen(NS_STREETS)) == 0) {
            hasNS = 1;
            // parse street names separated by ", "
            //assert(strcmp(str, line + strlen(NS_STREETS) + 1) == 0);
            setMapStreetNames(line + strlen(NS_STREETS) + 1, delim, map->colNames);
            //setMapStreetNames(str, delim, map->colNames);
#ifdef DEBUG
            printStrings(map->colNames, map->col);
            printf("NS_streetNames parsed\n");
#endif
        }
        else if (strncmp(line, LANDMARK, strlen(LANDMARK)) == 0) {
            Landmark *landmark = newLandmark();
            setLandmark(line + strlen(LANDMARK) + 1, landmark);

            int rc = addItem(map->landmarks, landmark);
            assert(rc == 0);

#ifdef DEBUG
            printLandmark(landmark);
            Array *arr = map->landmarks;
            printLandmark((Landmark *) ((char *) arr->items + arr->itemSize * (arr->size - 1)));
#endif
            deleteLandmark(landmark);
            // pointer to be deleted
        }
        else if (strncmp(line, TAXI_STAND, strlen(TAXI_STAND)) == 0) {
            TaxiStand *taxiStand = newTaxiStand();
            setTaxiStand(line + strlen(TAXI_STAND) + 1, taxiStand);
            int rc = addItem(map->stands, taxiStand);
            assert(rc == 0);
#ifdef DEBUG
            printTaxiStand(taxiStand);
#endif
            deleteTaxiStand(taxiStand);
        }
        else {  // unknown lines

        }
        /*
           LANDMARK N27(E27,T35) Grand Park
           LANDMARK X36 New Irvine Train Station
           TAXI_STAND D8(12) Taxi Stand A
           */
    }

    // if map file has no street names, then set the default ones
    if (!hasEW) {
        setDefaultStreetNames(map->rowNames, map->row, ROW_BASE);
    }
    if (!hasNS) {
        setDefaultStreetNames(map->colNames, map->col, COL_BASE);
    }

    if (line) { // the use should free 'line' if not null
        free(line);
    }
#ifdef DEBUG
    printf("Load map finished.\n");
#endif
    fclose(file);
    return 0;
}


void printMap(const Map *map) {
    assert(map);
    printf("--- Map Summary ---\n");
    printf("Grid size: %d x %d\n", map->row, map->col);
    printf("*** Street names (rows) ***\n");
    printStrings(map->rowNames, map->row);
    printf("*** Street names (columns) ***\n");
    printStrings(map->colNames, map->col);

    // print landmarks, taxi stands info
    //int i;
    //for (i = 0; i < map->landmarks->size; i++) {
    //    //printLandmark((Landmark *) map->landmarks->items + i);
    //    Array *arr = map->landmarks;
    //    printLandmark((Landmark *) ((char *) arr->items + arr->itemSize * i));
    //}

    // use function pointer to simplify code, add "void *" to avoid warnings
    printArray(map->landmarks, (void *) printLandmark);
    printArray(map->stands, (void *) printTaxiStand);
    printf("--- End of Map Summary ---\n");
}


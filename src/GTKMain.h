#ifndef GTKMAIN_H
#define GTKMAIN_H

#include "CallB.h"
#include "constants.h"

extern Map* map;

/* Window for Type Location and Destination */
BigP GTK1(int argc, char *argv[]);
/* Window for Confirmation */
int GTK2(int argc, char *argv[], char* info);
/* Window for Notice */
int GTK3(int argc, char *argv[], char* info);
/* Window for Alert */
int GTK4(int argc, char *argv[], char* alert, char* info);

#endif
#ifndef DBOARD_H
#define DBOARD_H
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "ServerManagement.h"
#include <stdint.h>
#include<ctype.h>

/*Define the global valuable*/
/*Make sure make them external after wards*/
int Jsig;
GtkLabel *GTKMoney;
GtkWidget *GTKMap;
extern Map* map;
extern Taxi *taxi[MaxTaxi];
extern int TaxiAmount;
extern char buffer[100];
/*end of defining Global Variable **************************************/



void Stop(GtkWidget *Widget, gpointer Data);/*send the stop signal to GTK loop*/
void UpdateWindow();/*This function is for load GTK and get the information from the server*/
/*------------------------------------------------------------*/


/*Here is the functions for actual drawing map*/
GtkWidget *CreateWindow(int argc, char *argv[]);
/*This function is the main drawing for the map*/
gboolean DrawAreaExposed(GtkWidget *Widget, GdkEventExpose *Event, gpointer Data);
/*Text*/
void GeneratingText(cairo_t *cr, cairo_text_extents_t te, char name[80], int x, int y);
void DSight(cairo_t *cr, Map *map, cairo_text_extents_t te);
void DriveCar(cairo_t *cr, Taxi *ta);
void RowText(cairo_t *cr, cairo_text_extents_t te, Map* map, int n);
void ColText(cairo_t *cr, cairo_text_extents_t te, Map* map, int n);
void TStand(cairo_t *cr, Map *map);
void TLmarks(cairo_t *cr, Landmark* landmark, cairo_text_extents_t te);


#endif

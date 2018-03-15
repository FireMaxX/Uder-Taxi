/* ClientMap.h: header file for ClientMap.c */
/* Author: Yifan Xu */

#ifndef CLIENTMAP_H
#define CLIENTMAP_H

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include <stdint.h>
#include <ctype.h>

/* Defining Global Variable **************************************/
extern int Jsig;
extern GtkWidget *GTKMap;
extern GtkLabel *GTKMessage;
extern Map* map;
extern Point taxi;
extern char GTKbuffer[100];
extern int GTKMaplength;
extern int GTKMapwidth;
/* End of defining Global Variable *******************************/

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
void DriveCar(cairo_t *cr, Point taxi);
void RowText(cairo_t *cr, cairo_text_extents_t te, Map* map, int n);
void ColText(cairo_t *cr, cairo_text_extents_t te, Map* map, int n);
void TStand(cairo_t *cr, Map *map);
void TLmarks(cairo_t *cr, Landmark* landmark, cairo_text_extents_t te);

#endif
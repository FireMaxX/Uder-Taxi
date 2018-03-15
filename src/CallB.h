#ifndef CALLB_H
#define CALLB_H

#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "map.h"
#include <stdint.h>
#include <ctype.h>

/*--------------------------------*/
/*Here comes the GreatPointer*/
typedef struct {
	char name[SLEN];
	char posi_C[SLEN];
	char posi_R[SLEN];
	char desti_C[SLEN];
	char desti_R[SLEN];
	char request[SLEN];
} BigP;

BigP BP;
char Eposi[SLEN];
char Edesti[SLEN];

extern Map* map;

extern char RealRe[SLEN];	/* Global Variable defined in Client.c */
extern int FF;

GtkWidget *window;

void destroy(GtkWidget *widget, gpointer data);/*destory the window*/
void R_callback(GtkWidget *widget);/*print out the information in the request*/
								   /*void N_callback(GtkWidget *widget, GtkWidget *entry); print out the name of the user*/
void P_callback_C(GtkWidget *widget, GtkWidget *entry);/*print out the pick up position of the user request*/
void P_callback_R(GtkWidget *widget, GtkWidget *entry);
void D_callback_C(GtkWidget *widget, GtkWidget *entry);/*print out the destionation of the user request*/
void D_callback_R(GtkWidget *widget, GtkWidget *entry);
void Y_callback(GtkWidget *widget);/*pass back yes to the main function*/
void N_callback(GtkWidget *widget);/*pass back No to the main function*/
void MessageCall(GtkWidget *widget);/*pop up the message to the user*/
void createWindow(GtkWidget *window);


/*void create_space(char *request, char *name, char *posi, char *desti);*/
/*reserve the space for the char*/
/*void delete_space(char *request, char *name, char *posi, char *desti);*/
/*delete the space for the char*/

/*for adding information to pointer*/
void copyString(char *str, char *entry);

void Crequest();/*combine everything to request*/

#endif

#include "CallB.h"
#include <assert.h>

static void on_destroy(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}

void MessageCall(GtkWidget *widget)
{
	printf("Message been read\n");
	gtk_widget_destroy(window);
	gtk_main_quit();
};


void createWindow(GtkWidget *window) {
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_title(GTK_WINDOW(window), "Client");
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 100); // width X hieght
															   // Connect the destroy event of the window
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), NULL);
}

void destroy(GtkWidget *widget,
	gpointer   data)
{
	gtk_main_quit();
};

/*void N_callback(GtkWidget *widget,
	GtkWidget *entry)
{
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	strcpy(Ename, entry_text);
	copyString(BP.name, Ename);
	printf("Entered Name: %s\n", entry_text);
};*/

void P_callback_C(GtkWidget *widget,
	GtkWidget *entry)
{
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	strcpy(Eposi, entry_text);
	copyString(BP.posi_C, Eposi);
	printf("Entered the pick up location Column at: %s\n", entry_text);
};

void P_callback_R(GtkWidget *widget,
	GtkWidget *entry)
{
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	strcpy(Eposi, entry_text);
	copyString(BP.posi_R, Eposi);
	printf("Entered the pick up location Row at: %s\n", entry_text);
};

void D_callback_C(GtkWidget *widget,
	GtkWidget *entry)
{
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	strcpy(Edesti, entry_text);
	copyString(BP.desti_C, Edesti);
	printf("Entered the Destination Column at: %s\n", entry_text);
};

void D_callback_R(GtkWidget *widget,
	GtkWidget *entry)
{
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	strcpy(Edesti, entry_text);
	copyString(BP.desti_R, Edesti);
	printf("Entered the Destination Row at: %s\n", entry_text);
};

void R_callback(GtkWidget *widget)
{
    // gtk_get_text
	printf("Uploading the request...\n");
	Crequest();
	gtk_widget_destroy(window);

}

// copy entry text to string
void copyString(char *str, char *entry) {
    assert(str);
	//printf("what copyed here: %s\n", entry);
    strcpy(str, entry);
	//printf("after copyed here: %s\n", str);
}

void Crequest(void)
{
	/*The request form will be
	  " <Name> From <Position> TO <Destination> "*/

	char C[10], D[20], temp[SLEN];
	char checkR, checkC[3];

	strcpy(C, " TO ");
	strcpy(D, "REQUEST_TAXI ");
	strcpy(temp, BP.posi_R);
	checkR = temp[0];

	memset(BP.posi_R,0,sizeof(BP.posi_R));

	BP.posi_R[0] = checkR;

	strcpy(temp, BP.posi_C);

	memset(BP.posi_C, 0, sizeof(BP.posi_C));
	

	checkR = temp[0];

	checkC[0] = checkR;
	checkC[1] = temp[1];
	checkC[2] = 0;

	if (checkC[1] == 'r' || checkC[1] == 'n'|| checkC[1] == 't'|| checkC[1] == 's')
	{
		BP.posi_C[0] = checkC[0];
		BP.posi_C[1] = 0;
	}
	else
	{
		BP.posi_C[0] = checkC[0];
		BP.posi_C[1] = checkC[1];
		BP.posi_C[2] = 0;
	}



	strcpy(temp, BP.desti_R);
	checkR = temp[0];
	memset(BP.desti_R, 0, sizeof(BP.desti_R));
	BP.desti_R[0] = checkR;


	strcpy(temp, BP.desti_C);
	memset(BP.desti_C, 0, sizeof(BP.desti_C));


	checkR = temp[0];
	checkC[0] = checkR;
	checkC[1] = temp[1];
	checkC[2] = 0;

	if (checkC[1] == 'r' || checkC[1] == 'n' || checkC[1] == 't' || checkC[1] == 's')
	{
		BP.desti_C[0] = checkC[0];
		BP.desti_C[1] = 0;
	}
	else
	{
		BP.desti_C[0] = checkC[0];
		BP.desti_C[1] = checkC[1];
		BP.desti_C[2] = 0;
	}



	strcat(BP.request, D);

	strcat(BP.request, BP.posi_R);
	strcat(BP.request, BP.posi_C);

	strcat(BP.request, C);

	strcat(BP.request, BP.desti_R);
	strcat(BP.request, BP.desti_C);

#ifdef DEBUG
	printf("The request is : %s\n", BP.request);
#endif
	strcpy(RealRe, BP.request);
}

void Y_callback(GtkWidget *widget)
{
#ifdef DEBUG
	printf("The user chooce YES\n");
#endif
	FF = 1;
	gtk_widget_destroy(window);
	gtk_main_quit();
}

void N_callback(GtkWidget *widget)
{
#ifdef DEBUG
	printf("The user chooce NO\n");
#endif
	FF = 0;
	gtk_widget_destroy(window);
	gtk_main_quit();
}


/*BigP *CreateBigPointer()
{

	BigP *Bigpointer = (BigP *)malloc(sizeof(BigP));
	if (Bigpointer == NULL)
	{
		return NULL;
	}

	Bigpointer->Name = (GtkWidget *)malloc(sizeof(GtkWidget));
	Bigpointer->Posi = (GtkWidget *)malloc(sizeof(GtkWidget));
	Bigpointer->Desti = (GtkWidget *)malloc(sizeof(GtkWidget));
	Bigpointer->Request = (GtkWidget *)malloc(sizeof(GtkWidget));

	return Bigpointer;
}

void DeleteBigP(BigP *Pointer)
{
	assert(Pointer);
	free(Pointer->Name);
	free(Pointer->Posi);
	free(Pointer->Desti);
	free(Pointer->Request);

	Pointer->Name = NULL;
	Pointer->Posi = NULL;
	Pointer->Desti = NULL;
	Pointer->Request = NULL;

	free(Pointer);


} */

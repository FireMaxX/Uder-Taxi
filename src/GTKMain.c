#include "GTKMain.h"

static gboolean on_match_select(GtkEntryCompletion *widget, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data) {
	GValue value = { 0, };
	gtk_tree_model_get_value(model, iter, 0, &value);/*The 0 is for the column value/words displayed*/
	fprintf(stdout, "You have selected %s\n", g_value_get_string(&value));
	g_value_unset(&value);
	return FALSE;
}

BigP GTK1(int argc, char *argv[])
{
	GtkWidget *button;
	GSList *group;
	GtkWidget *frame;
	GtkWidget *Pentry_C, *Pentry_R;
	GtkWidget *Dentry_C, *Dentry_R;
	GtkEntryCompletion *completion;
	GtkTreeIter iter;
	GtkWidget *inputBox, *parentHBox, *frameBox;
	GtkWidget *InputBox, *ParentHBox;
	GtkWidget *buttonHbox, *buttonVbox;


	gtk_init(&argc, &argv);

	// create the main, top level, window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	createWindow(window);
	g_signal_connect(window, "destroy",
		G_CALLBACK(destroy), NULL);

	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	//Create boxes to store widgets
	frameBox = gtk_vbox_new(FALSE, 10);
	frame = gtk_frame_new("Enter the pickup location");

	gtk_container_add(GTK_CONTAINER(window), frameBox);
	gtk_box_pack_start(GTK_BOX(frameBox), frame, TRUE, TRUE, 0);

	parentHBox = gtk_hbox_new(FALSE, 0);
	inputBox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), parentHBox);
	gtk_box_pack_start(GTK_BOX(parentHBox), inputBox, TRUE, TRUE, 0);



	/*rows */
	Pentry_R = gtk_entry_new();

	// set up completion
	completion = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(completion, 0);
	gtk_entry_set_text(GTK_ENTRY(Pentry_R), "AntBeater Road (e.g.)");
	gtk_entry_set_completion(GTK_ENTRY(Pentry_R), completion);
	g_signal_connect(G_OBJECT(completion), "match-selected", G_CALLBACK(on_match_select), NULL);

	GtkListStore *RlistStore = gtk_list_store_new(1, G_TYPE_STRING);

	// set up the listStore to store the items available for auto-completion

	int size = map->row;
	int i;
	char* string;
	int name;
	const char *Rnames[size];
	for (i = 0; i < size; i++) {
		name = map->rowNames[i];
		string = (char *)(uintptr_t)name;
		Rnames[i] = string;
	};

	for (i = 0; i < size; i++) {
		gtk_list_store_append(RlistStore, &iter);
		gtk_list_store_set(RlistStore, &iter, 0, Rnames[i], -1);
	}

	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(RlistStore));


	Pentry_C = gtk_entry_new();

	// set up completion
	completion = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(completion, 0);
	gtk_entry_set_text(GTK_ENTRY(Pentry_C), "31st Street (e.g.)");
	gtk_entry_set_completion(GTK_ENTRY(Pentry_C), completion);
	g_signal_connect(G_OBJECT(completion), "match-selected", G_CALLBACK(on_match_select), NULL);

	GtkListStore *ClistStore = gtk_list_store_new(1, G_TYPE_STRING);

	// set up the listStore to store the items available for auto-completion
	size = map->col;
	const char *Cnames[size];


	for (i = 0; i < size; i++) {
		name = map->colNames[i];
		string = (char *)(uintptr_t)name;

		Cnames[i] = string;
	};

	for (i = 0; i < size; i++) {
		gtk_list_store_append(ClistStore, &iter);
		gtk_list_store_set(ClistStore, &iter, 0, Cnames[i], -1);
	}

	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(ClistStore));
	// and insert our entry into the main window
	gtk_box_pack_start(GTK_BOX(inputBox), Pentry_R, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(inputBox), Pentry_C, TRUE, TRUE, 0);



	/*Destination */
	frame = gtk_frame_new("Enter the Destination");
	ParentHBox = gtk_hbox_new(FALSE, 0);
	InputBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(frameBox), frame, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), ParentHBox);
	gtk_box_pack_start(GTK_BOX(ParentHBox), InputBox, TRUE, TRUE, 0);

	
	/*Row-------------------------------------------------------------*/

	Dentry_R = gtk_entry_new();

	// set up completion
	completion = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(completion, 0);
	gtk_entry_set_text(GTK_ENTRY(Dentry_R), "Z End (e.g.)");
	gtk_entry_set_completion(GTK_ENTRY(Dentry_R), completion);
	g_signal_connect(G_OBJECT(completion), "match-selected", G_CALLBACK(on_match_select), NULL);


	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(RlistStore));
	// set up the listStore to store the items available for auto-completion
	Dentry_C = gtk_entry_new();

	// set up completion
	completion = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(completion, 0);
	gtk_entry_set_text(GTK_ENTRY(Dentry_C), "21st Street (e.g.)");
	gtk_entry_set_completion(GTK_ENTRY(Dentry_C), completion);
	g_signal_connect(G_OBJECT(completion), "match-selected", G_CALLBACK(on_match_select), NULL);


	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(ClistStore));

	// and insert our entry into the main window
	gtk_box_pack_start(GTK_BOX(InputBox), Dentry_R, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(InputBox), Dentry_C, TRUE, TRUE, 0);


	/*Here is for the radio buttons*/
	/*For now, There are useless*/
	buttonHbox = gtk_hbox_new(FALSE, 0);
	buttonVbox = gtk_vbox_new(FALSE, 0);

	button = gtk_radio_button_new_with_label(NULL, "Taxi");
	gtk_container_add(GTK_CONTAINER(buttonHbox), button);
	gtk_box_pack_start(GTK_BOX(frameBox), buttonHbox, TRUE, TRUE, 0);

	gtk_widget_show(button);
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));

	button = gtk_radio_button_new_with_label(group, "Bus");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	gtk_box_pack_start(GTK_BOX(buttonHbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);

	button = gtk_button_new_with_label("Send the Request");
	g_signal_connect(button, "clicked", G_CALLBACK(P_callback_C), Pentry_C);
	g_signal_connect(button, "clicked", G_CALLBACK(P_callback_R), Pentry_R);
	g_signal_connect(button, "clicked", G_CALLBACK(D_callback_C), Dentry_C);
	g_signal_connect(button, "clicked", G_CALLBACK(D_callback_R), Dentry_R);
	g_signal_connect(button, "clicked", G_CALLBACK(R_callback), NULL);
	/*record data*/


	gtk_box_pack_start(GTK_BOX(frameBox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);

	gtk_widget_show_all(window);

	/* start the main loop */
	gtk_main();

	return BP;
}

int GTK2(int   argc,
	char *argv[], char* info)
{
	GtkWidget *button;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *label;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/*set the window size*/

	gtk_window_set_title(GTK_WINDOW(window), "Confirmation");
	g_signal_connect(window, "destroy",
		G_CALLBACK(destroy), NULL);

	vbox = gtk_vbox_new(FALSE, 5);
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(window), hbox);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);


	frame = gtk_frame_new("Find a Taxi for you:");
	label = gtk_label_new(info);
	gtk_container_add(GTK_CONTAINER(frame), label);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("Confirm");
	g_signal_connect(button, "clicked", G_CALLBACK(Y_callback), NULL);
	gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);	
	
	button = gtk_button_new_with_label("Refuse");
	g_signal_connect(button, "clicked", G_CALLBACK(N_callback), NULL);
	gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);


	gtk_widget_show_all(window);

	gtk_main();

	return 0;
};

int GTK3(int   argc,
	char *argv[], char* info)
{
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *label;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window), "Notice");
	g_signal_connect(window, "destroy",
		G_CALLBACK(destroy), NULL);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);


	frame = gtk_frame_new("Here is the latest Notice:");
	label = gtk_label_new(info);
	gtk_container_add(GTK_CONTAINER(frame), label);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("Okay");
	g_signal_connect(button, "clicked", G_CALLBACK(MessageCall), NULL);
	gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}

int GTK4(int   argc,
	char *argv[], char* alert, char* info)
{
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *label;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window), "Alert!");
	g_signal_connect(window, "destroy",
		G_CALLBACK(destroy), NULL);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);


	frame = gtk_frame_new(alert);
	label = gtk_label_new(info);
	gtk_container_add(GTK_CONTAINER(frame), label);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("Okay");
	g_signal_connect(button, "clicked", G_CALLBACK(MessageCall), NULL);
	gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}

/*
int main(int argc,char *argv[])
{
	GTK1(0, 0);
	GTK2(0, 0, "233");
	printf("What is here ? %s\n", RealRe);
	printf("1 means accept and 0 means refuse, The value of FF is %d\n", FF);
	return 0;
}*/

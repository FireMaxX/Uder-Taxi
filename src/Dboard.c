#include "Dboard.h"
#include <math.h>

void UpdateWindow()		/* render the window on screen */
{
	/* this server has it's own main loop for handling client connections;
	* as such, it can't have the usual GUI main loop (gtk_main);
	* instead, we call this UpdateWindow function on regular basis
	*/
	while (gtk_events_pending())
	{
		gtk_main_iteration();
	}
} /* end of UpdateWindow */

void Stop(GtkWidget *Widget, gpointer Data)
{
	GTKMap = NULL;
	GTKMoney = NULL;
	Jsig = 1;
}

/*Function call for drawing the map*/
gboolean DrawAreaExposed(	/* drawing area is displayed, */
	GtkWidget *Widget,	/* render an analog clock     */
	GdkEventExpose *Event,
	gpointer Data)
{
	int x, y;
	int Mrow, Mcol;
	GtkWidget *DrawArea;
//	cairo_surface_t *surface;
	cairo_t *cr;
	cairo_text_extents_t te;

	Mrow = map->row + 1;
	Mcol = map->col + 1;
//	printf("the value of col %d\n", map->col);

	DrawArea = Widget;
	cr = gdk_cairo_create(DrawArea->window);
	/* Examples are in 1.0 x 1.0 coordinate space */
	cairo_scale(cr, 20, 20);

	/* Drawing code goes here */
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, 1000, 800);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	for (x = 1; x < Mcol; x++) {
		for (y = 1; y < Mrow; y++) {

			cairo_move_to(cr, x, 1);
			cairo_line_to(cr, x, Mrow -1);
			cairo_move_to(cr, 1, y);
			cairo_line_to(cr, Mcol -1, y);
			cairo_set_line_width(cr, 0.02);
			cairo_stroke(cr);
		}
	}

	for (x = 0; x < Mcol-1; x++) {
		ColText(cr, te, map, x);
	}
	for (y = 0; y < Mrow-1; y++)
	{
		RowText(cr, te, map, y);
	}
	/*Drawing the sights*/
	DSight(cr, map, te);
	TStand(cr, map);
	/*generating text*/

	/*move cars*/
	for (x = 0; x < TaxiAmount; x++)
	{
		DriveCar(cr, taxi[x]);
	}
	/* Write output and clean up */

	cairo_destroy(cr);


	return TRUE;
}

GtkWidget *CreateWindow( int argc, char *argv[])
{
	GtkWidget *Window;
	GtkWidget *HBox, *DrawArea, *frame, *label;
	//GtkWidget *ShutdownButton;
	GtkWidget *table;

	/* initialize the GTK libraries */
	gtk_init(&argc, &argv);
	Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(Window), "Uder Console V1.0");
	gtk_window_set_default_size(GTK_WINDOW(Window), 1000, 800);
	gtk_container_set_border_width(GTK_CONTAINER(Window), 10);

	Jsig = 0;
	HBox = gtk_hbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(Window), HBox);

	/* on the top, put a drawing area */
	DrawArea = gtk_drawing_area_new();
	gtk_widget_set_size_request(DrawArea, 1000, 800);/*FIXme we need to use globe here*/
	gtk_container_add(GTK_CONTAINER(HBox), DrawArea);



	g_signal_connect(Window, "destroy",
		G_CALLBACK(Stop), NULL);

	g_signal_connect(DrawArea, "expose_event",
		G_CALLBACK(DrawAreaExposed), NULL);

	/*Here is the table chart for server*/
	table = gtk_table_new(12, 8, TRUE);

	gtk_container_add(GTK_CONTAINER(HBox), table);
	frame = gtk_frame_new("Money earned");
	label = gtk_label_new(buffer);
	gtk_container_add(GTK_CONTAINER(frame), label);
	gtk_table_attach_defaults(GTK_TABLE(table), frame, 0, 10, 0, 1);

	GTKMoney= (GtkLabel*)label;
	GTKMap = DrawArea;
	/* make clock widgets public */

	gtk_widget_show_all(Window);

	return(Window);
} /* end of CreateWindow */


void GeneratingText(cairo_t *cr, cairo_text_extents_t te, char name[80], int x, int y)
{
	char FULL[100];
	int length;
	length = strlen(name);
	strcpy(FULL, name);
//	printf("The string is %s \n", name);
//	printf("FULL is %s \n", FULL);

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_select_font_face(cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.8);
	cairo_text_extents(cr, FULL, &te);
	cairo_move_to(cr, (x),(y));
	cairo_show_text(cr, FULL);

}

void DSight(cairo_t *cr, Map *map, cairo_text_extents_t te)
{
	int tx, ty, bx, by, n, m, x, y, width, height;
	double offset;
	char name[80];
	Array *arr = map->landmarks;
	Landmark* landmark;
	void* item;
	item = arr->items;

	offset = 1-(sqrt(2)/2);

	m = map->landmarks->size;
//	printf("the value of m is %d\n",m);
	for (n = 0; n < m; n++) 
	{
		landmark = (Landmark *)((char *)arr->items + arr->itemSize * n);
		tx = landmark->topLeft.x;
//		printf("mark tx is here %d\n", tx);
		ty = landmark->topLeft.y;
//		printf("mark ty is here %d\n", ty);
		bx = landmark->botRight.x;
		by = landmark->botRight.y;

		width = by - ty;
//		printf("mark width is here %d\n", width);
		height = bx - tx;
//		printf("mark height is here %d\n", height);
		cairo_set_source_rgb(cr, 1, 1, 0);
		cairo_rectangle(cr, ty+1, tx+1, width, height);
		cairo_fill(cr);

	}
	for (n = 0; n < m; n++)
	{
		landmark = (Landmark *)((char *)arr->items + arr->itemSize * n);
		strcpy(name, landmark->name);
//		printf("structure is here %s\n", name);
		tx = landmark->topLeft.x;
		ty = landmark->topLeft.y;
		if (tx == -1 && ty == -1)
		{
			tx = landmark->mark.x;
			ty = landmark->mark.y;
			GeneratingText(cr, te, name, ty-4, tx);
			break;
		}
		GeneratingText(cr, te, name, ty+1, tx+2);
	}
	for (n = 0; n < m; n++)
	{
		landmark = (Landmark *)((char *)arr->items + arr->itemSize * n);

		x = landmark->mark.x;
//		printf("mark x is here %d\n", x);
		y = landmark->mark.y;
//		printf("mark y is here %d\n", y);
		cairo_set_source_rgb(cr, 1, 0, 0);
//		cairo_rectangle(cr, y - 0.5, x - 0.5, 1, 1);
//		cairo_fill(cr);

		/*upper triangle*/
		cairo_move_to(cr, y+1, x+1-1 + offset);
		cairo_line_to(cr, y+1 - 1 + offset, x+1);
		cairo_line_to(cr, y+1 + 1 - offset, x+1);
		cairo_close_path(cr);

		/*lower triangle*/
		cairo_move_to(cr, y+1, x+1+1 - offset);
		cairo_line_to(cr, y+1-1 + offset, x+1);
		cairo_line_to(cr, y+1+1 - offset, x+1);
		cairo_close_path(cr);

		cairo_stroke_preserve(cr);
		cairo_fill(cr);

	}

}

void TStand(cairo_t *cr, Map *map)
{
	int x, y, n, m;
	Array *arr = map->stands;
	TaxiStand* TStand;

	m = map->stands->size;
	for (n = 0; n < m; n++)
	{
		TStand = (TaxiStand *)(arr->items + arr->itemSize * n);
		x = TStand->mark.x;
		y = TStand->mark.y;
		cairo_set_source_rgb(cr, 0, 1, 0);
		cairo_rectangle(cr, y+1-0.5, x+1-0.5, 1, 1);
		cairo_fill(cr);
	}
}

void DriveCar(cairo_t *cr, Taxi* ta)
{
	cairo_set_source_rgb(cr, 0, 0, 0.5);
//	cairo_rectangle(cr, ta->position->x - 0.5, ta->position->y - 0.5, 1, 1);
	cairo_arc(cr, ta->position->y+1, ta->position->x+1, 0.5, 0, 2 * M_PI);
//	printf("The name is %d, with [%d][%d]\n", ta->num, ta->position->y, ta->position->x);
	cairo_fill(cr);

}

void RowText(cairo_t *cr, cairo_text_extents_t te, Map* map, int n)
{
	char LAST[100];
	double x, y;
	char* string;
	int name;
	x = map->col;
	y = n+0.5;
	name = map->rowNames[n];

	string = (char *)(uintptr_t)name;
	if (!string) {
		name=0;
	}
	else
	{
		strcpy(LAST, string);

		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
		cairo_select_font_face(cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr, 0.5);
		cairo_text_extents(cr, LAST, &te);
		cairo_move_to(cr, (x + 0.5),(y +0.7));
		cairo_show_text(cr, LAST);
	}


}

void ColText(cairo_t *cr, cairo_text_extents_t te, Map* map, int n)
{
	char FULL[100];
	int x, y;
	char* string;
	int name;

	x = n + 2;
	y = map->row;

	name = map->colNames[n];

	string = (char *)(uintptr_t)name;
	if (!string) {
		name=0;
	}
	else {
		strcpy(FULL, string);

		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
		cairo_select_font_face(cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr, 0.5);
		cairo_save(cr);
		cairo_text_extents(cr, FULL, &te);
		cairo_move_to(cr, (x +0.5) - te.width / 2 - te.x_bearing,
			(y + 0.5) - te.height / 2 - te.y_bearing);
		cairo_rotate(cr, 1.5708);
		cairo_show_text(cr, FULL);
		cairo_restore(cr);
	}

}

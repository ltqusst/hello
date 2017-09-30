
#include <gtk/gtk.h>


void set_window_defaults(GtkWindow * wnd, const gchar * title,
		gint w, gint h, GtkWindowPosition pos)
{
	gtk_window_set_title(wnd, title);
	gtk_window_set_default_size(wnd, w, h);
	gtk_window_set_position(wnd, pos);

	g_signal_connect_swapped(G_OBJECT(wnd),"destroy",
							 G_CALLBACK(gtk_main_quit), NULL);
}


GtkBox * add_vbox(GtkWindow *wnd)
{
	GtkWidget *vbox;
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(wnd), vbox);
	return GTK_BOX(vbox);
}
GtkWidget * add_menu_bar(GtkBox *box)
{
	GtkWidget *menubar;
	menubar = gtk_menu_bar_new();

	gtk_box_pack_start(box, menubar, FALSE, FALSE, 3);
	return menubar;
}

int main(int argc, char * argv[])
{
	GtkWindow *window;
	GtkBox * vbox;
	GtkWidget * menubar;

	gtk_init(&argc, &argv);

	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

	set_window_defaults(window,
			"Window Title is HelloWorld",
			500,400,GTK_WIN_POS_CENTER);

	vbox = add_vbox(window);
	menubar = add_menu_bar(vbox);



	gtk_widget_show(GTK_WIDGET(window));
	gtk_main();
	return 0;
}

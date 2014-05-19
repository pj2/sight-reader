/* events.h
Author: Joshua Prendergast */

#ifndef EVENTS_H
#define EVENTS_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

gboolean key_press_event_callback(GtkWidget *widget, GdkEventKey *event, gpointer data);

#endif // EVENTS_H

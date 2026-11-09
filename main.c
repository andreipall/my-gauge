#include <gtk/gtk.h>
#include "my-gauge.h"

static void
on_slider_changed(GtkRange *range, gpointer user_data)
{
    MyGauge *gauge = MY_GAUGE(user_data);

    double value = gtk_range_get_value(range);
    my_gauge_set_value(gauge, value);
}


static void
on_gauge_changed(MyGauge *gauge,
                 double value,
                 gpointer user_data)
{
    g_print("Gauge value: %.1f\n", value);
}

static void
activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "Custom GTK Widget");
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 350);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    GtkWidget *gauge = my_gauge_new();
		g_signal_connect(
		gauge,
		"value-changed",
		G_CALLBACK(on_gauge_changed),
		NULL
	);
    my_gauge_set_value(MY_GAUGE(gauge), 21.0);

    gtk_widget_set_hexpand(gauge, TRUE);
    gtk_widget_set_vexpand(gauge, TRUE);

    GtkWidget *slider = gtk_scale_new_with_range(
        GTK_ORIENTATION_HORIZONTAL,
        0.0, 100.0, 1.0
    );

    gtk_range_set_value(GTK_RANGE(slider), 21.0);

    g_signal_connect(
        slider,
        "value-changed",
        G_CALLBACK(on_slider_changed),
        gauge
    );

    gtk_box_append(GTK_BOX(box), gauge);
    gtk_box_append(GTK_BOX(box), slider);

    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new(
        "com.example.customgauge",
        G_APPLICATION_DEFAULT_FLAGS
    );

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(
        G_APPLICATION(app), argc, argv
    );

    g_object_unref(app);

    return status;
}

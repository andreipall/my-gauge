#ifndef MY_GAUGE_H
#define MY_GAUGE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MY_TYPE_GAUGE (my_gauge_get_type())

G_DECLARE_FINAL_TYPE(
    MyGauge,
    my_gauge,
    MY,
    GAUGE,
    GtkWidget
)

GtkWidget *my_gauge_new(void);

void my_gauge_set_value(MyGauge *self, double value);
double my_gauge_get_value(MyGauge *self);

G_END_DECLS

#endif

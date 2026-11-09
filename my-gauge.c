#include "my-gauge.h"
#include <math.h>

struct _MyGauge {
    GtkWidget parent_instance;

    double value;
};

G_DEFINE_TYPE(MyGauge, my_gauge, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_VALUE,
    N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

enum {
    SIGNAL_VALUE_CHANGED,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
my_gauge_clicked(
    GtkGestureClick *gesture,
    int n_press,
    double x,
    double y,
    gpointer user_data
)
{
    MyGauge *self = MY_GAUGE(user_data);

    double next = self->value + 10.0;

    if (next > 100.0)
        next = 0.0;

    my_gauge_set_value(self, next);
}

/* Render the widget */
static void
my_gauge_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    MyGauge *self = MY_GAUGE(widget);

    int width = gtk_widget_get_width(widget);
    int height = gtk_widget_get_height(widget);

    double cx = width / 2.0;
    double cy = height / 2.0;
    double radius = MIN(width, height) * 0.35;

    graphene_rect_t bounds = GRAPHENE_RECT_INIT(
        0, 0, width, height
    );

    cairo_t *cr = gtk_snapshot_append_cairo(
        snapshot, &bounds
    );

    /* Background circle */
    cairo_set_source_rgb(cr, 0.25, 0.28, 0.32);
    cairo_set_line_width(cr, 12);

    cairo_arc(cr, cx, cy, radius, 0, 2 * G_PI);
    cairo_stroke(cr);

    /* Progress arc */
    cairo_set_source_rgb(cr, 0.25, 0.75, 0.55);
    cairo_set_line_width(cr, 12);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    double start = -G_PI / 2;
    double end = start + 2 * G_PI * self->value / 100.0;

    if (self->value > 0) {
        cairo_arc(cr, cx, cy, radius, start, end);
        cairo_stroke(cr);
    }

    /* Percentage text */
    char text[32];
    g_snprintf(text, sizeof(text), "%.0f%%", self->value);

    cairo_set_source_rgb(cr, 0.25, 0.28, 0.32);
    cairo_select_font_face(
        cr, "Sans",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_BOLD
    );

    cairo_set_font_size(cr, 28);

    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);

    cairo_move_to(
        cr,
        cx - (extents.width / 2 + extents.x_bearing),
        cy - (extents.height / 2 + extents.y_bearing)
    );

    cairo_show_text(cr, text);

    cairo_destroy(cr);
}

/* Preferred widget size */
static void
my_gauge_measure(
    GtkWidget *widget,
    GtkOrientation orientation,
    int for_size,
    int *minimum,
    int *natural,
    int *minimum_baseline,
    int *natural_baseline
)
{
    *minimum = 100;
    *natural = 200;

    if (minimum_baseline)
        *minimum_baseline = -1;

    if (natural_baseline)
        *natural_baseline = -1;
}

/* GObject property getter */
static void
my_gauge_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    MyGauge *self = MY_GAUGE(object);

    switch (property_id) {
        case PROP_VALUE:
            g_value_set_double(value, self->value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(
                object, property_id, pspec
            );
    }
}

/* GObject property setter */
static void
my_gauge_set_property(
    GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec
)
{
    MyGauge *self = MY_GAUGE(object);

    switch (property_id) {
        case PROP_VALUE:
            my_gauge_set_value(
                self, g_value_get_double(value)
            );
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(
                object, property_id, pspec
            );
    }
}

/* Register class behavior */
static void
my_gauge_class_init(MyGaugeClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    widget_class->snapshot = my_gauge_snapshot;
    widget_class->measure = my_gauge_measure;

    object_class->get_property = my_gauge_get_property;
    object_class->set_property = my_gauge_set_property;

    properties[PROP_VALUE] = g_param_spec_double(
        "value",
        "Value",
        "Current gauge value",
        0.0,
        100.0,
        0.0,
        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY |
        G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(
        object_class,
        N_PROPERTIES,
        properties
    );
    
    signals[SIGNAL_VALUE_CHANGED] = g_signal_new(
		"value-changed",
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST,
		0,
		NULL,
		NULL,
		NULL,
		G_TYPE_NONE,
		1,
		G_TYPE_DOUBLE
	);
}

/* Initialize each widget instance */
static void
my_gauge_init(MyGauge *self)
{
    self->value = 0.0;

    gtk_widget_set_overflow(
        GTK_WIDGET(self),
        GTK_OVERFLOW_HIDDEN
    );

    GtkGesture *click = gtk_gesture_click_new();

    g_signal_connect(
        click,
        "pressed",
        G_CALLBACK(my_gauge_clicked),
        self
    );

    gtk_widget_add_controller(
        GTK_WIDGET(self),
        GTK_EVENT_CONTROLLER(click)
    );
}

/* Public constructor */
GtkWidget *
my_gauge_new(void)
{
    return g_object_new(MY_TYPE_GAUGE, NULL);
}

/* Public setter */
void
my_gauge_set_value(MyGauge *self, double value)
{
    g_return_if_fail(MY_IS_GAUGE(self));
    g_return_if_fail(isfinite(value));

    value = CLAMP(value, 0.0, 100.0);

    if (self->value == value)
        return;

    self->value = value;

    gtk_widget_queue_draw(GTK_WIDGET(self));

    g_object_notify_by_pspec(
        G_OBJECT(self),
        properties[PROP_VALUE]
    );
    
    g_signal_emit(
		self,
		signals[SIGNAL_VALUE_CHANGED],
		0,
		self->value
	);
}

/* Public getter */
double
my_gauge_get_value(MyGauge *self)
{
    g_return_val_if_fail(MY_IS_GAUGE(self), 0.0);

    return self->value;
}






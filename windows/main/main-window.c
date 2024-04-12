#include "main-window.h"

#include <glib/gi18n.h>

struct _MainWindow {
    AdwApplicationWindow parent_instance;

    GtkWidget *color_scheme_button;
    GtkDrawingArea *hex_widget;
    GtkScrollbar *scroll_bar;
    GtkButton *test_button;
    GtkButton *erase_button;
    GtkButton *read_button;
    GtkButton *write_button;
    GtkProgressBar *progress_bar;
    GtkButton *status_icon;
    char *hex_buffer;
    int hex_buffer_size;
    int viewer_offset;
    int lines_on_screen_count;
    double scroll;
};

G_DEFINE_FINAL_TYPE (MainWindow, main_window, ADW_TYPE_APPLICATION_WINDOW)

static char *
get_color_scheme_icon_name(gpointer user_data, gboolean dark) {
    return g_strdup (dark ? "light-mode-symbolic" : "dark-mode-symbolic");
}

static void
color_scheme_button_clicked_cb(MainWindow *self) {
    AdwStyleManager *manager = adw_style_manager_get_default();

    if (adw_style_manager_get_dark(manager))
        adw_style_manager_set_color_scheme(manager, ADW_COLOR_SCHEME_FORCE_LIGHT);
    else
        adw_style_manager_set_color_scheme(manager, ADW_COLOR_SCHEME_FORCE_DARK);
}

static void
notify_system_supports_color_schemes_cb(MainWindow *self) {
    AdwStyleManager *manager = adw_style_manager_get_default();
    gboolean supports = adw_style_manager_get_system_supports_color_schemes(manager);

    gtk_widget_set_visible(self->color_scheme_button, !supports);

    if (supports)
        adw_style_manager_set_color_scheme(manager, ADW_COLOR_SCHEME_DEFAULT);
}

static void
main_window_finalize(GObject *gobject) {
    g_free(MAIN_MAIN_WINDOW(gobject)->hex_buffer);
    G_OBJECT_CLASS (main_window_parent_class)->finalize(gobject);
}

static void
main_window_class_init(MainWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = main_window_finalize;

    gtk_widget_class_add_binding_action(widget_class, GDK_KEY_q, GDK_CONTROL_MASK, "window.close", NULL);

    gtk_widget_class_set_template_from_resource(widget_class,
                                                "/dev/alexandro45/ezp2023plus/ui/windows/main/main-window.ui");
    gtk_widget_class_bind_template_child(widget_class, MainWindow, color_scheme_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, hex_widget);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, scroll_bar);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, test_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, erase_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, read_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, write_button);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, progress_bar);
    gtk_widget_class_bind_template_child(widget_class, MainWindow, status_icon);

    gtk_widget_class_bind_template_callback(widget_class, get_color_scheme_icon_name);
    gtk_widget_class_bind_template_callback(widget_class, color_scheme_button_clicked_cb);
}

static void
fill_buf(char *hex_buffer) {
    for (int i = 0; i < 16; i++) {
        hex_buffer[i] = '\xa9';
    }
    for (int i = 16; i < 32; i++) {
        hex_buffer[i] = 'B';
    }
    for (int i = 32; i < 48; i++) {
        hex_buffer[i] = 'C';
    }
}

#define BYTES_PER_LINE 16
#define GAP_POSITION ((BYTES_PER_LINE / 2) - 1)
#define FONT_SIZE 16
#define HEX_SPACING (FONT_SIZE * 1.5)
#define TEXT_SPACING (FONT_SIZE * 0.7)
#define GAP 20
#define HEX_BLOCK_X_OFFSET 70
#define TEXT_BLOCK_X_OFFSET (HEX_BLOCK_X_OFFSET + 430)

static void
hex_widget_draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    GdkRGBA color;
    MainWindow *mv = MAIN_MAIN_WINDOW(data);

    gtk_widget_get_color(GTK_WIDGET (area), &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, FONT_SIZE);

    char buffer[10];
    mv->lines_on_screen_count = height / FONT_SIZE;
    int i = mv->viewer_offset * BYTES_PER_LINE;
    for (int y = 0; y < mv->lines_on_screen_count; ++y) {

        //line numbers
        sprintf(buffer, "%04X", i & 0xffff);
        cairo_move_to(cr, 0, y * FONT_SIZE + FONT_SIZE);
        cairo_show_text(cr, buffer);

        for (int x = 0; x < BYTES_PER_LINE; ++x) {
            //hex
            sprintf(buffer, "%02X", mv->hex_buffer[i] & 0xff);
            cairo_move_to(cr, HEX_BLOCK_X_OFFSET + x * HEX_SPACING + (x > GAP_POSITION ? GAP : 0),
                          y * FONT_SIZE + FONT_SIZE);
            cairo_show_text(cr, buffer);

            //text
//            sprintf(buffer, "%c", mv->hex_buffer[i] & 0xff);
//            cairo_move_to(cr, TEXT_BLOCK_X_OFFSET + x * TEXT_SPACING + (x > GAP_POSITION ? GAP : 0), y * FONT_SIZE + FONT_SIZE);
//            cairo_show_text(cr, buffer);

            i++;
            if (i == mv->hex_buffer_size) break;
        }
        if (i == mv->hex_buffer_size) break;
    }

    cairo_fill(cr);
}

static gboolean scroll_cb(GtkEventControllerScroll *self, gdouble dx, gdouble dy, gpointer user_data) {
    MainWindow *mv = MAIN_MAIN_WINDOW(user_data);

    mv->scroll += dy;
    if (gtk_event_controller_scroll_get_unit(self) == GDK_SCROLL_UNIT_SURFACE) {
        if (fabs(mv->scroll) >= FONT_SIZE) {
            int prev_viewer_offset = mv->viewer_offset;

            while (fabs(mv->scroll) >= FONT_SIZE) {
                mv->viewer_offset += mv->scroll > 0 ? 1 : -1;
                if (mv->scroll > 0) mv->scroll -= FONT_SIZE;
                else if (mv->scroll < 0) mv->scroll += FONT_SIZE;
            }

            if (mv->viewer_offset < 0) mv->viewer_offset = 0;
            if (mv->viewer_offset > mv->hex_buffer_size / BYTES_PER_LINE)
                mv->viewer_offset = mv->hex_buffer_size / BYTES_PER_LINE;

            if (mv->viewer_offset != prev_viewer_offset) {
                gtk_widget_queue_draw(GTK_WIDGET(mv->hex_widget));
                gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(mv->scroll_bar), mv->viewer_offset);
            }
        }
    } else {
        int prev_viewer_offset = mv->viewer_offset;

        mv->viewer_offset += mv->scroll > 0 ? 1 : -1;
        mv->scroll = 0;

        if (mv->viewer_offset < 0) mv->viewer_offset = 0;
        if (mv->viewer_offset > mv->hex_buffer_size / BYTES_PER_LINE)
            mv->viewer_offset = mv->hex_buffer_size / BYTES_PER_LINE;

        if (mv->viewer_offset != prev_viewer_offset) {
            gtk_widget_queue_draw(GTK_WIDGET(mv->hex_widget));
            gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(mv->scroll_bar), mv->viewer_offset);
        }
    }
    return TRUE;
}

static void
scroll_begin(GtkEventControllerScroll *self, gpointer user_data) {
    MAIN_MAIN_WINDOW(user_data)->scroll = 0;
}

static void
scroll_bar_value_cb(GtkAdjustment *adjustment, gpointer user_data) {
    MainWindow *mv = MAIN_MAIN_WINDOW(user_data);

    int prev_viewer_offset = mv->viewer_offset;
    mv->viewer_offset = (int) gtk_adjustment_get_value(adjustment);

    if (prev_viewer_offset != mv->viewer_offset) {
        MAIN_MAIN_WINDOW(user_data)->scroll = 0;
        gtk_widget_queue_draw(GTK_WIDGET(mv->hex_widget));
    }
}

static void
button_clicked(GtkButton *self, gpointer user_data) {
    const char *name = gtk_widget_get_name(GTK_WIDGET(self));

    if (!strcmp(name, "test_button")) {
        printf("Test button clicked!\n");
    } else if (!strcmp(name, "erase_button")) {
        printf("Erase button clicked!\n");
    } else if (!strcmp(name, "read_button")) {
        printf("Read button clicked!\n");
    } else if (!strcmp(name, "write_button")) {
        printf("Write button clicked!\n");
    } else {
        g_warning("Unknown button clicked!");
    }
}

static void
main_window_init(MainWindow *self) {
    AdwStyleManager *manager = adw_style_manager_get_default();

    gtk_widget_init_template(GTK_WIDGET (self));

    g_signal_connect_object(manager,
                            "notify::system-supports-color-schemes",
                            G_CALLBACK (notify_system_supports_color_schemes_cb),
                            self,
                            G_CONNECT_SWAPPED);

    notify_system_supports_color_schemes_cb(self);

    self->hex_buffer_size = 1001;
    self->hex_buffer = g_malloc(self->hex_buffer_size);
    fill_buf(self->hex_buffer);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (self->hex_widget), hex_widget_draw_function, self, NULL);

    GtkEventController *scroll = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect_object(scroll, "scroll", G_CALLBACK (scroll_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(scroll, "scroll-begin", G_CALLBACK (scroll_begin), self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(self->hex_widget), GTK_EVENT_CONTROLLER (scroll));

    GtkAdjustment *scroll_adj = gtk_scrollbar_get_adjustment(self->scroll_bar);
    gtk_adjustment_set_upper(scroll_adj, (int) (self->hex_buffer_size / BYTES_PER_LINE) +
                                         (self->hex_buffer_size % BYTES_PER_LINE == 0 ? 0 : 1));
    g_signal_connect_object(scroll_adj, "value-changed", G_CALLBACK (scroll_bar_value_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(self->test_button, "clicked", G_CALLBACK (button_clicked), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->erase_button, "clicked", G_CALLBACK (button_clicked), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->read_button, "clicked", G_CALLBACK (button_clicked), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(self->write_button, "clicked", G_CALLBACK (button_clicked), self, G_CONNECT_DEFAULT);

    gtk_progress_bar_set_fraction(self->progress_bar, 0.2);
    gtk_progress_bar_set_text(self->progress_bar, "Reading...");
    gtk_progress_bar_set_show_text(self->progress_bar, TRUE);

    gtk_button_set_icon_name(self->status_icon, "status-error");
}

MainWindow *
main_window_new(GtkApplication *application) {
    return g_object_new(MAIN_TYPE_MAIN_WINDOW, "application", application, NULL);
}

/*
 * Copyright (C) 2016, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "indicator-application.h"

#include <string.h>
#include <cairo.h>


static void applet_change_background(MatePanelApplet* applet, MatePanelAppletBackgroundType type, GdkColor* color, cairo_pattern_t *pattern, IndicatorApplication *d);
static void applet_size_changed(MatePanelApplet* applet, int size, IndicatorApplication *d);
static void force_no_focus_padding(GtkWidget* widget);


IndicatorApplication::IndicatorApplication(AppletData *ad) :
    applet(ad->applet),
    orientation(ad->orientation),
    icon_size(16)
{
    tray = na_tray_new_for_screen(gtk_widget_get_screen(GTK_WIDGET(applet)), orientation);
    force_no_focus_padding(GTK_WIDGET(tray));

    alignment = gtk_alignment_new(0.5, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(tray));

    event_box = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    gtk_container_add(GTK_CONTAINER(event_box), alignment);

    change_background_handler_id = g_signal_connect(applet, "change_background",
                                                    G_CALLBACK (applet_change_background), this);
    change_size_handler_id = g_signal_connect(applet, "change_size",
                                              G_CALLBACK(applet_size_changed), this);

    applet_size_changed(applet, mate_panel_applet_get_size(applet), this);
}

IndicatorApplication::~IndicatorApplication()
{
    g_signal_handler_disconnect(applet, change_background_handler_id);
    g_signal_handler_disconnect(applet, change_size_handler_id);
}

static void applet_size_changed(MatePanelApplet* applet, int size, IndicatorApplication *d)
{
    if (d->orientation == GTK_ORIENTATION_HORIZONTAL){
	printf("111111\n");
        gtk_widget_set_size_request(d->event_box, -1, size);
    }
    else{
	printf("222222\n");
        gtk_widget_set_size_request(d->event_box, size, -1);
    }
    if (size < d->icon_size) {
	printf("333333\n");
        gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), 0, 0, 0, 0);
    } else {
        int padding1 = (size - d->icon_size) / 2;
        int padding2 = size - d->icon_size - padding1;
        if (d->orientation == GTK_ORIENTATION_HORIZONTAL){
	    printf("444444\n");
	    printf("size=%d\n",size);
	    printf("icon_size=%d\n",d->icon_size);
	    printf("padding1=%d\n",padding1);
	    printf("padding2=%d\n",padding2);
            gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), padding1, padding2, 0, 0);
	}
        else{
	    printf("555555\n");
            gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), 0, 0, padding1, padding2);
	}
    }
}

static void
applet_change_background(MatePanelApplet* applet,
                         MatePanelAppletBackgroundType type,
                         GdkColor* color,
                         cairo_pattern_t *pattern,
                         IndicatorApplication *d)
{
    g_return_if_fail(d);
    g_return_if_fail(d->tray);
    g_return_if_fail(NA_IS_TRAY(d->tray));

    na_tray_force_redraw(d->tray);
}

static void force_no_focus_padding(GtkWidget* widget)
{
    static gboolean first_time = TRUE;

    if (first_time)
    {
        gtk_rc_parse_string ("\n"
                             "style \"na-tray-style\"\n"
                             "{\n"
                             "    GtkWidget::focus-line-width=0\n"
                             "    GtkWidget::focus-padding=0\n"
                             "}\n"
                             "\n"
                             "    widget \"*.PanelAppletNaTray\" style \"na-tray-style\"\n"
                             "\n");

        first_time = FALSE;
    }

    /* The widget used to be called na-tray
     *
     * Issue #27
     */
    gtk_widget_set_name(widget, "PanelAppletNaTray");
}

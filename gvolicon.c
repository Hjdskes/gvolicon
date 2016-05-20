/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gvolicon.c
 * Copyright (C) 2013-2014 Jente Hidskes <hjdskes@gmail.com>
 *
 * Option parsing borrowed from cbatticon
 * Copyright (C) 2011-2013 Colin Jones <xentalion@gmail.com> 
 *
 * Based on code by fogobogo, who granted permission to license this under GPLv3
 *
 * Gvolicon is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gvolicon is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <alsa/asoundlib.h>

#define DEFAULT_VOLUME_STEP       5
#define DEFAULT_UPDATE_INTERVAL   1

/* some icon sets have a panel-* or *-panel version too */
#define ICON_MUTE                "audio-volume-muted"
#define ICON_LOW                 "audio-volume-low"
#define ICON_MEDIUM              "audio-volume-medium"
#define ICON_HIGH                "audio-volume-high"
#define ICON_MUTE_SYMBOLIC       "audio-volume-muted-symbolic"
#define ICON_LOW_SYMBOLIC        "audio-volume-low-symbolic"
#define ICON_MEDIUM_SYMBOLIC     "audio-volume-medium-symbolic"
#define ICON_HIGH_SYMBOLIC       "audio-volume-high-symbolic"

enum {
	MUTED = 0,
	UNMUTED
};

enum {
	STANDARD = 0,
	SYMBOLIC
};

struct volume {
	gchar *mixer;
	gchar *device;
	gint   has_playback_switch;
	gint   realvol;
	gint   oldvol;
	gint   mute;
	gint   oldmute;
	long   vol;
	long   max;
} vol;

struct configuration {
	gint update_interval;
	gint volume_step;
	gint icon_type;
} configuration = {
	DEFAULT_UPDATE_INTERVAL,
	DEFAULT_VOLUME_STEP,
	STANDARD
};

static snd_mixer_t *handle;
static snd_mixer_selem_id_t *vol_info;

static int
get_options(int argc, char **argv) {
	GOptionContext *option_context;
	GError *error = NULL;
	gboolean use_symbolic_icons = FALSE, display_version = FALSE;

	GOptionEntry option_entries[] = {
		{ "version",         'v', 0, G_OPTION_ARG_NONE,   &display_version,               "Display version and exit",                                NULL },
		{ "device",          'd', 0, G_OPTION_ARG_STRING, &vol.device,                    "Device to use",                                           NULL },
		{ "mixer",           'm', 0, G_OPTION_ARG_STRING, &vol.mixer,                     "Mixer to use",                                            NULL },
		{ "update-interval", 'u', 0, G_OPTION_ARG_INT,    &configuration.update_interval, "Set update interval in seconds",                          NULL },
		{ "volume-step",     's', 0, G_OPTION_ARG_INT,    &configuration.volume_step,     "Set volume step to increase or decrease the volume with", NULL },
		{ "symbolic-icons",  'i', 0, G_OPTION_ARG_NONE,   &use_symbolic_icons,            "Use symbolic icons",                                      NULL },
		{ NULL },
	};

	option_context = g_option_context_new(NULL);
	g_option_context_add_main_entries(option_context, option_entries, NULL);
	g_option_context_add_group(option_context, gtk_get_option_group(TRUE));

	if(g_option_context_parse(option_context, &argc, &argv, &error) == FALSE) {
		g_printerr("%s: can not parse command line arguments: %s\n", argv[0], error->message);
		g_error_free(error);
		error = NULL;
		return -1;
	}

	g_option_context_free(option_context);

	if(display_version == TRUE) {
		g_print("%s: a simple and lightweight volume icon that sits in your system tray. Version %s.\n", argv[0], VERSION);
		return -1;
	}

	if(use_symbolic_icons == TRUE) {
		GtkIconTheme *theme;
		theme = gtk_icon_theme_get_default();
		if(gtk_icon_theme_has_icon(theme, ICON_MUTE_SYMBOLIC) == TRUE)
			configuration.icon_type = SYMBOLIC;
		else {
			configuration.icon_type = STANDARD;
			g_printerr("%s: can not find symbolic icons! Falling back to standard icons.\n", argv[0]);
		}
	}

	if(configuration.volume_step <= 0) {
		configuration.volume_step = DEFAULT_VOLUME_STEP;
		g_printerr("%s: invalid volume step! It has been reset to default (%d).\n", argv[0], DEFAULT_VOLUME_STEP);
	}

	if(configuration.update_interval <= 0) {
		configuration.update_interval = DEFAULT_UPDATE_INTERVAL;
		g_printerr("%s: invalid update interval! It has been reset to default (%d seconds).\n", argv[0], DEFAULT_UPDATE_INTERVAL);
	}

	return 0;
}

static void
tray_icon_set_icon(GtkStatusIcon *icon) {
	snd_mixer_handle_events(handle);

	if(vol.has_playback_switch && vol.mute == MUTED) {
		gtk_status_icon_set_from_icon_name(icon, (configuration.icon_type == STANDARD) ? ICON_MUTE : ICON_MUTE_SYMBOLIC);
		return;
	}

	if(!vol.has_playback_switch && vol.realvol == 0)
		gtk_status_icon_set_from_icon_name(icon, (configuration.icon_type == STANDARD) ? ICON_MUTE : ICON_MUTE_SYMBOLIC);
	else if(vol.realvol > 0 && vol.realvol <= 33)
		gtk_status_icon_set_from_icon_name(icon, (configuration.icon_type == STANDARD) ? ICON_LOW : ICON_LOW_SYMBOLIC);
	else if(vol.realvol > 33 && vol.realvol <= 66)
		gtk_status_icon_set_from_icon_name(icon, (configuration.icon_type == STANDARD) ? ICON_MEDIUM : ICON_MEDIUM_SYMBOLIC);
	else
		gtk_status_icon_set_from_icon_name(icon, (configuration.icon_type == STANDARD) ? ICON_HIGH : ICON_HIGH_SYMBOLIC);
}

static void
tray_icon_set_tooltip(GtkStatusIcon *icon) {
	gchar tooltip[24];

	snd_mixer_handle_events(handle);

	if(vol.has_playback_switch && vol.mute == MUTED)
		g_sprintf(tooltip, "Volume: %d%% - Muted", vol.realvol);
	else if(!vol.has_playback_switch && vol.realvol == 0)
		g_sprintf(tooltip, "Muted");
	else
		g_sprintf(tooltip, "Volume: %d%%", vol.realvol);

	gtk_status_icon_set_tooltip_text(icon, tooltip);
}

static void
tray_icon_on_scroll(GtkStatusIcon *icon, GdkEventScroll *event) {
	snd_mixer_elem_t *elem;

	if(event->direction == GDK_SCROLL_UP)
		vol.realvol += configuration.volume_step;
	/* if we try to go below zero the volume jumps up to 100. This is to protect our ears */
	else if(event->direction == GDK_SCROLL_DOWN && vol.realvol != 0)
        vol.realvol -= configuration.volume_step;

	snd_mixer_handle_events(handle);
	elem = snd_mixer_find_selem(handle, vol_info);
	snd_mixer_selem_set_playback_volume_all(elem, (double)vol.max * vol.realvol / 100);

    tray_icon_set_icon(icon);
    tray_icon_set_tooltip(icon);
}

static void
tray_icon_on_click(GtkStatusIcon *icon) {
	snd_mixer_elem_t *elem;

	snd_mixer_handle_events(handle);
	elem = snd_mixer_find_selem(handle, vol_info);

	if(vol.has_playback_switch)
		snd_mixer_selem_set_playback_switch_all(elem, !vol.mute);
	else {
		if(vol.realvol != 0) {
			vol.oldvol = vol.realvol;
			vol.realvol = 0;
		} else
			vol.realvol = vol.oldvol;
		snd_mixer_selem_set_playback_volume_all(elem, (double)vol.max * vol.realvol / 100);
	}

	tray_icon_set_icon(icon);
	tray_icon_set_tooltip(icon);
}

static void
tray_icon_check_for_update(GtkStatusIcon *icon)  {
	snd_mixer_elem_t *elem;
	long min; //not used, but segfault if NULL in call below

	snd_mixer_handle_events(handle);
	elem = snd_mixer_find_selem(handle, vol_info);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &vol.max);
	snd_mixer_selem_get_playback_volume(elem, 0, &vol.vol);
	if(vol.has_playback_switch)
		snd_mixer_selem_get_playback_switch(elem, 0, &vol.mute);

	vol.realvol = vol.vol * 100 / vol.max;

	if(vol.realvol != vol.oldvol) {
		vol.oldvol = vol.realvol;

		tray_icon_set_icon(icon);
		tray_icon_set_tooltip(icon);
	} else if(vol.has_playback_switch && vol.mute != vol.oldmute) {
		vol.oldmute = vol.mute;

		tray_icon_set_icon(icon);
		tray_icon_set_tooltip(icon);
	}
}

int
main(int argc, char **argv) {
	GtkStatusIcon *tray_icon;
	snd_mixer_elem_t *elem;

	if(get_options(argc, argv) < 0)
		exit(EXIT_FAILURE);

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, vol.device ? vol.device : "default");
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_malloc(&vol_info);
	snd_mixer_selem_id_set_name(vol_info, vol.mixer ? vol.mixer : "Master");

	elem = snd_mixer_find_selem(handle, vol_info);
	if(elem == NULL) {
		g_printerr("%s: unable to open device! Exiting\n", argv[0]);
		if(vol_info)
			snd_mixer_selem_id_free(vol_info);
		if(handle)
			snd_mixer_close(handle);
		exit(EXIT_FAILURE);
	}

	/* initialize some values */
	vol.has_playback_switch = snd_mixer_selem_has_playback_switch(elem);
	if(vol.has_playback_switch) {
		snd_mixer_selem_get_playback_switch(elem, 0, &vol.mute);
		vol.oldmute = vol.mute;
	}

	gtk_init(&argc, &argv);

	tray_icon = gtk_status_icon_new();
	tray_icon_set_icon(tray_icon);
	gtk_status_icon_set_visible(tray_icon, TRUE);
	g_signal_connect(tray_icon, "activate", G_CALLBACK(tray_icon_on_click), NULL);
	g_signal_connect(tray_icon, "scroll-event", G_CALLBACK(tray_icon_on_scroll), NULL);
	g_timeout_add_seconds(configuration.update_interval, (GSourceFunc)tray_icon_check_for_update, tray_icon);

	gtk_main();

	if(vol_info)
		snd_mixer_selem_id_free(vol_info);
	if(handle)
		snd_mixer_close(handle);

	exit(EXIT_SUCCESS);
}

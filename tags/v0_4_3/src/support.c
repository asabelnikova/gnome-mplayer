/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * support.c
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 * 
 * support.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * callbacks.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with callbacks.h.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include "support.h"

gint detect_playlist(gchar * filename)
{

    FILE *fp;
    gint playlist = 0;
    gchar buffer[16 * 1024];
    gint size;

    fp = fopen(filename, "r");

    if (fp != NULL) {
        if (!feof(fp)) {
            memset(buffer, 0, sizeof(buffer));
            size = fread(buffer, 1, sizeof(buffer) - 1, fp);

            //printf("buffer=%s\n",buffer);
            if (strstr(g_strdown(buffer), "[playlist]") != 0) {
                playlist = 1;
            }

            if (strstr(g_strdown(buffer), "[reference]") != 0) {
                playlist = 1;
            }
			
            if (strstr(g_strdown(buffer), "<asx") != 0) {
                playlist = 1;
            }

            if (strstr(g_strdown(buffer), "http://") != 0) {
                playlist = 1;
            }

            if (strstr(g_strdown(buffer), "rtsp://") != 0) {
                playlist = 1;
            }
			

        }
        fclose(fp);
    }
    printf("playlist detection = %i\n", playlist);
    return playlist;
}

gboolean update_mplayer_config()
{

    GKeyFile *config = g_key_file_new();
    gchar *data;
    gchar *def;
    GError *error;
    gchar *filename;

    error = NULL;

    filename = g_strdup_printf("%s/.mplayer/config", getenv("HOME"));
    g_key_file_load_from_file(config,
                              filename,
                              G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);

    if (error != NULL) {
        //printf("%i\n%s", error->code, error->message);
        if (error->code == 4) {
            g_file_get_contents(filename, &data, NULL, NULL);
            //printf("%s\n", data);
            def = g_strconcat("[default]\n", data, NULL);
            g_file_set_contents(filename, def, -1, NULL);
			g_free(data);
			g_free(def);
			g_error_free(error);
			error = NULL;
            g_key_file_load_from_file(config,
                                      filename,
                                      G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS,
                                      &error);
        }
    }

	if (vo != NULL) {
		g_key_file_set_string(config,"gnome-mplayer","vo",vo);
	} else {
		g_key_file_remove_key(config,"gnome-mplayer","vo",NULL);
	}

	if (ao != NULL) {
		g_key_file_set_string(config,"gnome-mplayer","ao",ao);
	} else {
		g_key_file_remove_key(config,"gnome-mplayer","ao",NULL);
	}
	
    data = g_key_file_to_data(config, NULL, NULL);
    //printf("%i\n%s", strlen(data), data);

    g_file_set_contents(filename, data, -1, NULL);
    g_free(data);
    g_free(filename);
    g_key_file_free(config);

    return TRUE;
}

gboolean read_mplayer_config()
{

    GKeyFile *config = g_key_file_new();
    gchar *data;
    gchar *def;
    GError *error;
    gchar *filename;

    error = NULL;

    filename = g_strdup_printf("%s/.mplayer/config", getenv("HOME"));
    g_key_file_load_from_file(config,
                              filename,
                              G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);

    if (error != NULL) {
        //printf("%i\n%s\n", error->code, error->message);
        if (error->code == 4) {
            g_file_get_contents(filename, &data, NULL, NULL);
            //printf("%s\n", data);
            def = g_strconcat("[default]\n", data, NULL);
            g_file_set_contents(filename, def, -1, NULL);
			g_free(data);
			
			g_error_free(error);
			error = NULL;
            g_key_file_load_from_data(config,
                                      (const gchar*)def, strlen(def), 
                                      G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS,
                                      &error);
			g_free(def);
			
        }
    }

	if (vo != NULL) g_free(vo);
	if (ao != NULL) g_free(ao);
	vo = g_key_file_get_string(config,"gnome-mplayer","vo",NULL);
	ao = g_key_file_get_string(config,"gnome-mplayer","ao",NULL);
	
	
    g_free(filename);
    g_key_file_free(config);

	printf("vo = %s ao = %s\n",vo,ao);
    return TRUE;
}
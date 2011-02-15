/*
 * Auto mail forward Plug-in -- forward functionality plug-in for Sylpheed
 * Copyright (C) 2011 HAYASHI Kentaro <kenhys@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "defs.h"

#include <glib.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <sys/stat.h>

#include "sylmain.h"
#include "plugin.h"
#include "procmsg.h"
#include "procmime.h"
#include "utils.h"

#include <glib/gi18n-lib.h>

static SylPluginInfo info = {
	"Auto mail forward Plug-in",
	"0.1.0",
	"HAYASHI Kentaro",
	"Automatically forwarding mail plug-in for Sylpheed"
};

static void exec_autoforward_cb(GObject *obj, FolderItem *item, const gchar *file, guint num);
static void exec_autoforward_menu_cb(void);

void plugin_load(void)
{
	debug_print("initializing autoforward plug-in\n");

	syl_plugin_add_menuitem("/Tools", NULL, NULL, NULL);
	syl_plugin_add_menuitem("/Tools", _("Enable autoforward"), exec_autoforward_menu_cb, NULL);

    g_signal_connect(syl_app_get(), "add-msg", G_CALLBACK(exec_autoforward_cb), NULL);

    debug_print("autoforward_tool plug-in loading done.\n");
}

void plugin_unload(void)
{
	debug_print("autoforward_tool plug-in unloaded.\n");
}

SylPluginInfo *plugin_info(void)
{
	return &info;
}

gint plugin_interface_version(void)
{
	return SYL_PLUGIN_INTERFACE_VERSION;
}

static gboolean g_enable = FALSE;

static void exec_autoforward_menu_cb(void)
{
	MimeInfo *mimeinfo;
	FILE *fp, *outfp;
	gchar *infile, *outfile;
	gboolean err = FALSE;
	debug_print("exec_autoforward\n");

    if (g_enable != TRUE){
        debug_print("enable exec_autoforward_cb\n");
        g_enable=TRUE;
    }else{
        debug_print("disable exec_autoforward_cb\n");
        g_enable=FALSE;
    }
}

void exec_autoforward_cb(GObject *obj, FolderItem *item, const gchar *file, guint num)
{
	debug_print("exec_autoforward_cb\n");
	debug_print("guint num:%d\n", num);
    if (g_enable!=TRUE){
        return;
    }
}

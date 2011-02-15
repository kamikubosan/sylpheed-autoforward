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
#include "alertpanel.h"

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
	syl_plugin_add_menuitem("/Tools", _("Toggle autoforward"), exec_autoforward_menu_cb, NULL);

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
        syl_plugin_alertpanel_message(_("Autoforward"), _("autoforward plugin is enabled."), ALERT_NOTICE);
        debug_print("enable exec_autoforward_cb\n");
        g_enable=TRUE;
    }else{
        syl_plugin_alertpanel_message(_("Autoforward"), _("autoforward plugin is disabled."), ALERT_NOTICE);
        debug_print("disable exec_autoforward_cb\n");
        g_enable=FALSE;
    }
}

void exec_autoforward_cb(GObject *obj, FolderItem *item, const gchar *file, guint num)
{
	debug_print("[PLUGIN] exec_autoforward_cb\n");
    debug_print("[PLUGIN] file:%s\n", file);
	debug_print("[PLUGIN] guint num:%d\n", num);
    if (g_enable!=TRUE){
        return;
    }
    if (item->stype != F_NORMAL && item->stype != F_INBOX){
        debug_print("[PLUGIN] neither F_NORMAL nor F_INBOX\n");
        return;
    }
    
    PrefsAccount *ac = (PrefsAccount*)account_get_default();
    debug_print("[PLUGIN] check account address\n");
    g_return_if_fail(ac != NULL);
    
    debug_print("[PLUGIN] account address:%s\n", ac->address);
    syl_plugin_send_message_set_forward_flags(ac->address);
    debug_print("[PLUGIN] syl_plugin_send_message_set_forward_flags done.\n");

	FILE *fp;
    gchar *rcpath;
    GSList* to_list=NULL;

    gchar buf[PREFSBUFSIZE];
	rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, "autoforwardrc", NULL);
	g_free(rcpath);

    debug_print("[PLUGIN] rcpath:%s\n", rcpath);
	if ((fp = g_fopen(rcpath, "rb")) == NULL) {
		if (ENOENT != errno) FILE_OP_ERROR(rcpath, "fopen");
		g_free(rcpath);
		return;
	}
	g_free(rcpath);

    debug_print("[PLUGIN] read rcpath:%s\n", rcpath);
    while (fgets(buf, sizeof(buf), fp) != NULL) {
		g_strstrip(buf);
		if (buf[0] == '\0') continue;
        to_list = address_list_append(to_list, buf);
	}
	fclose(fp);

    debug_print("[PLUGIN] check to_list\n");
    g_return_if_fail(to_list != NULL);

    syl_plugin_send_message(file, ac, to_list);
    debug_print("[PLUGIN] syl_plugin_send_message done.\n");
}

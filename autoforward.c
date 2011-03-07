/*
 * Auto mail forward Plug-in
 *  -- forward received mail to address described in autoforwardrc.
 * Copyright (C) 2011 HAYASHI Kentaro <kenhys@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "prefs_common.h"
#include "online.xpm"
#include "offline.xpm"

#include <glib/gi18n-lib.h>
#include <locale.h>

static SylPluginInfo info = {
	N_("Auto mail forward Plug-in"),
	"0.4.0",
	"HAYASHI Kentaro",
	N_("Automatically forwarding mail plug-in for Sylpheed")
};

static gboolean g_enable = FALSE;

static void exec_autoforward_cb(GObject *obj, FolderItem *item, const gchar *file, guint num);
static void exec_autoforward_menu_cb(void);

static GtkWidget *g_plugin_on = NULL;
static GtkWidget *g_plugin_off = NULL;
static GtkWidget *g_onoff_switch = NULL;
static GtkTooltips *g_tooltip = NULL;

void plugin_load(void)
{
  syl_init_gettext("autoforward", ".");
  textdomain("autoforward");


  syl_plugin_add_menuitem("/Tools", NULL, NULL, NULL);
	syl_plugin_add_menuitem("/Tools", _("Toggle autoforward"), exec_autoforward_menu_cb, NULL);

    g_signal_connect(syl_app_get(), "add-msg", G_CALLBACK(exec_autoforward_cb), NULL);

    GtkWidget *mainwin = syl_plugin_main_window_get();
    GtkWidget *statusbar = syl_plugin_main_window_get_statusbar();
    GtkWidget *plugin_box = gtk_hbox_new(FALSE, 0);

    GdkPixbuf* on_pixbuf = gdk_pixbuf_new_from_xpm_data((const char**)online_xpm);
    g_plugin_on=gtk_image_new_from_pixbuf(on_pixbuf);
    /*g_plugin_on = gtk_label_new(_("AF ON"));*/
    
    GdkPixbuf* off_pixbuf = gdk_pixbuf_new_from_xpm_data((const char**)offline_xpm);
    g_plugin_off=gtk_image_new_from_pixbuf(off_pixbuf);
    /*g_plugin_off = gtk_label_new(_("AF OFF"));*/

    gtk_box_pack_start(GTK_BOX(plugin_box), g_plugin_on, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(plugin_box), g_plugin_off, FALSE, FALSE, 0);
    
    g_tooltip = gtk_tooltips_new();
    
    g_onoff_switch = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(g_onoff_switch), GTK_RELIEF_NONE);
	GTK_WIDGET_UNSET_FLAGS(g_onoff_switch, GTK_CAN_FOCUS);
	gtk_widget_set_size_request(g_onoff_switch, 20, 20);

    gtk_container_add(GTK_CONTAINER(g_onoff_switch), plugin_box);
	g_signal_connect(G_OBJECT(g_onoff_switch), "clicked",
                     G_CALLBACK(exec_autoforward_menu_cb), mainwin);
	gtk_box_pack_start(GTK_BOX(statusbar), g_onoff_switch, FALSE, FALSE, 0);

    gtk_widget_show_all(g_onoff_switch);
    gtk_widget_hide(g_plugin_on);

}

void plugin_unload(void)
{
}

SylPluginInfo *plugin_info(void)
{
	return &info;
}

gint plugin_interface_version(void)
{
	return SYL_PLUGIN_INTERFACE_VERSION;
}

static void exec_autoforward_menu_cb(void)
{

    if (g_enable != TRUE){
        syl_plugin_alertpanel_message(_("Autoforward"), _("autoforward plugin is enabled."), ALERT_NOTICE);
        g_enable=TRUE;
        gtk_widget_hide(g_plugin_off);
        gtk_widget_show(g_plugin_on);
        gtk_tooltips_set_tip
			(g_tooltip, g_onoff_switch,
			 _("Autoforward is enabled. Click the icon to disable plugin."),
			 NULL);
    }else{
        syl_plugin_alertpanel_message(_("Autoforward"), _("autoforward plugin is disabled."), ALERT_NOTICE);
        g_enable=FALSE;
        gtk_widget_hide(g_plugin_on);
        gtk_widget_show(g_plugin_off);
        gtk_tooltips_set_tip
			(g_tooltip, g_onoff_switch,
			 _("Autoforward is disabled. Click the icon to enable plugin."),
			 NULL);
    }
}

void exec_autoforward_cb(GObject *obj, FolderItem *item, const gchar *file, guint num)
{
    if (g_enable!=TRUE){
        return;
    }
    if (item->stype != F_NORMAL && item->stype != F_INBOX){
        return;
    }

    PrefsCommon *prefs_common = prefs_common_get();
    if (prefs_common->online_mode != TRUE){
        return;
    }
    
    PrefsAccount *ac = (PrefsAccount*)account_get_default();
    g_return_if_fail(ac != NULL);
    
    syl_plugin_send_message_set_forward_flags(ac->address);

	FILE *fp;
    gchar *rcpath;
    GSList* to_list=NULL;

    gchar buf[PREFSBUFSIZE];
	rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, "autoforwardrc", NULL);

	if ((fp = g_fopen(rcpath, "rb")) == NULL) {
		if (ENOENT != errno) FILE_OP_ERROR(rcpath, "fopen");
		g_free(rcpath);
		return;
	}
	g_free(rcpath);

    while (fgets(buf, sizeof(buf), fp) != NULL) {
		g_strstrip(buf);
		if (buf[0] == '\0') continue;
        to_list = address_list_append(to_list, buf);
	}
	fclose(fp);

    g_return_if_fail(to_list != NULL);

    syl_plugin_send_message(file, ac, to_list);
}

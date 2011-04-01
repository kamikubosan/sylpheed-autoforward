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


#include <glib.h>
#include <glib/gi18n-lib.h>
#include <locale.h>

#define _(String) dgettext("autoforward", String)
#define N_(String) gettext_noop(String)
#define gettext_noop(String) (String)

#define PLUGIN_NAME N_("Auto mail forward Plug-in")
#define PLUGIN_DESC N_("Automatically forwarding mail plug-in for Sylpheed")

static SylPluginInfo info = {
	N_(PLUGIN_NAME),
	"0.5.0",
	"HAYASHI Kentaro",
	N_(PLUGIN_DESC)
};

static gboolean g_enable = FALSE;

static void exec_autoforward_cb(GObject *obj, FolderItem *item, const gchar *file, guint num);
static void exec_autoforward_menu_cb(void);
static void exec_autoforward_onoff_cb(void);

static GtkWidget *g_plugin_on = NULL;
static GtkWidget *g_plugin_off = NULL;
static GtkWidget *g_onoff_switch = NULL;
static GtkTooltips *g_tooltip = NULL;
static GKeyFile *g_keyfile=NULL;

void plugin_load(void)
{
  syl_init_gettext("autoforward", "lib/locale");
  
  debug_print(gettext("Auto mail forward Plug-in"));
  debug_print(dgettext("autoforward", "Auto mail forward Plug-in"));

  syl_plugin_add_menuitem("/Tools", NULL, NULL, NULL);
  syl_plugin_add_menuitem("/Tools", _("Autoforward Settings"), exec_autoforward_menu_cb, NULL);

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
                     G_CALLBACK(exec_autoforward_onoff_cb), mainwin);
	gtk_box_pack_start(GTK_BOX(statusbar), g_onoff_switch, FALSE, FALSE, 0);

    gtk_widget_show_all(g_onoff_switch);
    gtk_widget_hide(g_plugin_on);
    info.name = g_strdup(_(PLUGIN_NAME));
    info.description = g_strdup(_(PLUGIN_DESC));

	gchar *rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, "autoforwardrc", NULL);
    g_keyfile = g_key_file_new();
    if (g_key_file_load_from_file(g_keyfile, rcpath, G_KEY_FILE_KEEP_COMMENTS, NULL)){
        gchar *startup=g_key_file_get_string (g_keyfile, "forward", "startup", NULL);
        debug_print("startup:%s", startup);
        g_free(rcpath);
        if (strcmp("true", startup)==0){
            g_enable=TRUE;
            gtk_widget_hide(g_plugin_off);
            gtk_widget_show(g_plugin_on);
            gtk_tooltips_set_tip
                (g_tooltip, g_onoff_switch,
                 _("Autoforward is enabled. Click the icon to disable plugin."),
                 NULL);
        }
    }
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

static GtkWidget *g_address;
static GtkWidget *g_startup;
static GtkWidget *g_from;
static GtkWidget *g_to;

static void prefs_ok_cb(GtkWidget *widget, gpointer data)
{
	gchar *rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, "autoforwardrc", NULL);
    g_keyfile = g_key_file_new();
    g_key_file_load_from_file(g_keyfile, rcpath, G_KEY_FILE_KEEP_COMMENTS, NULL);

    gchar *address = gtk_entry_get_text(GTK_ENTRY(g_address));
    if (address!=NULL){
        g_key_file_set_string (g_keyfile, "forward", "to", address);
    }
    gboolean startup = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(g_startup));
    g_key_file_set_boolean (g_keyfile, "forward", "startup", startup);
    debug_print("startup:%d\n", startup);

    /**/
    gsize sz;
    gchar *buf=g_key_file_to_data(g_keyfile, &sz, NULL);
    g_file_set_contents(rcpath, buf, sz, NULL);
    
	g_free(rcpath);

    gtk_widget_destroy(GTK_WIDGET(data));
}
static void prefs_cancel_cb(GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
}

static void exec_autoforward_menu_cb(void)
{
    /* show modal dialog */
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *confirm_area;
    GtkWidget *ok_btn;
    GtkWidget *cancel_btn;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 8);
	/*gtk_widget_set_size_request(window, 200, 100);*/
	gtk_window_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(window), TRUE);
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, TRUE, FALSE);
	gtk_widget_realize(window);

    vbox = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	confirm_area = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(confirm_area), GTK_BUTTONBOX_END);
	gtk_box_set_spacing(GTK_BOX(confirm_area), 6);


    ok_btn = gtk_button_new_from_stock(GTK_STOCK_OK);
    GTK_WIDGET_SET_FLAGS(ok_btn, GTK_CAN_DEFAULT);
    gtk_box_pack_start(GTK_BOX(confirm_area), ok_btn, FALSE, FALSE, 0);
    gtk_widget_show(ok_btn);

    cancel_btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    GTK_WIDGET_SET_FLAGS(cancel_btn, GTK_CAN_DEFAULT);
    gtk_box_pack_start(GTK_BOX(confirm_area), cancel_btn, FALSE, FALSE, 0);
    gtk_widget_show(cancel_btn);

    gtk_widget_show(confirm_area);
	
    gtk_box_pack_end(GTK_BOX(vbox), confirm_area, FALSE, FALSE, 0);
	gtk_widget_grab_default(ok_btn);

    gtk_window_set_title(GTK_WINDOW(window), _("Autoforward Settings"));

    g_signal_connect(G_OBJECT(ok_btn), "clicked",
                     G_CALLBACK(prefs_ok_cb), window);
	g_signal_connect(G_OBJECT(cancel_btn), "clicked",
                     G_CALLBACK(prefs_cancel_cb), window);

	/* email settings */
    GtkWidget *hbox = gtk_hbox_new(FALSE, 6);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *label = gtk_label_new(_("Forward to(E-mail):"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    g_address = gtk_entry_new();
    gtk_widget_show(g_address);
	gtk_box_pack_start(GTK_BOX(hbox), g_address, FALSE, TRUE, 0);

	/* email settings */
	g_startup = gtk_check_button_new_with_label(_("Enable plugin on startup"));
	gtk_widget_show(g_startup);
	gtk_box_pack_start(GTK_BOX(vbox), g_startup, FALSE, FALSE, 0);

    /* all */
    GtkWidget *radio1 = gtk_radio_button_new_with_label(NULL, _("Forward all mail"));
    /* folder filtered */
    GtkWidget *radio2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio1),
                                                                     _("Forward mail in folder"));

    GtkWidget *frame = gtk_frame_new(_("Forward condition:"));
	gtk_widget_show(frame);
	gtk_container_add(GTK_CONTAINER(vbox), frame);
    
    GtkWidget *vbox_cond = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(vbox_cond);
	gtk_container_add(GTK_CONTAINER(frame), vbox_cond);
    
    gtk_box_pack_start(GTK_BOX(vbox_cond), radio1, FALSE, FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox_cond), radio2, FALSE, FALSE, 6);

    /* treeview */
    GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);
   GtkTreeIter iter;
   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "foo", -1);

   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

      gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

   gtk_list_store_append(store, &iter);
   gtk_list_store_set(store, &iter, 0, "bar", -1);

   GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes ("Forward mail in following folder:",
                                                    gtk_cell_renderer_text_new (),
                                                    "text", 0,
                                                    NULL);
   GtkWidget *view=gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
   gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
   GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_container_add (GTK_CONTAINER (sw), view);
   gtk_box_pack_start (GTK_BOX(vbox_cond), sw, TRUE, TRUE, 2);




    /* add and delete */
    GtkWidget *bbox = gtk_hbutton_box_new();
    gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(bbox), 6);
    GtkWidget *btn_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
    GtkWidget *btn_delete = gtk_button_new_from_stock(GTK_STOCK_DELETE);

    gtk_box_pack_start(GTK_BOX(bbox), btn_add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bbox), btn_delete, FALSE, FALSE, 0);
    gtk_widget_show_all(bbox);
    
    gtk_box_pack_end(GTK_BOX(vbox_cond), bbox, FALSE, FALSE, 6);
    gtk_widget_show_all(vbox_cond);
    
    /* load settings */
    gchar *rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, "autoforwardrc", NULL);
    g_keyfile = g_key_file_new();
    if (g_key_file_load_from_file(g_keyfile, rcpath, G_KEY_FILE_KEEP_COMMENTS, NULL)){
        gchar *startup=g_key_file_get_string (g_keyfile, "forward", "startup", NULL);
        debug_print("startup:%s", startup);
        if (strcmp(startup, "true")==0){
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_startup), TRUE);
        }
        gchar *to=g_key_file_get_string (g_keyfile, "forward", "to", NULL);
        gtk_entry_set_text(GTK_ENTRY(g_address), to);
    }
    g_free(rcpath);

    gtk_widget_show(window);
}


static void exec_autoforward_onoff_cb(void)
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

    /* check item->path for filter */
    g_print("%s\n", item->name);
    g_print("%s\n", item->path);

    syl_plugin_send_message_set_forward_flags(ac->address);

	FILE *fp;
    gchar *rcpath;
    GSList* to_list=NULL;

    gchar buf[PREFSBUFSIZE];
	rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, "autoforwardrc", NULL);

    g_keyfile = g_key_file_new();
    g_key_file_load_from_file(g_keyfile, rcpath, G_KEY_FILE_KEEP_COMMENTS, NULL);
    gchar *to=g_key_file_get_string (g_keyfile, "forward", "to", NULL);
    debug_print("to:%s", to);
    to_list = address_list_append(to_list, to);

    gsize gz=0;
    gchar **folders = g_key_file_get_string_list(g_keyfile, "forward", "folder", &gz, NULL);
    gboolean bmatch = FALSE;
    if (gz != 0) {
        /* match or not */
        int nindex = 0;
        for (nindex = 0; nindex < gz; nindex++){
            if (memcmp(folders[nindex], item->path, strlen(folders[nindex])) == 0){
                bmatch = TRUE;
            }
        }
    } else {
        bmatch = TRUE;
    }
    g_free(rcpath);
    g_return_if_fail(to_list != NULL);

    g_return_if_fail(bmatch == TRUE);
    
    syl_plugin_send_message(file, ac, to_list);
}

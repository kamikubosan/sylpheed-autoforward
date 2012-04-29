/*
 * Auto mail forward Plug-in
 *  -- forward received mail to address described in autoforwardrc.
 * Copyright (C) 2011-2012 HAYASHI Kentaro <kenhys@gmail.com>
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

#ifndef AUTOFOWARD_H_INCLUDED
#define AUTOFOWARD_H_INCLUDED

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
#include "foldersel.h"
#include "../res/online.xpm"
#include "../res/offline.xpm"


#include <glib.h>
#include <glib/gi18n-lib.h>
#include <locale.h>

#define _(String) dgettext("autoforward", String)
#define N_(String) gettext_noop(String)
#define gettext_noop(String) (String)

#define PLUGIN_NAME N_("Auto mail forward Plug-in")
#define PLUGIN_DESC N_("Automatically forwarding mail plug-in for Sylpheed")

#endif

/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * Screenshot plug-in
 * Copyright 1998-2007 Sven Neumann <sven@gimp.org>
 * Copyright 2003      Henrik Brix Andersen <brix@gimp.org>
 * Copyright 2016      Michael Natterer <mitch@gimp.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <glib.h>
#include <glib/gstdio.h> /* g_unlink() */

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "screenshot.h"
#include "screenshot-gnome-shell.h"


static GDBusProxy *proxy = NULL;


gboolean
screenshot_gnome_shell_available (void)
{
  proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                         G_DBUS_PROXY_FLAGS_NONE,
                                         NULL,
                                         "org.gnome.Shell.Screenshot",
                                         "/org/gnome/Shell/Screenshot",
                                         "org.gnome.Shell.Screenshot",
                                         NULL, NULL);

  return proxy != NULL;
}

ScreenshotCapabilities
screenshot_gnome_shell_get_capabilities (void)
{
  return (SCREENSHOT_CAN_SHOOT_DECORATIONS |
          SCREENSHOT_CAN_SHOOT_POINTER);
}

GimpPDBStatusType
screenshot_gnome_shell_shoot (ScreenshotValues *shootvals,
                              GdkScreen        *screen,
                              gint32           *image_ID)
{
  gchar       *filename;
  const gchar *method = NULL;
  GVariant    *args   = NULL;
  GVariant    *retval;
  gboolean     success;

  if (shootvals->select_delay > 0)
    screenshot_delay (shootvals->select_delay);

  filename = gimp_temp_name ("png");

  switch (shootvals->shoot_type)
    {
    case SHOOT_ROOT:
      method = "Screenshot";
      args   = g_variant_new ("(bbs)",
                              shootvals->show_cursor,
                              TRUE, /* flash */
                              filename);
      break;

    case SHOOT_REGION:
      retval = g_dbus_proxy_call_sync (proxy, "SelectArea", NULL,
                                       G_DBUS_CALL_FLAGS_NONE,
                                       -1, NULL, NULL);
      g_variant_get (retval, "(iiii)",
                     &shootvals->x1,
                     &shootvals->y1,
                     &shootvals->x2,
                     &shootvals->y2);
      g_variant_unref (retval);

      shootvals->x2 += shootvals->x1;
      shootvals->y2 += shootvals->y1;

      method = "ScreenshotArea";
      args   = g_variant_new ("(iiiibs)",
                              shootvals->x1,
                              shootvals->y1,
                              shootvals->x2 - shootvals->x1,
                              shootvals->y2 - shootvals->y1,
                              TRUE, /* flash */
                              filename);
      break;

    case SHOOT_WINDOW:
      method = "ScreenshotWindow";
      args   = g_variant_new ("(bbbs)",
                              shootvals->decorate,
                              shootvals->show_cursor,
                              TRUE, /* flash */
                              filename);
      break;
    }

  g_free (filename);

  retval = g_dbus_proxy_call_sync (proxy, method, args,
                                   G_DBUS_CALL_FLAGS_NONE,
                                   -1, NULL, NULL);

  g_variant_get (retval, "(bs)",
                 &success,
                 &filename);

  g_variant_unref (retval);

  if (success && filename)
    {
      *image_ID = gimp_file_load (GIMP_RUN_NONINTERACTIVE,
                                  filename, filename);
      gimp_image_set_filename (*image_ID, "screenshot.png");

      g_unlink (filename);
      g_free (filename);

      g_object_unref (proxy);
      proxy = NULL;

      return GIMP_PDB_SUCCESS;
    }

  g_free (filename);

  g_object_unref (proxy);
  proxy = NULL;

  return GIMP_PDB_EXECUTION_ERROR;
}

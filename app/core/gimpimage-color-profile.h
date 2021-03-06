/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpimage-color-profile.h
 * Copyright (C) 2015 Michael Natterer <mitch@gimp.org>
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

#ifndef __GIMP_IMAGE_COLOR_PROFILE_H__
#define __GIMP_IMAGE_COLOR_PROFILE_H__


#define GIMP_ICC_PROFILE_PARASITE_NAME "icc-profile"


gboolean             gimp_image_validate_icc_parasite  (GimpImage           *image,
                                                        const GimpParasite  *icc_parasite,
                                                        gboolean            *is_builtin,
                                                        GError             **error);
const GimpParasite * gimp_image_get_icc_parasite       (GimpImage           *image);
void                 gimp_image_set_icc_parasite       (GimpImage           *image,
                                                        const GimpParasite  *icc_parasite);

gboolean             gimp_image_validate_icc_profile   (GimpImage           *image,
                                                        const guint8        *data,
                                                        gsize                length,
                                                        gboolean            *is_builtin,
                                                        GError             **error);
const guint8       * gimp_image_get_icc_profile        (GimpImage           *image,
                                                        gsize               *length);
gboolean             gimp_image_set_icc_profile        (GimpImage           *image,
                                                        const guint8        *data,
                                                        gsize                length,
                                                        GError             **error);

gboolean             gimp_image_validate_color_profile (GimpImage           *image,
                                                        GimpColorProfile    *profile,
                                                        gboolean            *is_builtin,
                                                        GError             **error);
GimpColorProfile   * gimp_image_get_color_profile      (GimpImage           *image);
gboolean             gimp_image_set_color_profile      (GimpImage           *image,
                                                        GimpColorProfile    *profile,
                                                        GError             **error);

GimpColorProfile   * gimp_image_get_builtin_color_profile
                                                       (GimpImage           *image);

gboolean             gimp_image_convert_color_profile  (GimpImage           *image,
                                                        GimpColorProfile    *dest_profile,
                                                        GimpColorRenderingIntent  intent,
                                                        gboolean             bpc,
                                                        GimpProgress        *progress,
                                                        GError             **error);

void                 gimp_image_import_color_profile   (GimpImage           *image,
                                                        GimpContext         *context,
                                                        GimpProgress        *progress,
                                                        gboolean             interactive);

void                 gimp_image_color_profile_pixel_to_srgb
                                                       (GimpImage           *image,
                                                        const Babl          *pixel_format,
                                                        gpointer             pixel,
                                                        GimpRGB             *color);


/*  internal API, to be called only from gimpimage.c  */

void                 _gimp_image_free_color_profile    (GimpImage           *image);
void                 _gimp_image_update_color_profile  (GimpImage           *image,
                                                        const GimpParasite  *icc_parasite);


#endif /* __GIMP_IMAGE_COLOR_PROFILE_H__ */

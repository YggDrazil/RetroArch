/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2016 - Daniel De Matteis
 *  Copyright (C) 2014-2015 - Jean-André Santoni
 *  Copyright (C) 2016      - Andrés Suárez
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nk_menu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <file/file_path.h>
#include <string/stdstring.h>
#include <lists/string_list.h>
#include <lists/dir_list.h>
#include <playlist.h>

#include "../../menu_driver.h"
#include "../../menu_hash.h"

static float ratio[] = {0.3f, 0.7f};

static bool playlist_icons_loaded;
static bool generic_icons_loaded;
static struct string_list *files  = NULL;
static playlist_t *tmp_playlist = NULL;

struct nk_image playlist_icons[100];

struct generic_icons
{
    struct nk_image disk;
    struct nk_image folder;
    struct nk_image file;
};

struct generic_icons icons;

static void load_icons(nk_menu_handle_t *nk)
{
   char path[PATH_MAX_LENGTH] = {0};
   char buf[PATH_MAX_LENGTH] = {0};

   fill_pathname_application_special(path, sizeof(path),
         APPLICATION_SPECIAL_DIRECTORY_ASSETS_NUKLEAR_ICONS);

   fill_pathname_join(buf, path,
         "disk.png", sizeof(buf));
   icons.disk = nk_common_image_load(buf);
   fill_pathname_join(buf, path,
         "folder.png", sizeof(buf));
   icons.folder = nk_common_image_load(buf);
   fill_pathname_join(buf, path,
         "file.png", sizeof(buf));
   icons.file = nk_common_image_load(buf);
}

static void load_playlist_icons(nk_menu_handle_t *nk, const char* icon, unsigned index)
{
   char path[PATH_MAX_LENGTH] = {0};
   char buf[PATH_MAX_LENGTH] = {0};

   fill_pathname_application_special(path, sizeof(path),
         APPLICATION_SPECIAL_DIRECTORY_ASSETS_NUKLEAR_ICONS);

   fill_pathname_join(buf, path, icon, sizeof(buf));
   snprintf(buf, sizeof(buf), "%s.png",buf);
   playlist_icons[index] = nk_common_image_load(buf);
}

void nk_wnd_library(nk_menu_handle_t *nk, const char* title, unsigned width, unsigned height)
{
   unsigned i;
   static char buf[PATH_MAX_LENGTH];
   static struct nk_panel left_col;
   static struct nk_panel right_col;
   struct nk_panel layout;

   struct nk_context *ctx = &nk->ctx;
   const int id  = NK_WND_LIBRARY;
   settings_t *settings = config_get_ptr();

   if (!generic_icons_loaded)
   {
      load_icons(nk);
      generic_icons_loaded = true;
   }

   if (!files)
      files = dir_list_new(settings->directory.playlist, "lpl", true, true);

   if (nk_begin(ctx, &layout, title, nk_rect(0, 0, width, height),
         NK_WINDOW_NO_SCROLLBAR))
   {
      nk_layout_row(ctx, NK_DYNAMIC, height, 2, ratio);
      nk_group_begin(ctx, &left_col, "Playlists", 0);
      {
         nk_layout_row_dynamic(ctx, 64, 1);

         for (i = 0; i < files->size; i++)
         {
            strlcpy(buf, files->elems[i].data, sizeof(buf));
            path_remove_extension(buf);
            if (!playlist_icons_loaded)
               load_playlist_icons(nk, path_basename(buf), i);
            if (nk_button_image_label(ctx, playlist_icons[i], path_basename(buf),
               NK_TEXT_CENTERED, NK_BUTTON_DEFAULT))
               tmp_playlist = playlist_init(files->elems[i].data, 100);;
         }
         playlist_icons_loaded = true;
         nk_group_end(ctx);
      }
      nk_group_begin(ctx, &right_col, "Content", 0);
      {
         nk_layout_row_dynamic(ctx, 128, 2);
         for (i = 0; i < playlist_size(tmp_playlist); i++)
         {
            const char *entry_path;
            const char *entry_label;
            playlist_get_index(tmp_playlist, i, &entry_path, &entry_label, NULL, NULL, NULL, NULL);
            if (nk_button_image_label(ctx, icons.file, entry_label, NK_TEXT_CENTERED, NK_BUTTON_DEFAULT))
            {
               RARCH_LOG ("do stuff\n");
            }
         }
         nk_group_end(ctx);
      }
   }

   /* save position and size to restore after context reset */
   nk_wnd_set_state(nk, id, nk_window_get_position(ctx), nk_window_get_size(ctx));
   nk_end(ctx);
}

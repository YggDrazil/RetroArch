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

static char* out;
static char core[PATH_MAX_LENGTH] = {0};
static char content[PATH_MAX_LENGTH] = {0};
static float ratio[] = {0.3f, 0.7f};

static struct string_list *files  = NULL;

static struct nk_panel left_col;
static struct nk_panel right_col;
playlist_t *tmp_playlist = NULL;

void nk_wnd_library(nk_menu_handle_t *nk, const char* title, unsigned width, unsigned height)
{
   unsigned i;
   static char buf[PATH_MAX_LENGTH];
   video_shader_ctx_t shader_info;
   struct nk_panel layout;

   struct nk_context *ctx = &nk->ctx;
   const int id  = NK_WND_LIBRARY;
   settings_t *settings = config_get_ptr();

   if (!files)
      files = dir_list_new(settings->directory.playlist, "lpl", true, true);

   if (nk_begin(ctx, &layout, title, nk_rect(0, 0, width, height), 0))
   {
      nk_layout_row(ctx, NK_DYNAMIC, height, 2, ratio);
      if (nk_group_begin(ctx, &left_col, "Playlists", 0))
      {
         nk_layout_row_dynamic(ctx, 30, 1);
         nk_label(ctx,"Playlists", NK_TEXT_LEFT);

         for (i = 0; i < files->size; i++)
         {
            strlcpy(buf, files->elems[i].data, sizeof(buf));
            path_remove_extension(buf);
            if (nk_button_label(ctx, path_basename(buf), NK_BUTTON_DEFAULT))
            {
               RARCH_LOG ("do stuff\n");
               tmp_playlist = playlist_init(files->elems[i].data, 100);;
            }
         }
         nk_group_end(ctx);
      }
      if (nk_group_begin(ctx, &right_col, "Content", 0))
      {
         nk_layout_row_dynamic(ctx, 30, 1);
         nk_label(ctx,"Content", NK_TEXT_LEFT);
         for (i = 0; i < playlist_size(tmp_playlist); i++)
         {
            const char *entry_path;
            const char *entry_label;
            playlist_get_index(tmp_playlist, i, &entry_path, &entry_label, NULL, NULL, NULL, NULL);
            if (nk_button_label(ctx, entry_label, NK_BUTTON_DEFAULT))
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

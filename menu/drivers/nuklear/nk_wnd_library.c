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

#include "../../menu_driver.h"
#include "../../menu_hash.h"

static char* out;
static char core[PATH_MAX_LENGTH] = {0};
static char content[PATH_MAX_LENGTH] = {0};
static float ratio[] = {0.3f, 0.7f};

void nk_wnd_library(nk_menu_handle_t *nk, const char* title, unsigned width, unsigned height)
{
   unsigned i;
   video_shader_ctx_t shader_info;
   struct nk_panel layout;
   struct nk_panel left_col;
   struct nk_panel right_col;
   struct nk_context *ctx = &nk->ctx;
   const int id           = NK_WND_MAIN;
   settings_t *settings  = config_get_ptr();

   static char picker_filter[PATH_MAX_LENGTH];
   static char picker_title[PATH_MAX_LENGTH];
   static char* picker_startup_dir;

   int len_core, len_content = 0;

   if (!out)
      out = &core;

   if (!string_is_empty(core))
      len_core = strlen(path_basename(core));
   if (!string_is_empty(content))
      len_content = strlen(content);

   if (nk->window[NK_WND_FILE_PICKER].open)
   {
      if (nk_wnd_file_picker(nk, picker_title, picker_startup_dir, out, picker_filter))
      {
         RARCH_LOG ("%s selected\n", out);
         nk_window_close(&nk->ctx, picker_title);
      }
   }

   if (nk_begin(ctx, &layout, title, nk_rect(0, 0, width, height), 0))
   {
      nk_layout_row(ctx, NK_DYNAMIC, 300, 2, ratio);
      if (nk_group_begin(ctx, &left_col, "", 0))
      {
         nk_layout_row_dynamic(ctx, 30, 1);
         nk_label(ctx,"Playlists", NK_TEXT_LEFT);
         unsigned i = 0;
         for (i = 0; i < 30; i++)
            nk_button_label(ctx, "Super Nintendo", NK_BUTTON_DEFAULT);
         nk_group_end(ctx);
      }
      if (nk_group_begin(ctx, &right_col, "", 0))
      {
         nk_layout_row_dynamic(ctx, 30, 1);
         nk_label(ctx,"Content", NK_TEXT_LEFT);
         nk_group_end(ctx);
      }
   }

   /* save position and size to restore after context reset */
   nk_wnd_set_state(nk, id, nk_window_get_position(ctx), nk_window_get_size(ctx));
   nk_end(ctx);
}

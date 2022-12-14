/*
 *  Copyright (c) 2009-2018 Giuseppe Torelli <colossus73@gmail.com>
 *  Copyright (c) 2009 Tadej Borovšak 	<tadeboro@gmail.com>
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License,or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not,write to the Free Software
 *  Foundation,Inc.,59 Temple Place - Suite 330,Boston,MA 02111-1307,USA.
 *
 */

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <gtk/gtk.h>
#include "audio.h"
#include "main-window.h"
#include "new_slideshow.h"
#include "slideshow_project.h"
#include "subtitles.h"

gboolean img_can_discard_unsaved_project(img_window_struct *img);
void img_taint_project(img_window_struct *img);
void img_refresh_window_title(img_window_struct *);
void img_new_slideshow(GtkMenuItem *,img_window_struct *);
void img_project_properties(GtkMenuItem *, img_window_struct *);
void img_add_slides(GSList *slides, img_window_struct *img);
void img_add_slides_thumbnails(GtkMenuItem *,img_window_struct *);
void img_remove_audio_files (GtkWidget *, img_window_struct *);
void img_remove_foreach_func (GtkTreeModel *, GtkTreePath *, GtkTreeIter *, GList **);
void img_delete_selected_slides(GtkMenuItem *,img_window_struct *);
void
img_rotate_slides_left( GtkWidget         *widget,
						img_window_struct *img );

void
img_rotate_slides_right( GtkWidget         *widget,
						 img_window_struct *img );
void img_show_about_dialog (GtkMenuItem *,img_window_struct *);
void img_start_stop_preview(GtkWidget *, img_window_struct *);
void img_go_fullscreen(GtkMenuItem *, img_window_struct *);
void img_goto_first_slide(GtkWidget *, img_window_struct *);
void img_goto_prev_slide(GtkWidget *, img_window_struct *);
void img_goto_next_slide(GtkWidget *, img_window_struct *);
void img_goto_last_slide(GtkWidget *, img_window_struct *);
void img_on_drag_data_received (GtkWidget *,GdkDragContext *, int, int, GtkSelectionData *, unsigned int, unsigned int, img_window_struct *);
void img_start_stop_export(GtkWidget *, img_window_struct *);
void img_choose_slideshow_filename(GtkWidget *,img_window_struct *);
void img_close_slideshow(GtkWidget *,img_window_struct *);
void img_select_audio_files_to_add ( GtkMenuItem* , img_window_struct *);
void img_add_audio_files (gchar *, img_window_struct *);
void img_increase_progressbar(img_window_struct *, gint);
GSList *img_import_slides_file_chooser(img_window_struct *);
void img_free_allocated_memory(img_window_struct *);
gint img_ask_user_confirmation(img_window_struct *, gchar *);
gboolean img_key_pressed(GtkWidget *, GdkEventKey *, img_window_struct *);
void img_exit_fullscreen(img_window_struct *img);
gboolean img_quit_application(GtkWidget *, GdkEvent *, img_window_struct *);
void img_move_audio_up( GtkButton *, img_window_struct * );
void img_move_audio_down( GtkButton *, img_window_struct * );
gboolean img_on_draw_event(GtkWidget *,cairo_t *,img_window_struct *);
void img_ken_burns_zoom_changed( GtkRange *, img_window_struct * );
gboolean img_image_area_scroll( GtkWidget *, GdkEvent *, img_window_struct * );
gboolean img_image_area_button_press( GtkWidget *, GdkEventButton *, img_window_struct * );
gboolean img_image_area_button_release( GtkWidget *, GdkEventButton *, img_window_struct * );
gboolean img_image_area_motion( GtkWidget *, GdkEventMotion *, img_window_struct * );
void img_clipboard_cut_copy_operation(img_window_struct *, ImgClipboardMode );
void img_clipboard_clear (GtkClipboard *, img_window_struct *);
void img_clipboard_get (GtkClipboard *, GtkSelectionData *, guint , img_window_struct *);
void img_text_pos_changed( GtkRange *range, img_window_struct *);
void img_zoom_in ( GtkWidget         *item, img_window_struct *img );
void img_zoom_out( GtkWidget        *item, img_window_struct *img );
void img_zoom_reset( GtkWidget      *item, img_window_struct *img );

void
img_zoom_fit( GtkWidget         *item,
              img_window_struct *img );

void
img_quality_toggled( GtkCheckMenuItem  *item,
					 img_window_struct *img );
void
img_draw_image_on_surface( cairo_t           *cr,
						   gint               width,
						   cairo_surface_t   *surface,
						   ImgStopPoint      *point,
						   img_window_struct *img );
void
img_add_stop_point( GtkButton         *button,
					img_window_struct *img );
void
img_update_stop_point( GtkSpinButton         *button,
					   img_window_struct *img );
void
img_delete_stop_point( GtkButton         *button,
					   img_window_struct *img );
void
img_update_stop_display( img_window_struct *img,
						 gboolean           update_pos );
						 
void
img_update_subtitles_widgets( img_window_struct * );

void
img_goto_prev_point( GtkButton         *button,
					 img_window_struct *img );
void
img_goto_next_point( GtkButton         *button,
					 img_window_struct *img );
void
img_goto_point ( GtkEntry          *entry,
				 img_window_struct *img );
void
img_calc_current_ken_point( ImgStopPoint *res,
							ImgStopPoint *from,
							ImgStopPoint *to,
							gdouble       progress,
							gint          mode );

void
img_add_empty_slide( GtkMenuItem       *item,
					 img_window_struct *img );

gboolean
img_save_window_settings( img_window_struct *img );

gboolean
img_load_window_settings( img_window_struct *img );

void
img_set_window_default_settings( img_window_struct *img );

void
img_rotate_flip_slide( slide_struct   *slide,
				  ImgAngle        angle,
				  gboolean        flipped,
				  GtkProgressBar *progress );

void
img_notebook_switch_page (GtkNotebook       *notebook,
                          gpointer            opaque,
                          guint              page_num,
                          img_window_struct *img);

void img_align_text_horizontally_vertically(GtkMenuItem *item,
					img_window_struct *img);

void
img_pattern_clicked(GtkMenuItem *item,
					img_window_struct *img);

void img_subtitle_top_border_toggled (GtkToggleButton *button, img_window_struct *img);
void img_subtitle_bottom_border_toggled (GtkToggleButton *button, img_window_struct *img);
void img_spinbutton_value_changed (GtkSpinButton *spinbutton, img_window_struct *img);
void img_fadeout_duration_changed (GtkSpinButton *spinbutton, img_window_struct *img);
void img_subtitle_style_changed(GtkButton *button, img_window_struct *img);
void img_set_slide_text_align(GtkButton *button, img_window_struct *img);
void img_flip_horizontally(GtkMenuItem *item, img_window_struct *img);
#endif

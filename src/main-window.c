/*
 *  Copyright (c) 2009-2019 Giuseppe Torelli <colossus73@gmail.com>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <assert.h>

#include "main-window.h"
#include "callbacks.h"
#include "export.h"
#include "subtitles.h"
#include "imgcellrendererpixbuf.h"
#include "video_formats.h"

static const GtkTargetEntry drop_targets[] =
{
	{ "text/uri-list",0,0 },
};

/* ****************************************************************************
 * Local function declarations
 * ************************************************************************* */
static void img_random_button_clicked(GtkButton *, img_window_struct *);
static GdkPixbuf *img_set_random_transition(img_window_struct *, slide_struct *);
static void img_combo_box_speed_changed (GtkComboBox *,  img_window_struct *);
static void img_slide_cut(GtkMenuItem * , img_window_struct *);
static void img_slide_copy(GtkMenuItem * , img_window_struct *);
static void img_slide_paste(GtkMenuItem* , img_window_struct *);
static void img_report_slides_transitions(img_window_struct *);
static void img_clear_audio_files(GtkButton *, img_window_struct *);
static void img_on_drag_audio_data_received (GtkWidget *,GdkDragContext *, int, int, GtkSelectionData *, unsigned int, unsigned int, img_window_struct *);
static gboolean img_sub_textview_focus_in (GtkWidget *, GdkEventFocus *, img_window_struct *);
static gboolean img_sub_textview_focus_out(GtkWidget *, GdkEventFocus *, img_window_struct *);
static void img_activate_remove_button_music_liststore(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, img_window_struct *);
static void img_quit_menu(GtkMenuItem *, img_window_struct *);
static void img_select_all_thumbnails(GtkMenuItem *, img_window_struct *);
static void img_unselect_all_thumbnails(GtkMenuItem *, img_window_struct *);
static void img_goto_line_entry_activate(GtkWidget *, img_window_struct *);
static gint img_sort_none_before_other(GtkTreeModel *, GtkTreeIter *, GtkTreeIter *, gpointer);
static void img_check_numeric_entry (GtkEditable *entry, gchar *text, gint lenght, gint *position, gpointer data);
static void img_show_uri(GtkMenuItem *, img_window_struct *);
static void img_select_slide_from_slide_report_dialog(GtkButton *, img_window_struct *);
static void img_show_slides_report_dialog(GtkMenuItem *, img_window_struct *);

static gboolean
img_iconview_selection_button_press( GtkWidget         *widget,
									 GdkEventButton    *button,
									 img_window_struct *img );

static gboolean
img_scroll_thumb( GtkWidget         *widget,
				  GdkEventScroll    *scroll,
				  img_window_struct *img );

static gboolean
img_subtitle_update( img_window_struct *img );

static GtkWidget *
img_create_subtitle_animation_combo( void );

static void
img_toggle_mode( GtkCheckMenuItem  *item,
				 img_window_struct *img );

static gint
img_sort_report_transitions( gconstpointer a,
							 gconstpointer b );

static void
img_toggle_frame_rate( GtkCheckMenuItem  *item,
					   img_window_struct *img );

static GtkWidget *
img_load_icon_from_theme(GtkIconTheme *icon_theme, gchar *name, gint size);

/* ****************************************************************************
 * Function definitions
 * ************************************************************************* */
img_window_struct *img_create_window (void)
{
	img_window_struct *img_struct = NULL;
	GtkWidget *vbox1;
	GtkWidget *menuitem1;
	GtkWidget *menuitem2;
	GtkWidget *menu1;
	GtkWidget *imagemenuitem1;
	GtkWidget *imagemenuitem5;
	GtkWidget *separatormenuitem1;
	GtkWidget *slide_menu;
	GtkWidget *deselect_all_menu;
	GtkWidget *menuitem3;
	GtkWidget *tmp_image;
	GtkWidget *menu3;
	GtkWidget *about;
	GtkWidget *contents;
	GtkToolItem *new_button;
	GtkWidget *add_slide;
	GtkWidget *separatortoolitem;
	GtkWidget *toolbutton_slide_goto, *label_of;
	GtkToolItem *first_slide, *last_slide, *prev_slide, *next_slide;
	GtkWidget *video_tab;
	GtkWidget *audio_tab;
	GtkWidget *swindow, *scrollable_window;
	GtkWidget *viewport;
	GtkWidget *align;
	GtkWidget *vbox_frames, *vbox_audio_frames;
	GtkWidget *frame1, *frame2, *frame3, *frame4, *frame_label;
	GtkWidget *transition_label;
	GtkWidget *vbox_info_slide, *vbox_slide_motion, *vbox_slide_caption, *vbox_slide_position;
	GtkWidget *table;
	GtkWidget *duration_label;
	GtkWidget *trans_duration_label;
	GtkWidget *total_time;
	GtkWidget *hbox_stop_points, *stop_points_label;
	GtkWidget *hbox_time_offset, *time_offset_label;
	GtkWidget *hbox_textview, *text_animation_hbox;
	GtkWidget *hbox_music_label;
	GtkWidget *hbox_fadeout;
	GtkWidget *fadeout_duration;
	GtkWidget *music_time;
	GtkWidget *hbox_buttons, *move_up_button;
	GtkWidget *move_down_button, *clear_button, *image_buttons, *vbox2, *scrolledwindow1;
    GtkWidget *message_scroll, *message_view;
	GtkCellRenderer *renderer, *pixbuf_cell;
	//GtkTreeSelection *selection;
	GtkTreeViewColumn *column;
	GtkIconTheme *icon_theme;
	GdkPixbuf *icon;
	gint x;
	GtkWidget *eventbox;
	GtkWidget *a_label;
	GtkWidget *a_hbox;
	GtkWidget *modes_vbox;
	GtkWidget *properties_menu;
	GtkWidget *preview_menu;
	GtkWidget *import_menu;
	GtkWidget *import_audio_menu;
	GtkWidget *import_button;
	GtkWidget *import_audio_button;
	GtkToolItem *remove_button;
	GtkWidget *flip_button;
	GtkToolItem *zoom_in_button, *zoom_out_button, *zoom_normal, *zoom_fit;
	GtkWidget *export_menu;
	GdkPixbuf *pixbuf;
	GtkWidget *flip_horizontally_menu;
	GtkWidget *rotate_left_menu;
	GtkWidget *rotate_right_menu;
	GtkWidget *rotate_left_button;
	GtkWidget *rotate_right_button;
	AtkObject *atk;
	GtkWidget *tmp_checks[PREVIEW_FPS_NO_PRESETS];

	img_struct = g_new0(img_window_struct, 1);

	/* Set some default values */
	img_struct->background_color[0] = 0;
	img_struct->background_color[1] = 0;
	img_struct->background_color[2] = 0;
	img_struct->slides_nr = 0;
	img_struct->distort_images = TRUE;

	img_struct->maxoffx = 0;
	img_struct->maxoffy = 0;
	img_struct->current_point.offx = 0;
	img_struct->current_point.offy = 0;
	img_struct->current_point.zoom = 1;

	img_struct->video_size[0] = 720;
	img_struct->video_size[1] = 576;
	img_struct->video_ratio = (gdouble)720 / 576;
	img_struct->video_format_index = 0; /* VOB is default */
    img_struct->aspect_ratio_index = 0;
    img_struct->bitrate_index = 0;
    img_struct->fps_index = 0;
    img_struct->export_fps = video_format_list[img_struct->video_format_index].fps_list[img_struct->fps_index].numerator;
    img_struct->export_fps /= video_format_list[img_struct->video_format_index].fps_list[img_struct->fps_index].denominator;

    img_struct->audio_fadeout = 5;

	img_struct->final_transition.duration = 0;
	img_struct->final_transition.render = NULL;
	img_struct->final_transition.speed = NORMAL;


	/* GUI STUFF */
	icon_theme = gtk_icon_theme_get_default();
	icon = gtk_icon_theme_load_icon(icon_theme, "imagination", 24, 0, NULL);

	img_struct->imagination_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_icon (GTK_WINDOW(img_struct->imagination_window),icon);
	gtk_window_set_position (GTK_WINDOW(img_struct->imagination_window),GTK_WIN_POS_CENTER);
	img_refresh_window_title(img_struct);
	g_signal_connect (G_OBJECT (img_struct->imagination_window),"delete-event",G_CALLBACK (img_quit_application),img_struct);
	g_signal_connect (G_OBJECT (img_struct->imagination_window), "destroy", G_CALLBACK (gtk_main_quit), NULL );
	g_signal_connect (G_OBJECT (img_struct->imagination_window), "key_press_event", G_CALLBACK(img_key_pressed), img_struct);
	img_struct->accel_group = gtk_accel_group_new();

	vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (vbox1);
	gtk_container_add (GTK_CONTAINER (img_struct->imagination_window), vbox1);

	/* Create the menu items */
	img_struct->menubar = gtk_menu_bar_new ();
	gtk_box_pack_start (GTK_BOX (vbox1), img_struct->menubar, FALSE, TRUE, 0);

	menuitem1 = gtk_menu_item_new_with_mnemonic (_("_Slideshow"));
	gtk_container_add (GTK_CONTAINER (img_struct->menubar), menuitem1);

	menu1 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem1), menu1);

	imagemenuitem1 = gtk_menu_item_new_with_mnemonic(_("_New"));
	gtk_container_add (GTK_CONTAINER (menu1), imagemenuitem1);
	gtk_widget_add_accelerator (imagemenuitem1,"activate", img_struct->accel_group,GDK_KEY_n,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (imagemenuitem1),"activate",G_CALLBACK (img_new_slideshow),img_struct);

	img_struct->open_menu = gtk_menu_item_new_with_mnemonic (_("_Open"));
	gtk_container_add (GTK_CONTAINER (menu1), img_struct->open_menu);
	gtk_widget_add_accelerator (img_struct->open_menu,"activate", img_struct->accel_group,GDK_KEY_o,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->open_menu),"activate",G_CALLBACK (img_choose_slideshow_filename),img_struct);

    img_struct->import_project_menu = gtk_menu_item_new_with_mnemonic (_("Import slideshow"));
    gtk_container_add (GTK_CONTAINER (menu1),img_struct->import_project_menu);
    g_signal_connect (G_OBJECT (img_struct->import_project_menu),"activate",G_CALLBACK (img_choose_slideshow_filename),img_struct);

	img_struct->save_menu = gtk_menu_item_new_with_mnemonic (_("_Save"));
	gtk_container_add (GTK_CONTAINER (menu1), img_struct->save_menu);
	gtk_widget_add_accelerator (img_struct->save_menu,"activate", img_struct->accel_group,GDK_KEY_s,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->save_menu),"activate",G_CALLBACK (img_choose_slideshow_filename),img_struct);

	img_struct->save_as_menu = gtk_menu_item_new_with_mnemonic (_("Save _As"));
	gtk_container_add (GTK_CONTAINER (menu1), img_struct->save_as_menu);
	g_signal_connect (G_OBJECT (img_struct->save_as_menu),"activate",G_CALLBACK (img_choose_slideshow_filename),img_struct);

	img_struct->close_menu = gtk_menu_item_new_with_mnemonic (_("_Close"));
	gtk_container_add (GTK_CONTAINER (menu1), img_struct->close_menu);
	gtk_widget_add_accelerator (img_struct->close_menu,"activate", img_struct->accel_group,GDK_KEY_w,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->close_menu),"activate",G_CALLBACK (img_close_slideshow),img_struct);

	separatormenuitem1 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menu1), separatormenuitem1);

	import_menu = gtk_menu_item_new_with_mnemonic (_("Import p_ictures"));
	gtk_container_add (GTK_CONTAINER (menu1),import_menu);
	gtk_widget_add_accelerator (import_menu,"activate",img_struct->accel_group,GDK_KEY_i,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (import_menu),"activate",G_CALLBACK (img_add_slides_thumbnails),img_struct);
	
	import_audio_menu = gtk_menu_item_new_with_mnemonic (_("Import _music"));
	gtk_container_add (GTK_CONTAINER (menu1),import_audio_menu);
	gtk_widget_add_accelerator (import_audio_menu,"activate",img_struct->accel_group,GDK_KEY_m,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (import_audio_menu),"activate",G_CALLBACK (img_select_audio_files_to_add),img_struct);

	properties_menu = gtk_menu_item_new_with_mnemonic (_("_Properties"));
	gtk_container_add (GTK_CONTAINER (menu1), properties_menu);
	gtk_widget_add_accelerator (properties_menu,"activate",img_struct->accel_group,GDK_KEY_p,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (properties_menu), "activate", G_CALLBACK (img_project_properties), img_struct);

	separatormenuitem1 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menu1), separatormenuitem1);

	preview_menu = gtk_menu_item_new_with_mnemonic (_("Preview"));
	gtk_menu_shell_append( GTK_MENU_SHELL( menu1 ), preview_menu);
	
	menu3 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (preview_menu), menu3);

	img_struct->fullscreen_music_preview = gtk_menu_item_new_with_mnemonic(_("With music") );
	gtk_widget_add_accelerator (img_struct->fullscreen_music_preview, "activate",img_struct->accel_group, GDK_KEY_space, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->fullscreen_music_preview),"activate",G_CALLBACK (img_start_stop_preview),img_struct);
	gtk_menu_shell_append( GTK_MENU_SHELL( menu3 ), img_struct->fullscreen_music_preview );
	
	img_struct->fullscreen_no_music = gtk_menu_item_new_with_mnemonic(_("Without music") );
	gtk_widget_add_accelerator (img_struct->fullscreen_no_music, "activate",img_struct->accel_group, GDK_KEY_m, GDK_SHIFT_MASK|GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->fullscreen_no_music),"activate",G_CALLBACK (img_start_stop_preview), img_struct);
	gtk_menu_shell_append( GTK_MENU_SHELL( menu3 ), img_struct->fullscreen_no_music );
	
	img_struct->fullscreen_loop_preview = gtk_menu_item_new_with_mnemonic(_("Continuos") );
	gtk_widget_add_accelerator (img_struct->fullscreen_loop_preview, "activate",img_struct->accel_group,GDK_KEY_p,GDK_SHIFT_MASK|GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->fullscreen_loop_preview),"activate",G_CALLBACK (img_start_stop_preview), img_struct);
	gtk_menu_shell_append( GTK_MENU_SHELL( menu3 ), img_struct->fullscreen_loop_preview );

	export_menu = gtk_menu_item_new_with_mnemonic (_("Ex_port"));
	gtk_container_add (GTK_CONTAINER (menu1), export_menu);
	gtk_widget_add_accelerator (export_menu, "activate",img_struct->accel_group,GDK_KEY_p,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (export_menu),"activate",G_CALLBACK (img_exporter),img_struct);

	separatormenuitem1 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menu1), separatormenuitem1);

	imagemenuitem5 = gtk_menu_item_new_with_mnemonic (_("_Quit"));
	gtk_container_add (GTK_CONTAINER (menu1), imagemenuitem5);
	g_signal_connect (G_OBJECT (imagemenuitem5),"activate",G_CALLBACK (img_quit_menu),img_struct);

	/* Slide menu */
	menuitem2 = gtk_menu_item_new_with_mnemonic (_("_Slide"));
	gtk_container_add (GTK_CONTAINER (img_struct->menubar), menuitem2);

	slide_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem2), slide_menu);

	img_struct->cut = gtk_menu_item_new_with_mnemonic (_("Cu_t"));
	gtk_container_add (GTK_CONTAINER (slide_menu), img_struct->cut);
	gtk_widget_add_accelerator (img_struct->cut, "activate",img_struct->accel_group,GDK_KEY_x,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->cut), "activate", G_CALLBACK (img_slide_cut), img_struct);

	img_struct->copy = gtk_menu_item_new_with_mnemonic (_("Copy"));
	gtk_container_add (GTK_CONTAINER (slide_menu), img_struct->copy);
	gtk_widget_add_accelerator (img_struct->copy, "activate",img_struct->accel_group,GDK_KEY_c,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->copy), "activate", G_CALLBACK (img_slide_copy), img_struct);

	img_struct->paste = gtk_menu_item_new_with_mnemonic (_("Paste"));
	gtk_container_add (GTK_CONTAINER (slide_menu), img_struct->paste);
	gtk_widget_add_accelerator (img_struct->paste, "activate",img_struct->accel_group,GDK_KEY_v,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->paste), "activate", G_CALLBACK (img_slide_paste), img_struct);

	/* Zoom controls */
	menuitem1 = gtk_menu_item_new_with_mnemonic( _("_Zoom") );
	gtk_menu_shell_append( GTK_MENU_SHELL( slide_menu ), menuitem1 );

	menu3 = gtk_menu_new();
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( menuitem1 ), menu3 );

	menuitem2 = gtk_menu_item_new_with_mnemonic(_("Zoom In"));
	gtk_widget_add_accelerator( menuitem2, "activate", img_struct->accel_group, GDK_KEY_plus,GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE );
	g_signal_connect( G_OBJECT( menuitem2 ), "activate", G_CALLBACK( img_zoom_in ), img_struct );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu3 ), menuitem2 );

	menuitem2 = gtk_menu_item_new_with_mnemonic(_("Zoom Out"));
	gtk_widget_add_accelerator( menuitem2, "activate", img_struct->accel_group, GDK_KEY_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE );
	g_signal_connect( G_OBJECT( menuitem2 ), "activate", G_CALLBACK( img_zoom_out ), img_struct );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu3 ), menuitem2 );

	menuitem2 = gtk_menu_item_new_with_mnemonic(_("Normal Size"));
	gtk_widget_add_accelerator( menuitem2, "activate", img_struct->accel_group, GDK_KEY_0,GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE );
	g_signal_connect( G_OBJECT( menuitem2 ), "activate",
					  G_CALLBACK( img_zoom_reset ), img_struct );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu3 ), menuitem2 );

	add_slide = gtk_menu_item_new_with_mnemonic (_("Add empt_y slide"));
	gtk_container_add (GTK_CONTAINER (slide_menu), add_slide);
	gtk_widget_add_accelerator( add_slide, "activate", img_struct->accel_group,	GDK_KEY_y, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE );
	g_signal_connect( G_OBJECT( add_slide ), "activate", G_CALLBACK( img_add_empty_slide ), img_struct );

	img_struct->edit_empty_slide = gtk_menu_item_new_with_mnemonic (_("Edit _empty slide"));
	gtk_container_add (GTK_CONTAINER (slide_menu), img_struct->edit_empty_slide);
	gtk_widget_set_sensitive(img_struct->edit_empty_slide, FALSE);
	gtk_widget_add_accelerator( img_struct->edit_empty_slide, "activate", img_struct->accel_group,	GDK_KEY_e, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE );
	g_signal_connect( G_OBJECT( img_struct->edit_empty_slide ), "activate", G_CALLBACK( img_add_empty_slide ), img_struct );

	img_struct->remove_menu = gtk_menu_item_new_with_mnemonic (_("Dele_te"));
	gtk_container_add (GTK_CONTAINER (slide_menu), img_struct->remove_menu);
	gtk_widget_add_accelerator (img_struct->remove_menu,"activate",img_struct->accel_group, GDK_KEY_Delete,0,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->remove_menu),"activate",G_CALLBACK (img_delete_selected_slides),img_struct);

	img_struct->report_menu = gtk_menu_item_new_with_mnemonic (_("Repor_t"));
	gtk_container_add (GTK_CONTAINER (slide_menu), img_struct->report_menu);
	gtk_widget_add_accelerator( img_struct->report_menu, "activate", img_struct->accel_group, GDK_KEY_t, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE );
	g_signal_connect (G_OBJECT (img_struct->report_menu),"activate",G_CALLBACK (img_show_slides_report_dialog),img_struct);

	flip_horizontally_menu = gtk_menu_item_new_with_mnemonic (_("_Flip horizontally"));
	gtk_container_add (GTK_CONTAINER (slide_menu),flip_horizontally_menu);
	gtk_widget_add_accelerator (flip_horizontally_menu,"activate",img_struct->accel_group, GDK_KEY_f,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect( G_OBJECT( flip_horizontally_menu ), "activate", G_CALLBACK( img_flip_horizontally), img_struct );

	rotate_left_menu = gtk_menu_item_new_with_mnemonic (_("Rotate co_unter-clockwise"));
	gtk_container_add (GTK_CONTAINER (slide_menu),rotate_left_menu);
	gtk_widget_add_accelerator (rotate_left_menu,"activate",img_struct->accel_group, GDK_KEY_u,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect( G_OBJECT( rotate_left_menu ), "activate", G_CALLBACK( img_rotate_slides_left), img_struct );

	rotate_right_menu = gtk_menu_item_new_with_mnemonic (_("_Rotate clockwise"));
	gtk_container_add (GTK_CONTAINER (slide_menu),rotate_right_menu);
	gtk_widget_add_accelerator (rotate_right_menu,"activate",img_struct->accel_group, GDK_KEY_r,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect( G_OBJECT( rotate_right_menu ), "activate", G_CALLBACK ( img_rotate_slides_right ), img_struct );

	img_struct->select_all_menu = gtk_menu_item_new_with_mnemonic(_("Select All"));
	gtk_container_add (GTK_CONTAINER (slide_menu),img_struct->select_all_menu);
	gtk_widget_add_accelerator (img_struct->select_all_menu,"activate",img_struct->accel_group,GDK_KEY_a,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->select_all_menu),"activate",G_CALLBACK (img_select_all_thumbnails),img_struct);

	deselect_all_menu = gtk_menu_item_new_with_mnemonic (_("Un_select all"));
	gtk_container_add (GTK_CONTAINER (slide_menu),deselect_all_menu);
	gtk_widget_add_accelerator (deselect_all_menu,"activate",img_struct->accel_group,GDK_KEY_a,GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (deselect_all_menu),"activate",G_CALLBACK (img_unselect_all_thumbnails),img_struct);

	/* View menu */
	menuitem1 = gtk_menu_item_new_with_mnemonic (_("_View"));
	gtk_container_add (GTK_CONTAINER (img_struct->menubar), menuitem1);

	menu1 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem1), menu1);

	img_struct->menu_preview_mode = gtk_radio_menu_item_new_with_label( NULL, _("Preview mode") );
	g_signal_connect( G_OBJECT( img_struct->menu_preview_mode ), "toggled", G_CALLBACK( img_toggle_mode ), img_struct );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu1 ), img_struct->menu_preview_mode );

	img_struct->menu_overview_mode = gtk_radio_menu_item_new_with_label_from_widget( 
							GTK_RADIO_MENU_ITEM( img_struct->menu_preview_mode ), _("Overview mode") );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu1 ), img_struct->menu_overview_mode );

	menuitem1 = gtk_menu_item_new_with_mnemonic( _("Preview _frame rate") );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu1 ), menuitem1 );

	img_struct->fullscreen = gtk_menu_item_new_with_mnemonic (_("Fullscreen"));
	gtk_container_add (GTK_CONTAINER (menu1),img_struct->fullscreen);
	gtk_widget_add_accelerator (img_struct->fullscreen, "activate", img_struct->accel_group,GDK_KEY_F11,0,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (img_struct->fullscreen),"activate", G_CALLBACK (img_go_fullscreen), img_struct);

	menu1 = gtk_menu_new();
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( menuitem1 ), menu1 );

	{
		gint       i,
				   j;
		GtkWidget *item;
		gchar     *label;
		GSList    *group = NULL;

		for( i = 0, j = PREVIEW_FPS_MIN;
			 i < PREVIEW_FPS_NO_PRESETS;
			 i++, j += PREVIEW_FPS_STEP )
		{
			label = g_strdup_printf( ngettext( "%d frame per second",
											   "%d frames per second",
											   j ),
									 j );
			item = gtk_radio_menu_item_new_with_label( group, label );
			tmp_checks[i] = item;
			g_signal_connect( G_OBJECT( item ), "toggled",
							  G_CALLBACK( img_toggle_frame_rate ), img_struct );
			g_object_set_data( G_OBJECT( item ), "index",
							   GINT_TO_POINTER( j ) );
			gtk_menu_shell_append( GTK_MENU_SHELL( menu1 ), item );
			g_free( label );
			group = gtk_radio_menu_item_get_group( GTK_RADIO_MENU_ITEM( item ) );
		}
	}

	/* Help menu */
	menuitem3 = gtk_menu_item_new_with_mnemonic (_("_Help"));
	gtk_container_add (GTK_CONTAINER (img_struct->menubar), menuitem3);
	menu3 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem3), menu3);

	contents = gtk_menu_item_new_with_mnemonic (_("Contents"));
	gtk_container_add (GTK_CONTAINER (menu3),contents);
	gtk_widget_add_accelerator (contents,"activate",img_struct->accel_group,GDK_KEY_F1,0,GTK_ACCEL_VISIBLE);
	g_signal_connect (G_OBJECT (contents),"activate",G_CALLBACK (img_show_uri),img_struct);

	about = gtk_menu_item_new_with_mnemonic (_("About"));
	gtk_container_add (GTK_CONTAINER (menu3), about);
	gtk_widget_show_all (img_struct->menubar);
	g_signal_connect (G_OBJECT (about),"activate",G_CALLBACK (img_show_about_dialog),img_struct);

	/* Create the toolbar */
	img_struct->toolbar = gtk_toolbar_new ();
	gtk_box_pack_start (GTK_BOX (vbox1), img_struct->toolbar, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (img_struct->toolbar), GTK_TOOLBAR_ICONS);

	image_buttons = gtk_image_new_from_icon_name ("document-new", GTK_ICON_SIZE_BUTTON);
	new_button = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), new_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(new_button), _("Create a new slideshow"));
	g_signal_connect (G_OBJECT (new_button),"clicked",G_CALLBACK (img_new_slideshow),img_struct);

	image_buttons = gtk_image_new_from_icon_name ("document-open", GTK_ICON_SIZE_BUTTON);
	img_struct->open_button = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), img_struct->open_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(img_struct->open_button), _("Open a slideshow"));
	g_signal_connect (G_OBJECT (img_struct->open_button),"clicked",G_CALLBACK (img_choose_slideshow_filename),img_struct);

	image_buttons = gtk_image_new_from_icon_name ("document-save", GTK_ICON_SIZE_BUTTON);
	img_struct->save_button =  gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), img_struct->save_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(img_struct->save_button), _("Save the slideshow"));
	g_signal_connect (G_OBJECT (img_struct->save_button),"clicked",G_CALLBACK (img_choose_slideshow_filename),img_struct);

	separatortoolitem = GTK_WIDGET (gtk_separator_tool_item_new());
	gtk_widget_show (separatortoolitem);
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),separatortoolitem);

	pixbuf = gtk_icon_theme_load_icon(icon_theme,"image", 20, 0, NULL);
	tmp_image = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);

	import_button = GTK_WIDGET (gtk_tool_button_new (tmp_image,""));
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),import_button);
	gtk_widget_set_tooltip_text(import_button, _("Import pictures"));
	g_signal_connect ((gpointer)import_button, "clicked", G_CALLBACK (img_add_slides_thumbnails),img_struct);

	pixbuf = gtk_icon_theme_load_icon(icon_theme,"sound", 20, 0, NULL);
	tmp_image = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);

	import_audio_button = GTK_WIDGET (gtk_tool_button_new (tmp_image,""));
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),import_audio_button);
	gtk_widget_set_tooltip_text(import_audio_button, _("Import music"));
	g_signal_connect(G_OBJECT(import_audio_button), "clicked", G_CALLBACK(img_select_audio_files_to_add), img_struct);

	image_buttons = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_BUTTON); 
	remove_button = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), remove_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(remove_button), _("Delete the selected slides"));
	g_signal_connect (G_OBJECT (remove_button),"clicked",G_CALLBACK (img_delete_selected_slides),img_struct);

	flip_button = GTK_WIDGET (gtk_tool_button_new(
		    img_load_icon_from_theme(icon_theme, "object-flip-horizontal", 22) ,""));
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),flip_button);
	gtk_widget_set_tooltip_text(flip_button, _("Flip horizontally the selected slides"));
	g_signal_connect (G_OBJECT (flip_button),"clicked",G_CALLBACK (img_flip_horizontally),img_struct);

	rotate_left_button = GTK_WIDGET (gtk_tool_button_new(
		    img_load_icon_from_theme(icon_theme, "object-rotate-left", 22) ,""));
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar), rotate_left_button);
	gtk_widget_set_tooltip_text( rotate_left_button,
								 _("Rotate the slide 90 degrees to the left") );
	g_signal_connect( G_OBJECT( rotate_left_button ), "clicked",
					  G_CALLBACK (img_rotate_slides_left ), img_struct );
	
	rotate_right_button = GTK_WIDGET (gtk_tool_button_new(
		    img_load_icon_from_theme(icon_theme, "object-rotate-right", 22) ,""));
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),rotate_right_button);
	gtk_widget_set_tooltip_text( rotate_right_button,
								 _("Rotate the slide 90 degrees to the right") );
	g_signal_connect( G_OBJECT( rotate_right_button ), "clicked",
					  G_CALLBACK( img_rotate_slides_right ), img_struct );


	image_buttons = gtk_image_new_from_icon_name ("zoom-in", GTK_ICON_SIZE_BUTTON);
	zoom_in_button = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), zoom_in_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(zoom_in_button), _("Zoom In"));
	g_signal_connect (G_OBJECT (zoom_in_button),"clicked",G_CALLBACK (img_zoom_in),img_struct);

	image_buttons = gtk_image_new_from_icon_name("zoom-out", GTK_ICON_SIZE_BUTTON);
	zoom_out_button = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), zoom_out_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(zoom_out_button), _("Zoom Out"));
	g_signal_connect (G_OBJECT (zoom_out_button),"clicked",G_CALLBACK (img_zoom_out),img_struct);

	image_buttons = gtk_image_new_from_icon_name("zoom-original", GTK_ICON_SIZE_BUTTON);
	zoom_normal = gtk_tool_button_new(image_buttons, NULL); 
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), zoom_normal, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(zoom_normal), _("Normal Size"));
	g_signal_connect (G_OBJECT (zoom_normal),"clicked",G_CALLBACK (img_zoom_reset),img_struct);

    
    image_buttons = gtk_image_new_from_icon_name ("zoom-fit-best", GTK_ICON_SIZE_BUTTON);
    zoom_fit = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), zoom_fit, -1);
    gtk_widget_set_tooltip_text(GTK_WIDGET(zoom_fit), _("Fit Zoom in Window"));
    g_signal_connect (G_OBJECT (zoom_fit),"clicked",G_CALLBACK (img_zoom_fit),img_struct);

	separatortoolitem = GTK_WIDGET (gtk_separator_tool_item_new());
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),separatortoolitem);

	image_buttons = gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
	img_struct->preview_button = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), img_struct->preview_button, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(img_struct->preview_button), _("Starts the preview without music"));
	g_signal_connect (G_OBJECT (img_struct->preview_button),"clicked",G_CALLBACK (img_start_stop_preview),img_struct);

	separatortoolitem = GTK_WIDGET (gtk_separator_tool_item_new());
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar),separatortoolitem);

	image_buttons = gtk_image_new_from_icon_name("go-first", GTK_ICON_SIZE_MENU);
	first_slide = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), first_slide, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(first_slide), _("Go to the first slide of the slideshow"));
	g_signal_connect (G_OBJECT (first_slide),"clicked",G_CALLBACK (img_goto_first_slide),img_struct);

	image_buttons = gtk_image_new_from_icon_name("go-previous", GTK_ICON_SIZE_MENU);
	prev_slide = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), prev_slide, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(prev_slide), _("Go to the previous slide of the slideshow") );
	g_signal_connect (G_OBJECT (prev_slide),"clicked",G_CALLBACK (img_goto_prev_slide),img_struct);

	toolbutton_slide_goto = GTK_WIDGET(gtk_tool_item_new ());
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar), toolbutton_slide_goto);
 
	img_struct->slide_number_entry = gtk_entry_new();
	atk = gtk_widget_get_accessible(img_struct->slide_number_entry);
	atk_object_set_description(atk, _("Current slide number"));
	gtk_entry_set_max_length(GTK_ENTRY (img_struct->slide_number_entry), 4);
	gtk_entry_set_width_chars(GTK_ENTRY (img_struct->slide_number_entry), 6);
	gtk_container_add(GTK_CONTAINER(toolbutton_slide_goto),img_struct->slide_number_entry);
	g_signal_connect(G_OBJECT (img_struct->slide_number_entry), "activate", G_CALLBACK(img_goto_line_entry_activate), img_struct);
	g_signal_connect(G_OBJECT (img_struct->slide_number_entry), "insert-text", G_CALLBACK (img_check_numeric_entry), NULL );

	toolbutton_slide_goto = GTK_WIDGET(gtk_tool_item_new ());
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar), toolbutton_slide_goto);

	label_of = gtk_label_new(_(" of "));
	gtk_container_add (GTK_CONTAINER (toolbutton_slide_goto),label_of);

	toolbutton_slide_goto = GTK_WIDGET(gtk_tool_item_new ());
	gtk_container_add (GTK_CONTAINER (img_struct->toolbar), toolbutton_slide_goto);

	img_struct->total_slide_number_label = gtk_label_new(NULL);
	atk = gtk_widget_get_accessible(img_struct->total_slide_number_label);
	atk_object_set_description(atk, _("Total number of slides"));
	gtk_container_add (GTK_CONTAINER (toolbutton_slide_goto),img_struct->total_slide_number_label);

	image_buttons = gtk_image_new_from_icon_name("go-next", GTK_ICON_SIZE_MENU); 
	next_slide = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), next_slide, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(next_slide), _("Go to the next slide of the slideshow") );
	g_signal_connect (G_OBJECT (next_slide),"clicked",G_CALLBACK (img_goto_next_slide),img_struct);

	image_buttons = gtk_image_new_from_icon_name("go-last", GTK_ICON_SIZE_MENU);
	last_slide = gtk_tool_button_new(image_buttons, NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(img_struct->toolbar), last_slide, -1);
	gtk_widget_set_tooltip_text(GTK_WIDGET(last_slide), _("Go to the last slide of the slideshow") );
	g_signal_connect (G_OBJECT (last_slide),"clicked",G_CALLBACK (img_goto_last_slide),img_struct);

	gtk_widget_show_all (img_struct->toolbar);

	/* Create the image area and the other widgets */
	img_struct->paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (vbox1), img_struct->paned, TRUE, TRUE, 0);

	modes_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_paned_add1( GTK_PANED( img_struct->paned ), modes_vbox );

	swindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start( GTK_BOX( modes_vbox ), swindow, TRUE, TRUE, 0 );
	img_struct->prev_root = swindow;

	img_struct->viewport_align = gtk_viewport_new( NULL, NULL );
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(img_struct->viewport_align), GTK_SHADOW_NONE);
	gtk_container_add( GTK_CONTAINER( swindow ), img_struct->viewport_align );

	gtk_widget_set_halign(img_struct->viewport_align, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(img_struct->viewport_align, GTK_ALIGN_CENTER);
	
	gtk_widget_set_hexpand(img_struct->viewport_align, FALSE);
	gtk_widget_set_vexpand(img_struct->viewport_align, FALSE);
	

	align = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(img_struct->viewport_align), align);

	img_struct->image_area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(align), img_struct->image_area);
	gtk_widget_add_events( img_struct->image_area, GDK_BUTTON1_MOTION_MASK
												 | GDK_POINTER_MOTION_HINT_MASK
												 | GDK_BUTTON_PRESS_MASK
												 | GDK_BUTTON_RELEASE_MASK
												 | GDK_SCROLL_MASK );
	g_signal_connect( G_OBJECT( img_struct->image_area ), "draw",
					  G_CALLBACK( img_on_draw_event ), img_struct );
	g_signal_connect( G_OBJECT( img_struct->image_area ), "button-press-event",
					  G_CALLBACK( img_image_area_button_press ), img_struct );
	g_signal_connect( G_OBJECT( img_struct->image_area ), "button-release-event",
					  G_CALLBACK( img_image_area_button_release ), img_struct );
	g_signal_connect( G_OBJECT( img_struct->image_area ), "motion-notify-event",
					  G_CALLBACK( img_image_area_motion ), img_struct );
	g_signal_connect( G_OBJECT( img_struct->image_area ), "scroll-event", 
					  G_CALLBACK( img_image_area_scroll ), img_struct);
	
	vbox_frames = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	scrollable_window = gtk_scrolled_window_new(NULL, NULL);
	g_object_set (G_OBJECT (scrollable_window),"hscrollbar-policy",GTK_POLICY_AUTOMATIC,"vscrollbar-policy",GTK_POLICY_AUTOMATIC,NULL);
	gtk_container_add (GTK_CONTAINER (scrollable_window), vbox_frames);
	
	video_tab = gtk_label_new (_("Video"));
	img_struct->notebook = gtk_notebook_new();
	gtk_paned_add2( GTK_PANED( img_struct->paned ), img_struct->notebook );
	gtk_notebook_append_page(GTK_NOTEBOOK(img_struct->notebook), scrollable_window, video_tab);

	viewport = gtk_bin_get_child(GTK_BIN(scrollable_window));
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(viewport), GTK_SHADOW_NONE);
	gtk_container_set_border_width( GTK_CONTAINER( viewport ), 5 );

	/* Slide frame */
	frame1 = gtk_frame_new (NULL);
	gtk_box_pack_start (GTK_BOX (vbox_frames), frame1, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_OUT);

	frame_label = gtk_label_new (_("<b>Slide Settings</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frame1), frame_label);
	gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);
	gtk_widget_set_margin_start(frame_label, 2);
	gtk_widget_set_margin_top(frame_label, 2);

	vbox_info_slide = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	gtk_container_add(GTK_CONTAINER(frame1), vbox_info_slide);
	gtk_widget_set_halign(GTK_WIDGET(vbox_info_slide), GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(GTK_WIDGET(vbox_info_slide), 2);
	gtk_widget_set_margin_bottom(GTK_WIDGET(vbox_info_slide), 2);
	gtk_widget_set_margin_start(GTK_WIDGET(vbox_info_slide), 5);
	gtk_widget_set_margin_end(GTK_WIDGET(vbox_info_slide), 5);
	gtk_container_set_border_width (GTK_CONTAINER (vbox_info_slide), 2);

	/* Transition types label */
	transition_label = gtk_label_new (_("Transition Type:"));
	gtk_box_pack_start (GTK_BOX (vbox_info_slide), transition_label, FALSE, FALSE, 0);
	gtk_label_set_xalign(GTK_LABEL(transition_label), 0);
	gtk_label_set_yalign(GTK_LABEL(transition_label), -1);

	/* Slide selected, slide resolution, slide type and slide total duration */
	table = gtk_table_new (4, 2, FALSE);
	gtk_box_pack_start (GTK_BOX (vbox_info_slide), table, TRUE, TRUE, 0);
	gtk_table_set_row_spacings (GTK_TABLE (table), 4);
	gtk_table_set_col_spacings (GTK_TABLE (table), 4);

	/* Transition type */
	img_struct->transition_type = _gtk_combo_box_new_text( TRUE );
	atk = gtk_widget_get_accessible(img_struct->transition_type);
	atk_object_set_description(atk, _("Transition type"));
	gtk_table_attach (GTK_TABLE (table), img_struct->transition_type, 0, 1, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(gtk_combo_box_get_model(GTK_COMBO_BOX(img_struct->transition_type))),
										1, GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(gtk_combo_box_get_model(GTK_COMBO_BOX(img_struct->transition_type))),1, img_sort_none_before_other,NULL,NULL);

	gtk_widget_set_sensitive(img_struct->transition_type, FALSE);
	g_signal_connect (G_OBJECT (img_struct->transition_type), "changed",G_CALLBACK (img_combo_box_transition_type_changed),img_struct);

	img_struct->random_button = gtk_button_new_with_mnemonic (_("Random"));
	gtk_widget_set_tooltip_text(img_struct->random_button,_("Imagination randomly decides which transition to apply"));
	gtk_table_attach (GTK_TABLE (table), img_struct->random_button, 1, 2, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_sensitive(img_struct->random_button, FALSE);
	g_signal_connect (G_OBJECT (img_struct->random_button),"clicked",G_CALLBACK (img_random_button_clicked),img_struct);

	/* Transition duration */
	trans_duration_label = gtk_label_new (_("Transition Speed:"));
	gtk_table_attach (GTK_TABLE (table), trans_duration_label, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_xalign(GTK_LABEL(trans_duration_label), 0);
	gtk_label_set_yalign(GTK_LABEL(trans_duration_label), 0.5);

	img_struct->trans_duration = _gtk_combo_box_new_text(FALSE);
	gtk_table_attach (GTK_TABLE (table), img_struct->trans_duration, 1, 2, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
	{
		GtkTreeIter   iter;
		GtkListStore *store = GTK_LIST_STORE( gtk_combo_box_get_model( GTK_COMBO_BOX( img_struct->trans_duration ) ) );

		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 0, _("Fast"), -1 );
		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 0, _("Normal"), -1 );
		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 0, _("Slow"), -1 );
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(img_struct->trans_duration),1);
	gtk_widget_set_sensitive(img_struct->trans_duration, FALSE);
	g_signal_connect (G_OBJECT (img_struct->trans_duration),"changed",G_CALLBACK (img_combo_box_speed_changed),img_struct);

	/* Slide duration */
	duration_label = gtk_label_new (_("Slide Duration in sec:"));
	gtk_table_attach (GTK_TABLE (table), duration_label, 0, 1, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_xalign(GTK_LABEL(duration_label), 0);
	gtk_label_set_yalign(GTK_LABEL(duration_label), 0.5);

	GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new (0.10, 0.01, 9999.0, 0.01, 1.0, 0.0);
	img_struct->duration = gtk_spin_button_new (adj, 1.0, 2);
	gtk_table_attach (GTK_TABLE (table), img_struct->duration, 1, 2, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_sensitive(img_struct->duration, FALSE);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON (img_struct->duration),TRUE);
	g_signal_connect (G_OBJECT (img_struct->duration),"value-changed",G_CALLBACK (img_spinbutton_value_changed),img_struct);

	/* Slide Total Duration */
	total_time = gtk_label_new (_("Slideshow Length:"));
	gtk_table_attach (GTK_TABLE (table), total_time, 0, 1, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_xalign(GTK_LABEL(total_time), 0);
	gtk_label_set_yalign(GTK_LABEL(total_time), 0.5);

	img_struct->total_time_data = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (table), img_struct->total_time_data, 1, 2, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_xalign(GTK_LABEL(img_struct->total_time_data), 0);
	gtk_label_set_yalign(GTK_LABEL(img_struct->total_time_data), 0.5);

	/* Slide motion frame */
	frame2 = gtk_frame_new (NULL);
	gtk_box_pack_start (GTK_BOX (vbox_frames), frame2, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_OUT);

	frame_label = gtk_label_new (_("<b>Slide Motion</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frame2), frame_label);
	gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);
	gtk_widget_set_margin_start(frame_label, 2);
	gtk_widget_set_margin_top(frame_label, 2);

	vbox_slide_motion = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	gtk_container_add(GTK_CONTAINER(frame2), vbox_slide_motion);
	gtk_widget_set_halign(GTK_WIDGET(vbox_slide_motion), GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(GTK_WIDGET(vbox_slide_motion), 2);
	gtk_widget_set_margin_bottom(GTK_WIDGET(vbox_slide_motion), 2);
	gtk_widget_set_margin_start(GTK_WIDGET(vbox_slide_motion), 5);
	gtk_widget_set_margin_end(GTK_WIDGET(vbox_slide_motion), 5);

	hbox_stop_points = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start (GTK_BOX (vbox_slide_motion), hbox_stop_points, TRUE, FALSE, 0);
	stop_points_label = gtk_label_new(_("Stop Point:"));
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), stop_points_label, TRUE, TRUE, 0);
	gtk_label_set_xalign(GTK_LABEL(stop_points_label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(stop_points_label), 0.5);

	img_struct->ken_left = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->ken_left ), "clicked",
					  G_CALLBACK( img_goto_prev_point ), img_struct );
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), img_struct->ken_left, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("go-previous", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(img_struct->ken_left), image_buttons);

	img_struct->ken_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY (img_struct->ken_entry), 2);
	gtk_entry_set_width_chars(GTK_ENTRY (img_struct->ken_entry), 4);
	{
		GObject *object = G_OBJECT( img_struct->ken_entry );
	
		g_signal_connect( object, "activate",
						  G_CALLBACK( img_goto_point ), img_struct );
		g_signal_connect( object, "insert-text",
						  G_CALLBACK( img_check_numeric_entry ), NULL );
	}
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), img_struct->ken_entry, FALSE, TRUE, 0);
	label_of = gtk_label_new(_(" of "));
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), label_of, FALSE, FALSE, 0);
	img_struct->total_stop_points_label = gtk_label_new(NULL);
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), img_struct->total_stop_points_label, FALSE, FALSE, 0);

	img_struct->ken_right = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->ken_right ), "clicked",
					  G_CALLBACK( img_goto_next_point ), img_struct );
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), img_struct->ken_right, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("go-next", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(img_struct->ken_right), image_buttons);

	img_struct->ken_add = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->ken_add ), "clicked",
					  G_CALLBACK( img_add_stop_point ), img_struct );
	gtk_box_pack_start (GTK_BOX (hbox_stop_points), img_struct->ken_add, FALSE, FALSE, 5);
	image_buttons = gtk_image_new_from_icon_name ("list-add", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->ken_add), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->ken_add,_("Add Stop point"));

	hbox_time_offset = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox_slide_motion), hbox_time_offset, FALSE, FALSE, 0);
	time_offset_label = gtk_label_new(_("Duration:"));
	gtk_box_pack_start (GTK_BOX (hbox_time_offset), time_offset_label, TRUE, TRUE, 0);
	gtk_label_set_xalign(GTK_LABEL(time_offset_label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(time_offset_label), 0.5);
	img_struct->ken_duration = gtk_spin_button_new_with_range (1, 60, 1);
	gtk_widget_set_size_request(img_struct->ken_duration, 50, -1);
	gtk_entry_set_max_length(GTK_ENTRY(img_struct->ken_duration), 2);
	gtk_box_pack_start (GTK_BOX (hbox_time_offset), img_struct->ken_duration, FALSE, FALSE, 0);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON (img_struct->ken_duration),TRUE);
	g_signal_connect( G_OBJECT( img_struct->ken_duration ), "value-changed",
					  G_CALLBACK( img_update_stop_point ), img_struct );
	
	img_struct->ken_remove = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->ken_remove ), "clicked",
					  G_CALLBACK( img_delete_stop_point ), img_struct );
	gtk_box_pack_start (GTK_BOX (hbox_time_offset), img_struct->ken_remove, FALSE, FALSE, 5);
	image_buttons = gtk_image_new_from_icon_name ("list-remove", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->ken_remove), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->ken_remove,_("Remove Stop point"));

	GtkWidget *hbox_zoom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox_slide_motion), hbox_zoom, FALSE, FALSE, 0);

	GtkWidget *label = gtk_label_new(_("Zoom: "));
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(label), 0.5);
	gtk_box_pack_start (GTK_BOX (hbox_zoom), label, FALSE, TRUE, 0);

	img_struct->ken_zoom = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
							1, 30,
							0.10000000000000001);
	gtk_scale_set_value_pos (GTK_SCALE(img_struct->ken_zoom), GTK_POS_LEFT);
	gtk_box_pack_start (GTK_BOX (hbox_zoom), img_struct->ken_zoom, TRUE, TRUE, 0);
	g_signal_connect( G_OBJECT( img_struct->ken_zoom ), "value-changed",
					  G_CALLBACK( img_ken_burns_zoom_changed ), img_struct );
	
	/* Slide text frame */
	frame4 = gtk_frame_new (NULL);
	gtk_box_pack_start (GTK_BOX (vbox_frames), frame4, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame4), GTK_SHADOW_OUT);

	frame_label = gtk_label_new (_("<b>Slide Text</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frame4), frame_label);
	gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);
	gtk_widget_set_margin_start(frame_label, 2);
	gtk_widget_set_margin_top(frame_label, 2);

	vbox_slide_caption = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	gtk_container_add(GTK_CONTAINER(frame4), vbox_slide_caption);
	gtk_widget_set_halign(GTK_WIDGET(vbox_slide_caption), GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(GTK_WIDGET(vbox_slide_caption), 5);
	gtk_widget_set_margin_bottom(GTK_WIDGET(vbox_slide_caption), 5);
	gtk_widget_set_margin_start(GTK_WIDGET(vbox_slide_caption), 5);
	gtk_widget_set_margin_end(GTK_WIDGET(vbox_slide_caption), 5);

	a_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_box_pack_start (GTK_BOX (vbox_slide_caption), a_hbox, FALSE, FALSE, 0);

	img_struct->sub_color = gtk_color_button_new();
	gtk_color_button_set_use_alpha( GTK_COLOR_BUTTON( img_struct->sub_color ), TRUE );
	g_signal_connect( G_OBJECT( img_struct->sub_color ), "color-set",
					  G_CALLBACK( img_font_color_changed ), img_struct );
	gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->sub_color, FALSE, FALSE, 0 );
    gtk_widget_set_tooltip_text(img_struct->sub_color, _("Font color"));

	img_struct->sub_brdr_color = gtk_color_button_new();
    gtk_color_button_set_use_alpha( GTK_COLOR_BUTTON( img_struct->sub_brdr_color ), TRUE );
    g_signal_connect( G_OBJECT( img_struct->sub_brdr_color ), "color-set",
                      G_CALLBACK( img_font_brdr_color_changed ), img_struct );
    gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->sub_brdr_color, FALSE, FALSE, 0 );
    gtk_widget_set_tooltip_text(img_struct->sub_brdr_color, _("Font border color. If the opacity value is set to 0, Imagination will not render any border"));

	img_struct->sub_bgcolor = gtk_color_button_new();
	gtk_color_button_set_use_alpha( GTK_COLOR_BUTTON( img_struct->sub_bgcolor ), TRUE );
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON( img_struct->sub_bgcolor ), 0);
    g_signal_connect( G_OBJECT( img_struct->sub_bgcolor ), "color-set",
                      G_CALLBACK( img_font_bg_color_changed ), img_struct );
    gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->sub_bgcolor, FALSE, FALSE, 0 );
	gtk_widget_set_tooltip_text(img_struct->sub_bgcolor, _("Font background color"));

	img_struct->bold_style = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->bold_style ), "clicked",
					  G_CALLBACK( img_subtitle_style_changed ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->bold_style, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-text-bold", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->bold_style), image_buttons);

	img_struct->italic_style = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->italic_style ), "clicked",
					  G_CALLBACK( img_subtitle_style_changed ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->italic_style, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-text-italic", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->italic_style), image_buttons);

	img_struct->underline_style = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->underline_style ), "clicked",
					  G_CALLBACK( img_subtitle_style_changed ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->underline_style, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-text-underline", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->underline_style), image_buttons);
	
	img_struct->clear_formatting = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->clear_formatting ), "clicked",
					  G_CALLBACK( img_subtitle_style_changed ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->clear_formatting, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("edit-clear", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->clear_formatting), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->clear_formatting, _("Remove formatting"));

	img_struct->left_justify = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->left_justify ), "clicked",
					  G_CALLBACK( img_set_slide_text_align ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->left_justify, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-justify-left", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->left_justify), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->left_justify, _("Align left"));

	img_struct->fill_justify = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->fill_justify ), "clicked",
					  G_CALLBACK( img_set_slide_text_align ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->fill_justify, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-justify-center", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->fill_justify), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->fill_justify, _("Align center"));

	img_struct->right_justify = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->right_justify ), "clicked",
					  G_CALLBACK( img_set_slide_text_align ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->right_justify, FALSE, FALSE, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-justify-right", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->right_justify), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->right_justify, _("Align right"));

	hbox_textview = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_box_pack_start (GTK_BOX (vbox_slide_caption), hbox_textview, FALSE, FALSE, 0);
	
	img_struct->sub_textview = gtk_text_view_new();
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(img_struct->sub_textview), FALSE);
	img_struct->slide_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(img_struct->sub_textview));
	g_signal_connect( G_OBJECT( img_struct->slide_text_buffer ), "changed",
					  G_CALLBACK( img_queue_subtitle_update ), img_struct );
	/* Let's connect the focus-in and focus-out events to prevent the
	 * DEL key when pressed inside the textview delete the selected slide */
	g_signal_connect( G_OBJECT( img_struct->sub_textview ), "focus-in-event",
					  G_CALLBACK( img_sub_textview_focus_in ), img_struct );
	g_signal_connect( G_OBJECT( img_struct->sub_textview ), "focus-out-event",
					  G_CALLBACK( img_sub_textview_focus_out ), img_struct );
	img_struct->scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	g_object_set (G_OBJECT (img_struct->scrolled_win),"hscrollbar-policy",GTK_POLICY_AUTOMATIC,"vscrollbar-policy",GTK_POLICY_AUTOMATIC,"shadow-type",GTK_SHADOW_IN,NULL);
	gtk_container_add(GTK_CONTAINER (img_struct->scrolled_win), img_struct->sub_textview);
	gtk_box_pack_start (GTK_BOX (hbox_textview), img_struct->scrolled_win, TRUE, TRUE, 0);
	gtk_widget_set_size_request(img_struct->scrolled_win, -1, 70);

	text_animation_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_box_pack_start( GTK_BOX( vbox_slide_caption ), text_animation_hbox, FALSE, FALSE, 0 );

	img_struct->sub_font = gtk_font_button_new();
	g_signal_connect( G_OBJECT( img_struct->sub_font ), "font-set",
					  G_CALLBACK( img_text_font_set ), img_struct );
	gtk_box_pack_start (GTK_BOX (text_animation_hbox), img_struct->sub_font, TRUE, TRUE, 0);
	gtk_widget_set_tooltip_text(img_struct->sub_font, _("Click to choose the font"));

	pixbuf = gtk_icon_theme_load_icon(icon_theme,"image", 20, 0, NULL);
	tmp_image = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);

	img_struct->pattern_image = GTK_WIDGET (gtk_tool_button_new (tmp_image,""));
	gtk_box_pack_start (GTK_BOX (text_animation_hbox), img_struct->pattern_image, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(img_struct->pattern_image, _("Click to choose the text pattern") );
	g_signal_connect (	G_OBJECT (img_struct->pattern_image), "clicked", G_CALLBACK (img_pattern_clicked), img_struct);
   
	gtk_widget_set_size_request(img_struct->sub_color, 35, -1);
	gtk_widget_set_size_request(img_struct->sub_brdr_color, 35, -1);
	gtk_widget_set_size_request(img_struct->sub_bgcolor, 35, -1);
	
	a_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_box_pack_start (GTK_BOX (vbox_slide_caption), a_hbox, FALSE, FALSE, 0);
	
	img_struct->border_top = gtk_check_button_new_with_mnemonic(_("Top Border"));
	gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->border_top, TRUE, TRUE, 0 );
	g_signal_connect (img_struct->border_top, "toggled", G_CALLBACK (img_subtitle_top_border_toggled), img_struct);

	img_struct->border_bottom = gtk_check_button_new_with_mnemonic(_("Bottom Border"));
	gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->border_bottom, TRUE, TRUE, 0 );
	g_signal_connect (img_struct->border_bottom, "toggled", G_CALLBACK (img_subtitle_bottom_border_toggled), img_struct);

	img_struct->sub_border_width = gtk_spin_button_new_with_range (1, 10, 1);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON (img_struct->sub_border_width),TRUE);
	gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->sub_border_width, FALSE, FALSE, 0 );
	gtk_widget_set_size_request(img_struct->sub_border_width, 50, -1);
	gtk_entry_set_max_length(GTK_ENTRY(img_struct->sub_border_width), 2);
	gtk_widget_set_tooltip_text(img_struct->sub_border_width, _("Border width") );
	g_signal_connect( G_OBJECT( img_struct->sub_border_width ), "value-changed", G_CALLBACK( img_sub_border_width_changed ), img_struct );

	img_struct->sub_border_color = gtk_color_button_new();
	g_signal_connect( G_OBJECT( img_struct->sub_border_color ), "color-set", G_CALLBACK( img_sub_border_color_changed ), img_struct );
    gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->sub_border_color, FALSE, FALSE, 0 );
	gtk_widget_set_tooltip_text(img_struct->sub_border_color, _("Border color"));
	gtk_widget_set_size_request(img_struct->sub_border_color, 35, -1);

	a_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_box_pack_start (GTK_BOX (vbox_slide_caption), a_hbox, FALSE, FALSE, 0);
	a_label = gtk_label_new(_("Animation:"));
	gtk_label_set_xalign(GTK_LABEL(a_label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(a_label), 0.5);
	gtk_box_pack_start (GTK_BOX (a_hbox), a_label, TRUE, TRUE, 0);
	img_struct->sub_anim = img_create_subtitle_animation_combo();
	gtk_combo_box_set_active(GTK_COMBO_BOX(img_struct->sub_anim), 0);
	g_signal_connect( G_OBJECT( img_struct->sub_anim ), "changed",
					  G_CALLBACK( img_text_anim_set ), img_struct );
	gtk_box_pack_start (GTK_BOX (a_hbox), img_struct->sub_anim, FALSE, FALSE, 0);

	a_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_box_pack_start (GTK_BOX (vbox_slide_caption), a_hbox, FALSE, FALSE, 0);

	a_label = gtk_label_new( _("Animation Speed:") );
	gtk_label_set_xalign(GTK_LABEL(a_label), 0);
	gtk_label_set_yalign(GTK_LABEL(a_label), 0.5);
	gtk_box_pack_start( GTK_BOX( a_hbox ), a_label, TRUE, TRUE, 0 );

	img_struct->sub_anim_duration = gtk_spin_button_new_with_range (1, 60, 1);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON (img_struct->sub_anim_duration),TRUE);
	gtk_box_pack_start( GTK_BOX( a_hbox ), img_struct->sub_anim_duration,
						FALSE, FALSE, 0 );
	gtk_widget_set_size_request(img_struct->sub_anim_duration, 50, -1);
	gtk_entry_set_max_length(GTK_ENTRY(img_struct->sub_anim_duration), 2);
	
	gtk_widget_set_sensitive( img_struct->sub_anim_duration, FALSE );
	g_signal_connect( G_OBJECT( img_struct->sub_anim_duration ), "value-changed",
					  G_CALLBACK( img_combo_box_anim_speed_changed ), img_struct );

	frame4 = gtk_frame_new (NULL);
	gtk_box_pack_start (GTK_BOX (vbox_frames), frame4, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame4), GTK_SHADOW_OUT);

	vbox_slide_position = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	gtk_container_add (GTK_CONTAINER (frame4), vbox_slide_position);
	gtk_widget_set_halign(GTK_WIDGET(vbox_slide_position), GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(GTK_WIDGET(vbox_slide_position), 5);
	gtk_widget_set_margin_bottom(GTK_WIDGET(vbox_slide_position), 5);
	gtk_widget_set_margin_start(GTK_WIDGET(vbox_slide_position), 5);
	gtk_widget_set_margin_end(GTK_WIDGET(vbox_slide_position), 5);

	frame_label = gtk_label_new (_("<b>Slide Text Position</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frame4), frame_label);
	gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);
	gtk_widget_set_margin_start(frame_label, 2);
	gtk_widget_set_margin_top(frame_label, 2);

	table = gtk_table_new (3, 3, FALSE);
	gtk_box_pack_start (GTK_BOX (vbox_slide_position), table, TRUE, TRUE, 0);
	gtk_table_set_row_spacings (GTK_TABLE (table), 4);
	gtk_table_set_col_spacings (GTK_TABLE (table), 4);

	label = gtk_label_new(_("Horizontal Position: "));
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(label), 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

	img_struct->sub_posX_adj = (GtkAdjustment *) gtk_adjustment_new( 1.0, 1, (gdouble)img_struct->video_size[0], 1.0, 1.0, 0.0 );
	img_struct->sub_posX = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
					     img_struct->sub_posX_adj);
	gtk_table_attach (GTK_TABLE (table), img_struct->sub_posX, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_scale_set_draw_value( GTK_SCALE(img_struct->sub_posX), FALSE);
	g_signal_connect( G_OBJECT( img_struct->sub_posX ), "value-changed",
					  G_CALLBACK( img_text_pos_changed ), img_struct );

	img_struct->x_justify = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->x_justify ), "clicked",
					  G_CALLBACK( img_align_text_horizontally_vertically ), img_struct );
	gtk_table_attach (GTK_TABLE (table), img_struct->x_justify, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-justify-center", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->x_justify), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->x_justify,_("Center the text horizontally"));

	label = gtk_label_new(_("Vertical Position: "));
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(label), 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

	img_struct->sub_posY_adj = (GtkAdjustment *) gtk_adjustment_new( 1.0, 1, (gdouble)img_struct->video_size[1], 1.0, 1.0, 0.0 );
	img_struct->sub_posY = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
					     img_struct->sub_posY_adj);
	gtk_table_attach (GTK_TABLE (table), img_struct->sub_posY, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_scale_set_draw_value( GTK_SCALE(img_struct->sub_posY), FALSE);
	g_signal_connect( G_OBJECT( img_struct->sub_posY ), "value-changed",
					  G_CALLBACK( img_text_pos_changed ), img_struct );

	img_struct->y_justify = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->y_justify ), "clicked",
					  G_CALLBACK( img_align_text_horizontally_vertically ), img_struct );
	gtk_table_attach (GTK_TABLE (table), img_struct->y_justify, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	image_buttons = gtk_image_new_from_icon_name ("format-justify-center", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->y_justify), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->y_justify,_("Center the text vertically"));

	label = gtk_label_new(_("Angle: "));
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(label), 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

	img_struct->sub_angle = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
							 -90, 90, 1);
	gtk_table_attach (GTK_TABLE (table), img_struct->sub_angle, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_scale_set_draw_value( GTK_SCALE(img_struct->sub_angle), FALSE);
	g_signal_connect( G_OBJECT( img_struct->sub_angle ), "value-changed",
					  G_CALLBACK( img_text_pos_changed ), img_struct );

	img_struct->reset_angle = gtk_button_new();
	g_signal_connect( G_OBJECT( img_struct->reset_angle ), "clicked",
					  G_CALLBACK( img_align_text_horizontally_vertically ), img_struct );
	gtk_table_attach (GTK_TABLE (table), img_struct->reset_angle, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	image_buttons = gtk_image_new_from_icon_name ("edit-delete", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(img_struct->reset_angle), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->reset_angle,_("Reset the angle"));

	/* Background music frame */
	audio_tab = gtk_label_new (_("Audio"));
	vbox_audio_frames = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox_audio_frames), 10);
	gtk_notebook_append_page(GTK_NOTEBOOK(img_struct->notebook), vbox_audio_frames, audio_tab);

	frame3 = gtk_frame_new (NULL);
	gtk_box_pack_start (GTK_BOX (vbox_audio_frames), frame3, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame3), GTK_SHADOW_OUT);

	frame_label = gtk_label_new (_("<b>Background Music</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frame3), frame_label);
	gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);
	gtk_widget_set_margin_start(frame_label, 2);
	gtk_widget_set_margin_top(frame_label, 2);

	/* Add the liststore */
	vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(frame3), vbox2);
	gtk_widget_set_halign(GTK_WIDGET(vbox2), GTK_ALIGN_FILL);
	gtk_widget_set_margin_top(GTK_WIDGET(vbox2), 2);
	gtk_widget_set_margin_bottom(GTK_WIDGET(vbox2), 2);
	gtk_widget_set_margin_start(GTK_WIDGET(vbox2), 5);
	gtk_widget_set_margin_end(GTK_WIDGET(vbox2), 5);
	gtk_container_set_border_width (GTK_CONTAINER (vbox2), 0);
	
	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

	img_struct->music_file_liststore = gtk_list_store_new (6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
	g_signal_connect (G_OBJECT (img_struct->music_file_liststore), "row-inserted",	G_CALLBACK (img_activate_remove_button_music_liststore) , img_struct);

	img_struct->music_file_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(img_struct->music_file_liststore));
	//selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(img_struct->music_file_treeview));

	gtk_drag_dest_set (GTK_WIDGET(img_struct->music_file_treeview),GTK_DEST_DEFAULT_ALL,drop_targets,1,GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK | GDK_ACTION_ASK);
	g_signal_connect (G_OBJECT (img_struct->music_file_treeview),"drag-data-received",G_CALLBACK (img_on_drag_audio_data_received), img_struct);

	/* First and last three columns aren't displayed, so we
	 * only need two columns. */
	for (x = 1; x < 3; x++)
	{
		column = gtk_tree_view_column_new();
		renderer = gtk_cell_renderer_text_new();
		gtk_tree_view_column_pack_start(column, renderer, TRUE);
		gtk_tree_view_column_set_attributes(column, renderer, "text", x,
															  "foreground", 4,
															  NULL );
		gtk_tree_view_append_column (GTK_TREE_VIEW (img_struct->music_file_treeview), column);
	}

	//gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(img_struct->music_file_treeview), TRUE);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (img_struct->music_file_treeview), FALSE);
	gtk_tree_view_set_tooltip_column( GTK_TREE_VIEW( img_struct->music_file_treeview ), 5 );
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), img_struct->music_file_treeview);

	/* Add the total music labels and the buttons */
	hbox_music_label = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_box_pack_start( GTK_BOX( vbox2 ), hbox_music_label, FALSE, FALSE, 0 );

	music_time = gtk_label_new(_("Music Duration:"));
	gtk_box_pack_start(GTK_BOX(hbox_music_label), music_time, TRUE, TRUE, 0);
	gtk_label_set_xalign(GTK_LABEL(music_time), 0);
	gtk_label_set_yalign(GTK_LABEL(music_time), 0.5);

	img_struct->music_time_data = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox_music_label), img_struct->music_time_data, TRUE, TRUE, 0);
	gtk_label_set_xalign(GTK_LABEL(img_struct->music_time_data), 1);
	gtk_label_set_yalign(GTK_LABEL(img_struct->music_time_data), 0.5);

	/* Add the fadeout duration and spinner */
	hbox_fadeout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_box_pack_start( GTK_BOX( vbox2 ), hbox_fadeout, FALSE, FALSE, 0 );

	fadeout_duration = gtk_label_new(_("Final fade-out duration:"));
	gtk_box_pack_start(GTK_BOX(hbox_fadeout), fadeout_duration, TRUE, TRUE, 0);
	gtk_label_set_xalign(GTK_LABEL(fadeout_duration), 0);
	gtk_label_set_yalign(GTK_LABEL(fadeout_duration), 0.5);

	GtkAdjustment *adj2 = (GtkAdjustment *) gtk_adjustment_new (img_struct->audio_fadeout, 0, 100, 1, 5, 0.0);
	img_struct->fadeout_duration = gtk_spin_button_new (adj2, 1.0, 0);
	g_signal_connect (G_OBJECT (img_struct->fadeout_duration),"value-changed",G_CALLBACK (img_fadeout_duration_changed),img_struct);
	gtk_box_pack_start(GTK_BOX(hbox_fadeout), img_struct->fadeout_duration, TRUE, TRUE, 0);

	hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_box_set_homogeneous(GTK_BOX(hbox_buttons), TRUE);
	gtk_box_pack_start( GTK_BOX( vbox2 ), hbox_buttons, FALSE, FALSE, 0 );

	img_struct->play_audio_button = gtk_button_new();
	gtk_widget_set_sensitive(img_struct->play_audio_button, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox_buttons), img_struct->play_audio_button, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("media-playback-start", GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (img_struct->play_audio_button), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->play_audio_button, _("Play the selected file"));
	g_signal_connect ( (gpointer) img_struct->play_audio_button, "clicked", G_CALLBACK (img_play_stop_selected_file), img_struct);

	img_struct->remove_audio_button = gtk_button_new();
	gtk_widget_set_sensitive(img_struct->remove_audio_button, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox_buttons), img_struct->remove_audio_button, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("edit-delete", GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (img_struct->remove_audio_button), image_buttons);
	gtk_widget_set_tooltip_text(img_struct->remove_audio_button, _("Delete the selected file"));
	g_signal_connect ( (gpointer) img_struct->remove_audio_button, "clicked", G_CALLBACK (img_remove_audio_files), img_struct);

	move_up_button = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox_buttons), move_up_button, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("go-up", GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (move_up_button), image_buttons);
	gtk_widget_set_tooltip_text(move_up_button, _("Move the selected file up"));
	g_signal_connect( G_OBJECT( move_up_button ), "clicked", G_CALLBACK( img_move_audio_up ), img_struct );

	move_down_button = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox_buttons), move_down_button, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("go-down", GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (move_down_button), image_buttons);
	gtk_widget_set_tooltip_text(move_down_button, _("Move the selected file down"));
	g_signal_connect( G_OBJECT( move_down_button ), "clicked", G_CALLBACK( img_move_audio_down ), img_struct );
	
	clear_button = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox_buttons), clear_button, FALSE, TRUE, 0);
	image_buttons = gtk_image_new_from_icon_name ("edit-clear", GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (clear_button), image_buttons);
	gtk_widget_set_tooltip_text(clear_button, _("Clear all files"));
	g_signal_connect ( (gpointer) clear_button, "clicked", G_CALLBACK (img_clear_audio_files), img_struct);
    /* End of Background music frame */
    
    /* Begin of Message tab */
    img_struct->message_label = gtk_label_new (_("Messages"));
    message_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(message_scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    img_struct->message_page = gtk_notebook_append_page(GTK_NOTEBOOK(img_struct->notebook),
                             message_scroll,
                             img_struct->message_label);

    message_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(message_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(message_view), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(message_scroll), message_view);
    img_struct->message_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (message_view));
    g_signal_connect( (gpointer) img_struct->notebook, "switch_page",
                      G_CALLBACK (img_notebook_switch_page), img_struct);
    /* End of Message tab */

	/* Create the bottom slides thumbnail model */
	img_struct->thumbnail_model = gtk_list_store_new( 4,
                                                      GDK_TYPE_PIXBUF,  /* thumbnail */
                                                      G_TYPE_POINTER,   /* slide_info */
													  GDK_TYPE_PIXBUF,  /* transition thumbnail */
													  G_TYPE_BOOLEAN ); /* presence of a subtitle */

	/* Create overview mode widgets */
	/* FIXME: A lot of duplicate code here!! */
	{
		GtkWidget       *icon,
						*swindow;
		GtkCellRenderer *cell;
		gchar           *path;
		GdkPixbuf       *text;

		swindow = gtk_scrolled_window_new( NULL, NULL );
		gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swindow ),
										GTK_POLICY_AUTOMATIC,
										GTK_POLICY_AUTOMATIC );
		gtk_box_pack_start( GTK_BOX( modes_vbox ), swindow, TRUE, TRUE, 0 );
		img_struct->over_root = swindow;

		icon = gtk_icon_view_new_with_model(
					GTK_TREE_MODEL( img_struct->thumbnail_model ) );
		gtk_icon_view_set_selection_mode( GTK_ICON_VIEW( icon ),
										  GTK_SELECTION_MULTIPLE );
		gtk_icon_view_set_item_orientation( GTK_ICON_VIEW( icon ),
									   GTK_ORIENTATION_HORIZONTAL );
		gtk_icon_view_set_column_spacing( GTK_ICON_VIEW( icon ), 0 );
		gtk_icon_view_set_row_spacing( GTK_ICON_VIEW( icon ), 0 );
		img_struct->over_icon = icon;
		img_struct->active_icon = icon;
		g_signal_connect( G_OBJECT( icon ), "selection-changed",
						  G_CALLBACK( img_iconview_selection_changed ),
						  img_struct );
		g_signal_connect( G_OBJECT( icon ), "select-all",
						  G_CALLBACK( img_iconview_selection_changed ),
						  img_struct );
		g_signal_connect( G_OBJECT( icon ), "button-press-event",
						  G_CALLBACK( img_iconview_selection_button_press ),
						  img_struct );
		gtk_container_add( GTK_CONTAINER( swindow ), icon );

		cell = img_cell_renderer_pixbuf_new();
		gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( icon ), cell, FALSE );
		img_struct->over_cell = G_OBJECT( cell );

#if PLUGINS_INSTALLED
		path = g_strconcat( DATADIR,
							"/imagination/pixmaps/imagination-text.png",
							NULL );
#else
		path = g_strdup( "pixmaps/imagination-text.png" );
#endif
		text = gdk_pixbuf_new_from_file( path, NULL );
		g_free( path );
		g_object_set( G_OBJECT( cell ), "ypad", 2,
										"text-ico", text,
										NULL );
		g_object_unref( G_OBJECT( text ) );
		gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( icon ), cell,
										"pixbuf", 0,
										"transition", 2,
										"has-text", 3,
										NULL );
		gtk_icon_view_set_reorderable( GTK_ICON_VIEW( icon ), TRUE );
	}

	/* Add wrapper for DnD */
	eventbox = gtk_event_box_new();
	img_struct->thum_root = eventbox;
	gtk_event_box_set_above_child( GTK_EVENT_BOX( eventbox ), FALSE );
	gtk_event_box_set_visible_window( GTK_EVENT_BOX( eventbox ), FALSE );
	gtk_drag_dest_set( GTK_WIDGET( eventbox ), GTK_DEST_DEFAULT_ALL,
					   drop_targets, 1, GDK_ACTION_COPY | GDK_ACTION_MOVE |
					   GDK_ACTION_LINK | GDK_ACTION_ASK);
	g_signal_connect( G_OBJECT( eventbox ), "drag-data-received",
					  G_CALLBACK( img_on_drag_data_received), img_struct );
	gtk_box_pack_start( GTK_BOX( vbox1 ), eventbox, FALSE, TRUE, 0 );

	/* Create the thumbnail viewer */
	img_struct->thumb_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request( img_struct->thumb_scrolledwindow, -1, 85 );
	g_signal_connect( G_OBJECT( img_struct->thumb_scrolledwindow ), "scroll-event",
					  G_CALLBACK( img_scroll_thumb ), img_struct );
	gtk_container_add( GTK_CONTAINER( eventbox ), img_struct->thumb_scrolledwindow );
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (img_struct->thumb_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (img_struct->thumb_scrolledwindow), GTK_SHADOW_IN);

	img_struct->thumbnail_iconview = gtk_icon_view_new_with_model(GTK_TREE_MODEL (img_struct->thumbnail_model));
	gtk_container_add( GTK_CONTAINER( img_struct->thumb_scrolledwindow ), img_struct->thumbnail_iconview );

	/* Create the cell layout */
	pixbuf_cell = img_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (img_struct->thumbnail_iconview), pixbuf_cell, FALSE);
	{
		gchar     *path;
		GdkPixbuf *text;

#if PLUGINS_INSTALLED
		path = g_strconcat( DATADIR,
							"/imagination/pixmaps/imagination-text.png",
							NULL );
#else
		path = g_strdup( "pixmaps/imagination-text.png" );
#endif
		text = gdk_pixbuf_new_from_file( path, NULL );
		g_free( path );
		g_object_set( G_OBJECT( pixbuf_cell ), "width", 115,
											   "ypad", 2,
											   "text-ico", text,
											   NULL );
		g_object_unref( G_OBJECT( text ) );
	}
	gtk_cell_layout_set_attributes(
			GTK_CELL_LAYOUT( img_struct->thumbnail_iconview ), pixbuf_cell,
			"pixbuf", 0,
			"transition", 2,
			"has-text", 3,
			NULL );

	/* Set some iconview properties */
	gtk_icon_view_set_text_column( GTK_ICON_VIEW( img_struct->thumbnail_iconview ), -1 );
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW (img_struct->thumbnail_iconview),TRUE);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (img_struct->thumbnail_iconview), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_item_orientation (GTK_ICON_VIEW (img_struct->thumbnail_iconview), GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_column_spacing (GTK_ICON_VIEW (img_struct->thumbnail_iconview),0);
	gtk_icon_view_set_row_spacing (GTK_ICON_VIEW (img_struct->thumbnail_iconview),0);
	gtk_icon_view_set_item_padding (GTK_ICON_VIEW (img_struct->thumbnail_iconview), 0);

	g_signal_connect (G_OBJECT (img_struct->thumbnail_iconview),"selection-changed",G_CALLBACK (img_iconview_selection_changed),img_struct);
	g_signal_connect (G_OBJECT (img_struct->thumbnail_iconview),"select-all",G_CALLBACK (img_iconview_selection_changed),img_struct);
	g_signal_connect (G_OBJECT (img_struct->thumbnail_iconview),"button-press-event",G_CALLBACK (img_iconview_selection_button_press),img_struct);
	gtk_widget_show_all( eventbox );

	/* Create the status bar */
	img_struct->statusbar = gtk_statusbar_new ();
	gtk_widget_show (img_struct->statusbar);
	gtk_box_pack_start (GTK_BOX (vbox1), img_struct->statusbar, FALSE, TRUE, 0);
	img_struct->context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (img_struct->statusbar), "statusbar");

	/* Create the progress bar */
	img_struct->progress_bar = gtk_progress_bar_new();
	gtk_widget_set_size_request (img_struct->progress_bar, -1, 15);
	{
		GtkWidget *vbox;

		vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_box_pack_start (GTK_BOX (img_struct->statusbar), vbox, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), img_struct->progress_bar, TRUE, FALSE, 0);
		gtk_widget_show (vbox);
	}
	gtk_widget_show_all( img_struct->paned );
	gtk_window_add_accel_group (GTK_WINDOW (img_struct->imagination_window), img_struct->accel_group);

	/* Disable all Ken Burns controls */
	img_ken_burns_update_sensitivity( img_struct, FALSE, 0 );

	/* Disable all subtitle controls */
	img_subtitle_update_sensitivity( img_struct, 0 );

	/* Load interface settings or apply default ones */
	if( ! img_load_window_settings( img_struct ) )
		img_set_window_default_settings( img_struct );

	/* Update preview frame rate */
	{
		gint index =
			( img_struct->preview_fps - PREVIEW_FPS_MIN ) / PREVIEW_FPS_STEP;
		gtk_check_menu_item_set_active(
				GTK_CHECK_MENU_ITEM( tmp_checks[index] ), TRUE );
	}
	
	/* As many distros replaced ffmpeg with avconv let's check for it */
	img_check_for_encoder(img_struct);
	return img_struct;
}

static GtkWidget *img_load_icon_from_theme(GtkIconTheme* icon_theme, gchar *name, gint size) {
	GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(icon_theme, name, size,0,NULL);
	if (!pixbuf) 
	    pixbuf = gtk_icon_theme_load_icon(icon_theme, "image-missing", size, 0, NULL);
	GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	return image;
}

static void img_slide_cut(GtkMenuItem* UNUSED(item), img_window_struct *img)
{
	img_clipboard_cut_copy_operation(img, IMG_CLIPBOARD_CUT);
}

static void img_slide_copy(GtkMenuItem* UNUSED(item), img_window_struct *img)
{
	img_clipboard_cut_copy_operation(img, IMG_CLIPBOARD_COPY);
}

static void img_slide_paste(GtkMenuItem* UNUSED(item), img_window_struct *img)
{
	GtkClipboard *clipboard;
	GtkSelectionData *selection;
	GList *where_to_paste = NULL, *node;
	GtkTreeModel *model;
	GtkTreeIter iter, position_iter;
	GdkPixbuf *thumb, *trans;
	gboolean   has_sub;
	slide_struct *pasted_slide, *info_slide;
	gint pos;
    GtkTreeRowReference  *position_rawref;
    GList                *rowref_list = NULL;
    GtkTreePath          *path, *position_path;


    clipboard = gtk_clipboard_get(IMG_CLIPBOARD);
	selection = gtk_clipboard_wait_for_contents(clipboard, IMG_INFO_LIST);

	if (selection == NULL)
	{
		img_message (img, FALSE, "Paste: selection is NULL\n");
		return;
	}

	model			 =	GTK_TREE_MODEL(img->thumbnail_model);
	where_to_paste	 =	gtk_icon_view_get_selected_items(GTK_ICON_VIEW(img->active_icon));
    if (where_to_paste == NULL)     /*no icon selected */
        return;
    position_rawref  =  gtk_tree_row_reference_new(model, where_to_paste->data);

    /* Build a list selected of icons to move */
    node = img->selected_paths;
    while (node)
    {
        rowref_list = g_list_append(rowref_list, gtk_tree_row_reference_new(model, node->data));
        node = node->next;
    }

	if (img->clipboard_mode == IMG_CLIPBOARD_CUT)
	{
        /* move the icons */
		node = rowref_list;
		while (node)
        {
            path = gtk_tree_row_reference_get_path(node->data);
            if (path)
            {
                if (gtk_tree_model_get_iter(model, &iter, path))
                {
                    position_path = gtk_tree_row_reference_get_path(position_rawref);
                    /* position_path can not be NULL as we only move, without delete */
                    assert (position_path != NULL);
                    gtk_tree_model_get_iter (model, &position_iter,  position_path);
                    gtk_list_store_move_after(GTK_LIST_STORE(model), &iter, &position_iter);
                    gtk_tree_path_free(position_path);
                }
                gtk_tree_path_free(path);
            }
            node = node->next;
        }
	}
	else    /* clipboard copy */
	{
        node = rowref_list;
        while (node)
		{
            path = gtk_tree_row_reference_get_path(node->data);
            if (path)
            {
                if (gtk_tree_model_get_iter(model, &iter, path))
                {
                    /* Get slide and some additional data */
                    gtk_tree_model_get_iter(model, &iter, path);
                    gtk_tree_model_get(model, &iter, 0, &thumb,
                                                    1, &info_slide,
                                                    2, &trans,
                                                    3, &has_sub,
                                                    -1);
                    if( thumb )
                        g_object_unref( G_OBJECT( thumb ) );
                    if( trans )
                        g_object_unref( G_OBJECT( trans ) );

                    /* Create new slide that is exact copy of rpevious one */
                    pasted_slide = g_slice_copy( sizeof( slide_struct ), info_slide );

                    if (pasted_slide)
                    {
                        /* Fill fields with fresh strings, since g_slice_copy cannot do
                        * that for us. */
                        pasted_slide->o_filename = g_strdup(info_slide->o_filename);
                        pasted_slide->p_filename = g_strdup(info_slide->p_filename);
                        pasted_slide->original_filename = g_strdup(info_slide->original_filename);
                        pasted_slide->resolution = g_strdup(info_slide->resolution);
                        pasted_slide->type = info_slide->type ? g_strdup(info_slide->type): NULL;
                        pasted_slide->path = g_strdup(info_slide->path);

                        /* Stop Points also need to copied by hand. */
                        if (info_slide->no_points)
                        {
                            GList *dummy_pnt = info_slide->points;
                            ImgStopPoint *point;

                            pasted_slide->points = NULL;
                            while (dummy_pnt)
                            {
                                point = g_slice_copy( sizeof( ImgStopPoint ),
                                                    dummy_pnt->data );
                                pasted_slide->points = g_list_append(pasted_slide->points, point);
                                dummy_pnt = dummy_pnt->next;
                            }
                        }

                        /* Text should be duplicated if present. Font description
                        * should also be copied!! */
                        if (info_slide->subtitle)
							memcpy(pasted_slide->subtitle, info_slide->subtitle, info_slide->subtitle_length);
                        pasted_slide->font_desc =
                                pango_font_description_copy( info_slide->font_desc );

                        position_path = gtk_tree_row_reference_get_path(position_rawref);
                        /* position_path can not be NULL as we only copy, without delete */
                        assert (position_path != NULL);
                        pos = gtk_tree_path_get_indices(position_path)[0]+1;
                        gtk_tree_path_free(position_path);

                        gtk_list_store_insert_with_values(
                                GTK_LIST_STORE( model ), &iter, pos,
                                                        0, thumb,
                                                        1, pasted_slide,
                                                        2, trans,
                                                        3, has_sub,
                                                        -1 );
                    /* Let's update the total number of slides and the label in toolbar */
                    img->slides_nr++;
                    }
                }
                gtk_tree_path_free(path);

		}
        node = node->next;

	}
    }

    /* Free rowref_list */
    GList *node1;
    for(node1 = rowref_list;node1 != NULL;node1 = node1->next) {
	gtk_tree_row_reference_free(node1->data);
    }
    g_list_free(rowref_list);
    gtk_tree_row_reference_free(position_rawref);

	/* Free the GList containing the paths of the selected slides */
	if (img->selected_paths)
	{
		GList *node2;
		for(node2 = img->selected_paths;node2 != NULL;node2 = node2->next) {
        gtk_tree_path_free(node2->data);
		}
		g_list_free (img->selected_paths);
		img->selected_paths = NULL;
	}
	/* Free the GList containing the paths where to paste */
	GList *node3;
	for(node3 = where_to_paste;node3 != NULL;node3 = node3->next) {
		gtk_tree_path_free(node3->data);
	}
	g_list_free (where_to_paste);

	/* Update display */
	img_set_statusbar_message(img, 0);

	/* Free the GTK selection structure */
	gtk_selection_data_free (selection);
}

static void img_clear_audio_files(GtkButton * UNUSED(button), img_window_struct *img)
{
	gtk_list_store_clear(GTK_LIST_STORE(img->music_file_liststore));
    img_play_stop_selected_file(NULL, img);
	gtk_widget_set_sensitive(img->play_audio_button, FALSE);
	gtk_widget_set_sensitive(img->remove_audio_button, FALSE);
	gtk_label_set_text(GTK_LABEL(img->music_time_data), "");
	img_taint_project(img);
}

static gboolean img_sub_textview_focus_in(GtkWidget * UNUSED(widget), GdkEventFocus * UNUSED(event), img_window_struct *img)
{
	gtk_widget_remove_accelerator (img->select_all_menu, img->accel_group, GDK_KEY_a, GDK_CONTROL_MASK);
	gtk_widget_remove_accelerator (img->remove_menu,     img->accel_group, GDK_KEY_Delete, 0);
	gtk_widget_remove_accelerator (img->fullscreen_music_preview,     img->accel_group, GDK_KEY_space, 0);
	return FALSE;
}


static gboolean img_sub_textview_focus_out(GtkWidget * UNUSED(widget), GdkEventFocus * UNUSED(event), img_window_struct *img)
{
	gtk_widget_add_accelerator (img->select_all_menu,"activate", img->accel_group, GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator (img->remove_menu,    "activate", img->accel_group, GDK_KEY_Delete, 0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator (img->fullscreen_music_preview,    "activate", img->accel_group, GDK_KEY_space, 0, GTK_ACCEL_VISIBLE);
	return FALSE;
}

static void img_on_drag_audio_data_received (GtkWidget *
	UNUSED(widget),GdkDragContext *context,int UNUSED(x), int UNUSED(y),
	GtkSelectionData *data,unsigned int  UNUSED(info), unsigned int time,
	img_window_struct *img)
{
	gchar **audio = NULL;
	gchar *filename,*ttime;
	GtkWidget *dialog;
	gint len = 0;

	audio = gtk_selection_data_get_uris(data);
	if (audio == NULL)
	{
		dialog = gtk_message_dialog_new(GTK_WINDOW(img->imagination_window),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,_("Sorry, I could not perform the operation!"));
		gtk_window_set_title(GTK_WINDOW(dialog),"Imagination");
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (GTK_WIDGET (dialog));
		gtk_drag_finish(context,FALSE,FALSE,time);
		return;
	}
	gtk_drag_finish (context,TRUE,FALSE,time);
	while(audio[len])
	{
		filename = g_filename_from_uri (audio[len],NULL,NULL);
		img_add_audio_files(filename,img);
		g_free(filename);
		len++;
	}
	ttime = img_convert_seconds_to_time(img->total_music_secs);
	gtk_label_set_text(GTK_LABEL(img->music_time_data), ttime);
	g_free(ttime);
	g_strfreev (audio);
}

static void img_activate_remove_button_music_liststore(GtkTreeModel * UNUSED(tree_model), GtkTreePath * UNUSED(path), GtkTreeIter *iter, img_window_struct *img)
{
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(img->music_file_liststore), iter) == TRUE)
	{
		gtk_widget_set_sensitive ( img->remove_audio_button, TRUE);
		gtk_widget_set_sensitive ( img->play_audio_button, TRUE);
	}
}

static void img_quit_menu(GtkMenuItem * UNUSED(menuitem), img_window_struct *img)
{
	if( ! img_quit_application( NULL, NULL, img ) )
		gtk_main_quit();
}

void img_iconview_selection_changed(GtkIconView *iconview, img_window_struct *img)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint dummy, nr_selected = 0;
	GList *selected = NULL;
	gchar *slide_info_msg = NULL, *selected_slide_nr = NULL;
	slide_struct *info_slide;

	if (img->export_is_running)
		return;

	model = gtk_icon_view_get_model(iconview);

	selected = gtk_icon_view_get_selected_items(iconview);
	nr_selected = g_list_length(selected);
	img_set_total_slideshow_duration(img);

	if (selected == NULL)
	{
		img_set_statusbar_message(img,nr_selected);
		if( img->current_image )
		{
			cairo_surface_destroy( img->current_image );
			img->current_image = NULL;
		}
		img->current_slide = NULL;
		gtk_widget_queue_draw( img->image_area );

		/* Disable slide settings */
        img_disable_videotab(img);

		if (img->slides_nr == 0)
			gtk_label_set_text(GTK_LABEL (img->total_time_data),"");

		gtk_widget_set_sensitive(img->edit_empty_slide, FALSE);
		return;
	}
	gtk_widget_set_sensitive(img->trans_duration,	TRUE);
	gtk_widget_set_sensitive(img->duration,			TRUE);
	gtk_widget_set_sensitive(img->transition_type,	TRUE);
	gtk_widget_set_sensitive(img->random_button,	TRUE);

	img->cur_nr_of_selected_slide = gtk_tree_path_get_indices(selected->data)[0]+1;
	selected_slide_nr = g_strdup_printf("%d",img->cur_nr_of_selected_slide);
	gtk_entry_set_text(GTK_ENTRY(img->slide_number_entry),selected_slide_nr);
	g_free(selected_slide_nr);

	gtk_tree_model_get_iter(model,&iter,selected->data);
	GList *node4;
	for(node4 = selected;node4 != NULL;node4 = node4->next) {
		gtk_tree_path_free(node4->data);
	}
	g_list_free (selected);
	gtk_tree_model_get(model,&iter,1,&info_slide,-1);
	img->current_slide = info_slide;

	/* Set the transition type */
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(img->transition_type));

	/* Block "changed" signal from model to avoid rewriting the same value back into current slide. */
	g_signal_handlers_block_by_func(img->transition_type, (gpointer)img_combo_box_transition_type_changed, img);
	{
		GtkTreeIter   iter;
		GtkTreeModel *model;

		model = gtk_combo_box_get_model( GTK_COMBO_BOX( img->transition_type ) );
		gtk_tree_model_get_iter_from_string( model, &iter, info_slide->path );
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(img->transition_type), &iter );
	}
	g_signal_handlers_unblock_by_func(img->transition_type, (gpointer)img_combo_box_transition_type_changed, img);

	/* Moved this piece of code below the setting the transition, since we
	 * get false negatives in certain situations (eg.: if the previously
	 * selected transition doesn't have transition renderer set): */
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(img->transition_type)) == 0)
		gtk_widget_set_sensitive(img->trans_duration,FALSE);
	else
		gtk_widget_set_sensitive(img->trans_duration,TRUE);

	/* Set the transition speed */
	if (info_slide->speed == FAST)
		dummy = 0;
	else if (info_slide->speed == NORMAL)
		dummy = 1;
	else
		dummy = 2;
	g_signal_handlers_block_by_func((gpointer)img->duration, (gpointer)img_combo_box_speed_changed, img);
	gtk_combo_box_set_active(GTK_COMBO_BOX(img->trans_duration),dummy);
	g_signal_handlers_block_by_func((gpointer)img->duration, (gpointer)img_combo_box_speed_changed, img);

	/* Set the transition duration */
	g_signal_handlers_block_by_func((gpointer)img->duration, (gpointer)img_spinbutton_value_changed, img);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(img->duration), info_slide->duration);
	g_signal_handlers_unblock_by_func((gpointer)img->duration, (gpointer)img_spinbutton_value_changed, img);

	/* Update Ken Burns display */
	img_update_stop_display( img, TRUE );

	/* Update subtitle widgets */
	img_update_subtitles_widgets( img );

	/* Enable/disable Edit empty slide menu item */
	if (info_slide->original_filename == NULL)
		gtk_widget_set_sensitive(img->edit_empty_slide, TRUE);
	else
		gtk_widget_set_sensitive(img->edit_empty_slide, FALSE);
	
	if (nr_selected > 1)
	{
		img_set_statusbar_message(img,nr_selected);
		img_ken_burns_update_sensitivity( img, FALSE, 0 );
		img_subtitle_update_sensitivity( img, 2 );
	}
	else
	{
		if (FALSE == info_slide->load_ok)
        {
            slide_info_msg = g_strdup_printf(_("File '%s' not found"), info_slide->original_filename);
            gtk_statusbar_push(GTK_STATUSBAR (img->statusbar), img->context_id, slide_info_msg);
            g_free (slide_info_msg);
            img_disable_videotab(img);
        }
        else
        {
            if (info_slide->o_filename != NULL)
            {
                slide_info_msg = g_strdup_printf("%s    %s: %s    %s: %s",info_slide->o_filename, _("Resolution"), info_slide->resolution, _("Type"), info_slide->type ? info_slide->type : "?");
                gtk_statusbar_push(GTK_STATUSBAR (img->statusbar), img->context_id, slide_info_msg);
                g_free(slide_info_msg);
            }
            img_ken_burns_update_sensitivity( img, TRUE, info_slide->no_points );
            img_subtitle_update_sensitivity( img, 1 );
        }
	}

	if( img->current_image )
	{
		cairo_surface_destroy( img->current_image );
		img->current_image = NULL;
	}
		
	/* This is not needed when in overview mode, since we're not displaying any
	 * large image preview. */
	if( img->mode == 0 )
	{
		if( ! info_slide->o_filename )
		{
			img_scale_gradient( info_slide->gradient,
								info_slide->g_start_point,
								info_slide->g_stop_point,
								info_slide->g_start_color,
								info_slide->g_stop_color,
								img->video_size[0],
								img->video_size[1], NULL,
								&img->current_image );
		}
		else
			/* Respect quality settings */
			img_scale_image( info_slide->p_filename,
								(gdouble)img->video_size[0] / img->video_size[1],
								0, img->video_size[1], img->distort_images,
								img->background_color, NULL, &img->current_image );
	}
}

void img_combo_box_transition_type_changed (GtkComboBox *combo, img_window_struct *img)
{
	GList        *selected,
				 *bak;
	GtkTreeIter   iter;
	GtkTreeModel *model;
	gpointer      address;
	slide_struct *info_slide;
	gint          transition_id;
	GtkTreePath  *p;
	gchar        *path;
	GdkPixbuf    *pix;

	/* Check if anything is selected and return if nothing is */
	selected = gtk_icon_view_get_selected_items(
					GTK_ICON_VIEW( img->active_icon ) );
	if( selected == NULL )
		return;

	/* Get information about selected transition */
	model = gtk_combo_box_get_model( combo );
	gtk_combo_box_get_active_iter( combo, &iter );
	gtk_tree_model_get( model, &iter, 0, &pix,
									  2, &address,
									  3, &transition_id,
									  -1 );

	/* If user applied None transition, make duration combo insensitive */
	if( transition_id == -1 )
		gtk_widget_set_sensitive( img->trans_duration, FALSE );
	else
		gtk_widget_set_sensitive( img->trans_duration, TRUE );

	/* Get string representation of the path, which will be
	 * saved inside slide */
	p = gtk_tree_model_get_path( model, &iter );
	path = gtk_tree_path_to_string( p );
	gtk_tree_path_free( p );

	/* Update all selected slides */
	model = GTK_TREE_MODEL( img->thumbnail_model );
	bak = selected;
	while (selected)
	{
		gtk_tree_model_get_iter( model, &iter, selected->data );
		gtk_tree_model_get( model, &iter, 1, &info_slide, -1 );
		gtk_list_store_set( GTK_LIST_STORE( model ), &iter, 2, pix, -1 );
		info_slide->render = (ImgRender)address;
		info_slide->transition_id = transition_id;
		g_free( info_slide->path );
		info_slide->path = g_strdup( path );

		/* If this is first slide, we need to copy transition
		 * to the last pseudo-slide too. */
		if( gtk_tree_path_get_indices( selected->data )[0] == 0 && img->bye_bye_transition)
			img->final_transition.render = (ImgRender)address;

		selected = selected->next;
	}
	g_free( path );
	if( pix )
		g_object_unref( G_OBJECT( pix ) );
	img_taint_project(img);
	img_report_slides_transitions( img );
	img_set_total_slideshow_duration( img );
	GList *node5;
	for(node5 = bak;node5 != NULL;node5 = node5->next) {
		gtk_tree_path_free(node5->data);
	}
	g_list_free( bak );
}

static void img_random_button_clicked(GtkButton * UNUSED(button), img_window_struct *img)
{
	GList        *selected,
				 *bak;
	GtkTreeIter   iter;
	GtkTreeModel *model;
	slide_struct *info_slide;
	GdkPixbuf    *pixbuf;

	model = GTK_TREE_MODEL( img->thumbnail_model );
	selected = gtk_icon_view_get_selected_items(GTK_ICON_VIEW (img->active_icon));
	if (selected == NULL)
		return;

	/* Avoiding GList memory leak. */
	bak = selected;
	while (selected)
	{
		gtk_tree_model_get_iter(model, &iter,selected->data);
		gtk_tree_model_get(model, &iter,1,&info_slide,-1);
		pixbuf = img_set_random_transition(img, info_slide);
		gtk_list_store_set( GTK_LIST_STORE( model ), &iter, 2, pixbuf, -1 );
		if( pixbuf )
			g_object_unref( G_OBJECT( pixbuf ) );

		/* If this is first slide, copy transition to last
		 * pseudo-slide */
		if( gtk_tree_path_get_indices( selected->data )[0] == 0 && img->bye_bye_transition)
			img->final_transition.render = info_slide->render;

		selected = selected->next;
	}
	img_taint_project(img);
	GList *node6;
	for(node6 = bak;node6 != NULL;node6 = node6->next) {
		gtk_tree_path_free(node6->data);
	}
	g_list_free(bak);

	/* This fixes enable/disable issue */
	img_iconview_selection_changed(GTK_ICON_VIEW(img->active_icon), img );
}

static GdkPixbuf *
img_set_random_transition( img_window_struct *img,
						   slide_struct      *info_slide )
{
	gint          nr;
	gint          r1, r2;
	gpointer      address;
	gint          transition_id;
	GtkTreeModel *model;
	GtkTreeIter   iter;
	gchar         path[10];
	GdkPixbuf    *pix;

	/* Get tree store that holds transitions */
	model = gtk_combo_box_get_model( GTK_COMBO_BOX( img->transition_type ) );

	/* Get number of top-levels (categories) and select one */
	nr = gtk_tree_model_iter_n_children( model, NULL );

	/* Fix crash if no modules are loaded */
	if( nr < 2 )
		return( NULL );

	r1 = g_random_int_range( 1, nr );
	g_snprintf( path, sizeof( path ), "%d", r1 );
	gtk_tree_model_get_iter_from_string( model, &iter, path );

	/* Get number of transitions in selected category and select one */
	nr = gtk_tree_model_iter_n_children( model, &iter );
	r2 = g_random_int_range( 0, nr );
	g_snprintf( path, sizeof( path ), "%d:%d", r1, r2 );
	gtk_tree_model_get_iter_from_string( model, &iter, path );

	gtk_tree_model_get( model, &iter, 0, &pix, 2, &address, 3, &transition_id, -1 );
	info_slide->transition_id = transition_id;
	info_slide->render = (ImgRender)address;

	/* Prevent leak here */
	if( info_slide->path )
		g_free( info_slide->path );
	info_slide->path = g_strdup( path );

	/* Select proper iter in transition model */
	g_signal_handlers_block_by_func((gpointer)img->transition_type, (gpointer)img_combo_box_transition_type_changed, img);	
	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(img->transition_type), &iter);
	/* Update the slide dialog report in real time */
	img_report_slides_transitions(img);
	g_signal_handlers_unblock_by_func((gpointer)img->transition_type, (gpointer)img_combo_box_transition_type_changed, img);	

	return( pix );
}

static void img_combo_box_speed_changed (GtkComboBox *combo, img_window_struct *img)
{
	gint speed;
	gdouble duration;
	GList *selected, *bak;
	GtkTreeIter iter;
	GtkTreeModel *model;
	slide_struct *info_slide;

	model = GTK_TREE_MODEL( img->thumbnail_model );
	selected = gtk_icon_view_get_selected_items(GTK_ICON_VIEW (img->active_icon));
	if (selected == NULL)
		return;

	speed = gtk_combo_box_get_active(combo);

	if (speed == 0)
		duration = FAST;
	else if (speed == 1)
		duration = NORMAL;
	else 
		duration = SLOW;

	/* Avoid memory leak */
	bak = selected;
	while (selected)
	{
		gtk_tree_model_get_iter(model, &iter,selected->data);
		gtk_tree_model_get(model, &iter,1,&info_slide,-1);
		info_slide->speed = duration;

		/* If we're modifying first slide, we need to modify
		 * last pseudo-slide too. */
		if( gtk_tree_path_get_indices( selected->data )[0] == 0 && img->bye_bye_transition)
			img->final_transition.speed = duration;

		selected = selected->next;
		img_taint_project(img);
	}
	img_set_total_slideshow_duration(img);

	GList *node7;
	for(node7 = bak;node7 != NULL;node7 = node7->next) {
		gtk_tree_path_free(node7->data);
	}
	g_list_free(bak);
}

static void img_select_all_thumbnails(GtkMenuItem * UNUSED(item), img_window_struct *img)
{
	gtk_icon_view_select_all(GTK_ICON_VIEW (img->active_icon));
}

static void img_unselect_all_thumbnails(GtkMenuItem * UNUSED(item), img_window_struct *img)
{
	gtk_icon_view_unselect_all(GTK_ICON_VIEW (img->active_icon));
}

static void img_goto_line_entry_activate(GtkWidget * UNUSED(entry), img_window_struct *img)
{
	gint slide;
	GtkTreePath *path;

	slide = strtol(gtk_entry_get_text(GTK_ENTRY(img->slide_number_entry)), NULL, 10);
	if (slide > 0 && slide <= img->slides_nr)
	{
		gtk_icon_view_unselect_all(GTK_ICON_VIEW (img->active_icon));
		path = gtk_tree_path_new_from_indices(slide-1,-1);
		gtk_icon_view_set_cursor (GTK_ICON_VIEW (img->active_icon), path, NULL, FALSE);
		gtk_icon_view_select_path (GTK_ICON_VIEW (img->active_icon), path);
		gtk_icon_view_scroll_to_path (GTK_ICON_VIEW (img->active_icon), path, FALSE, 0, 0);
		gtk_tree_path_free (path);
	}
}

static gint img_sort_none_before_other(GtkTreeModel *model,GtkTreeIter *a,GtkTreeIter *b,gpointer UNUSED(data))
{
	gchar *name1, *name2;
	gint i;

	gtk_tree_model_get(model, a, 1, &name1, -1);
	gtk_tree_model_get(model, b, 1, &name2, -1);

	if (strcmp(name1,_("None")) == 0)
		i = -1;
	else if (strcmp(name2,_("None")) == 0)
		i = 1;
	else
		i = (g_strcmp0 (name1,name2));

	g_free(name1);
	g_free(name2);
	return i;	
}

static void img_check_numeric_entry (GtkEditable *entry, gchar *text, gint UNUSED(length), gint * UNUSED(position), gpointer  UNUSED(data))
{
	if(*text < '0' || *text > '9')
		g_signal_stop_emission_by_name( (gpointer)entry, "insert-text" );
}

/*
 * img_iconview_selection_button_press:
 *
 * This is a temporary hack that should do the job of unselecting slides if
 * single slide should be selected after select all.
 */
static gboolean
img_iconview_selection_button_press( GtkWidget         *widget,
									 GdkEventButton    *button,
									 img_window_struct * UNUSED(img) )
{
	if( ( button->button == 1 ) &&
		! ( button->state & ( GDK_SHIFT_MASK | GDK_CONTROL_MASK ) ) )
		gtk_icon_view_unselect_all( GTK_ICON_VIEW( widget ) );

	return( FALSE );
}

static gboolean
img_scroll_thumb( GtkWidget         *widget,
				  GdkEventScroll    *scroll,
				  img_window_struct * UNUSED(img) )
{
	GtkAdjustment *adj;
	gdouble        page, step, upper, value, offset;

	adj = gtk_scrolled_window_get_hadjustment( GTK_SCROLLED_WINDOW( widget ) );

	page  = gtk_adjustment_get_page_size( adj );
	step  = gtk_adjustment_get_step_increment( adj );
	upper = gtk_adjustment_get_upper( adj );
	value = gtk_adjustment_get_value( adj );


	switch (scroll->direction) {
	    case GDK_SCROLL_UP:
	    case GDK_SCROLL_LEFT:
		offset = - step;
		break;
	    case GDK_SCROLL_DOWN:
	    case GDK_SCROLL_RIGHT:
		offset = + step;
		break;
	    case GDK_SCROLL_SMOOTH:
		/* one of the ->delta is 0 depending on the direction */
		offset = step * (scroll->delta_x + scroll->delta_y);
	}

	gtk_adjustment_set_value( adj, CLAMP( value + offset, 0, upper - page ) );
	return( TRUE );
}

static void img_show_uri(GtkMenuItem * UNUSED(menuitem), img_window_struct *img)
{
	gchar *file = NULL;
	gchar *lang = NULL;
	
	lang = g_strndup(g_getenv("LANG"),2);
	file = g_strconcat("file://",DATADIR,"/doc/",PACKAGE,"/html/",lang,"/index.html",NULL);
	g_free(lang);
	img_message (img, FALSE, "%s\n", file);

	/* If help is not localized yet, show default language (english) */
	if ( !gtk_show_uri_on_window(GTK_WINDOW(img->imagination_window), file, GDK_CURRENT_TIME, NULL))
	{
		g_free( file );
		file = g_strconcat("file://",DATADIR,"/doc/",PACKAGE,"/html/en/index.html",NULL);
		gtk_show_uri_on_window(GTK_WINDOW(img->imagination_window), file, GDK_CURRENT_TIME, NULL);
	}
	g_free(file);
}

void
img_queue_subtitle_update( GtkTextBuffer     * UNUSED(buffer),
						   img_window_struct *img )
{
	/* This queue enables us to avoid sensless copying and redrawing when typing
	 * relatively fast (limit is cca. 3 keypresses per second) */
	if( img->subtitle_update_id )
		g_source_remove( img->subtitle_update_id );

	img_taint_project(img);

	img->subtitle_update_id =
			g_timeout_add( 300, (GSourceFunc)img_subtitle_update, img );
}

static GtkWidget *
img_create_subtitle_animation_combo( void )
{
	GtkWidget       *combo;
	GtkListStore    *store;
	TextAnimation   *animations;
	gint             no_anims;
	register gint    i;
	GtkTreeIter      iter;
	GtkCellRenderer *cell;

	store = gtk_list_store_new( 3, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT );

	no_anims = img_get_text_animation_list( &animations );
	for( i = 0; i < no_anims; i++ )
	{
		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 0, animations[i].name,
										  1, animations[i].func,
										  2, animations[i].id,
										  -1 );
	}
	img_free_text_animation_list( no_anims, animations );

	combo = gtk_combo_box_new_with_model( GTK_TREE_MODEL( store ) );
	g_object_unref( G_OBJECT( store ) );

	cell = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( combo ), cell, TRUE );
	gtk_cell_layout_add_attribute( GTK_CELL_LAYOUT( combo ), cell, "text", 0 );

	return( combo );
}

static gboolean img_subtitle_update( img_window_struct *img )
{
	gboolean     has_subtitle;
	GtkTreeIter	 iter;  
	GList       *list;

	if( img->current_slide->subtitle )
	{
		g_free( img->current_slide->subtitle );
		img->current_slide->subtitle = NULL;
	}
	has_subtitle = 1 < gtk_text_buffer_get_char_count( img->slide_text_buffer );
	if( has_subtitle )
	{	
		img_store_rtf_buffer_content(img);
	}
	list = gtk_icon_view_get_selected_items(
				GTK_ICON_VIEW( img->active_icon ) );
	gtk_tree_model_get_iter( GTK_TREE_MODEL( img->thumbnail_model ),
							 &iter, list->data );
	GList *node8;
	for(node8 = list;node8 != NULL;node8 = node8->next) {
		gtk_tree_path_free(node8->data);
	}
	g_list_free( list );
	gtk_list_store_set( GTK_LIST_STORE( img->thumbnail_model ), &iter,
						3, has_subtitle, -1 );

	/* Queue redraw */
	gtk_widget_queue_draw( img->image_area );

	/* Set source id to zero and remove itself from main context */
	img->subtitle_update_id = 0;

	return( FALSE );
}

void
img_text_font_set( GtkFontButton     *button,
				   img_window_struct *img )
{
	const gchar *string;
	
	string = gtk_font_button_get_font_name( button );

	img_update_sub_properties( img, NULL, -1, -1, string, NULL, NULL, NULL, NULL,
								img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);

	gtk_widget_queue_draw( img->image_area );
	img_taint_project(img);
}

void
img_text_anim_set( GtkComboBox       *combo,
				   img_window_struct *img )
{
	GtkTreeModel      *model;
	GtkTreeIter        iter;
	TextAnimationFunc  anim;
	gint               anim_id;

	model = gtk_combo_box_get_model( combo );
	gtk_combo_box_get_active_iter( combo, &iter );
	gtk_tree_model_get( model, &iter, 1, &anim, 2, &anim_id, -1 );

	img_update_sub_properties( img, anim, anim_id, -1, NULL, NULL, NULL, NULL, NULL, 
								img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);

	/* Speed should be disabled when None is in effect */
	gtk_widget_set_sensitive( img->sub_anim_duration,
							  (gboolean)gtk_combo_box_get_active( combo ) );

	gtk_widget_queue_draw( img->image_area );
	img_taint_project(img);
}

void
img_font_color_changed( GtkColorButton    *button,
						img_window_struct *img )
{
	GdkColor color;
	guint16  alpha;
	gchar	 *rgb;
	gdouble  font_color[4];
	gboolean 	selection;
	GtkTextIter start, end;
	GtkTextTag 	*tag;

	gtk_color_button_get_color( button, &color );
	alpha = gtk_color_button_get_alpha( button  );
	
	selection = gtk_text_buffer_get_selection_bounds(img->slide_text_buffer, &start, &end);
	if (selection > 0)
	{
		rgb = gdk_color_to_string(&color);
		tag = gtk_text_tag_table_lookup(img->tag_table, "foreground");
		g_object_set(tag, "foreground", rgb, NULL);
		g_free(rgb);
		gtk_text_buffer_apply_tag(img->slide_text_buffer, tag, &start, &end);
		img_store_rtf_buffer_content(img);
	}
	else
	{
		font_color[0] = (gdouble)color.red   / 0xffff;
		font_color[1] = (gdouble)color.green / 0xffff;
		font_color[2] = (gdouble)color.blue  / 0xffff;
		font_color[3] = (gdouble)alpha       / 0xffff;
 		img_update_sub_properties( img, NULL, -1, -1, NULL, font_color, NULL, NULL, NULL,
								img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);
	}
	gtk_widget_queue_draw( img->image_area );
	img_taint_project(img);
}

void
img_font_brdr_color_changed( GtkColorButton    *button,
                          img_window_struct *img )
{
    GdkColor color;
    guint16  alpha;
    gdouble  font_brdr_color[4];

    gtk_color_button_get_color( button, &color );
    alpha = gtk_color_button_get_alpha( button  );

	font_brdr_color[0] = (gdouble)color.red   / 0xffff;
	font_brdr_color[1] = (gdouble)color.green / 0xffff;
	font_brdr_color[2] = (gdouble)color.blue  / 0xffff;
	font_brdr_color[3] = (gdouble)alpha       / 0xffff;
   	img_update_sub_properties( img, NULL, -1, -1, NULL, NULL, font_brdr_color, NULL, NULL,
							img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);

	gtk_widget_queue_draw( img->image_area );
	img_taint_project(img);
}

void
img_font_bg_color_changed( GtkColorButton    *button,
                          img_window_struct *img )
{
    GdkColor color;
    guint16  alpha;
	gchar	 *rgb;
	gdouble  font_bgcolor[4];
	gboolean 	selection;
    GtkTextIter start, end;
	GtkTextTag 	*tag;

    gtk_color_button_get_color( button, &color );
    alpha = gtk_color_button_get_alpha( button  );

	selection = gtk_text_buffer_get_selection_bounds(img->slide_text_buffer, &start, &end);
	if (selection > 0)
	{
		rgb = gdk_color_to_string(&color);
		tag = gtk_text_tag_table_lookup(img->tag_table, "background");
		g_object_set(tag, "background", rgb, NULL);
		g_free(rgb);
		gtk_text_buffer_apply_tag(img->slide_text_buffer, tag, &start, &end);
		img_store_rtf_buffer_content(img);
	}
	else
	{
		font_bgcolor[0] = (gdouble)color.red   / 0xffff;
		font_bgcolor[1] = (gdouble)color.green / 0xffff;
		font_bgcolor[2] = (gdouble)color.blue  / 0xffff;
		font_bgcolor[3] = (gdouble)alpha       / 0xffff;
	
		img_update_sub_properties( img, NULL, -1, -1, NULL, NULL, NULL, font_bgcolor, NULL, 
								img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);
	}
    gtk_widget_queue_draw( img->image_area );
    img_taint_project(img);
}

void
img_sub_border_color_changed( GtkColorButton    *button,
                          img_window_struct *img )
{
    GdkColor color;
    guint16  alpha;
    gdouble  font_bgcolor[4];

    gtk_color_button_get_color( button, &color );
    alpha = gtk_color_button_get_alpha( button  );

    font_bgcolor[0] = (gdouble)color.red   / 0xffff;
    font_bgcolor[1] = (gdouble)color.green / 0xffff;
    font_bgcolor[2] = (gdouble)color.blue  / 0xffff;
    font_bgcolor[3] = (gdouble)alpha       / 0xffff;

    img_update_sub_properties( img, NULL, -1, -1, NULL, NULL, NULL, NULL, font_bgcolor, 
								img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);
    img_taint_project(img);

    gtk_widget_queue_draw( img->image_area );
}

void
img_combo_box_anim_speed_changed( GtkSpinButton       *spinbutton,
								  img_window_struct *img )
{
	gint speed;

	speed = gtk_spin_button_get_value_as_int(spinbutton);
	img_update_sub_properties( img, NULL, -1, speed, NULL, NULL, NULL, NULL, NULL, 
								img->current_slide->top_border, img->current_slide->bottom_border, img->current_slide->alignment, -1);
	img_taint_project(img);
}

void img_sub_border_width_changed( GtkSpinButton       *spinbutton,
								  img_window_struct *img )
{
	gint width;

	width = gtk_spin_button_get_value_as_int(spinbutton);
	img_update_sub_properties( img, NULL, -1, -1, NULL, NULL, NULL, NULL, NULL, 
								img->current_slide->top_border, img->current_slide->bottom_border, -1, width);

	gtk_widget_queue_draw( img->image_area );
	img_taint_project(img);
}

void
img_disable_videotab (img_window_struct *img)
{
    gtk_widget_set_sensitive(img->random_button, FALSE);
    gtk_widget_set_sensitive(img->transition_type, FALSE);
    gtk_widget_set_sensitive(img->duration, FALSE);
    gtk_widget_set_sensitive(img->trans_duration,   FALSE);

    img_ken_burns_update_sensitivity (img, FALSE, 0);
    img_subtitle_update_sensitivity (img, 0);
}


void
img_ken_burns_update_sensitivity( img_window_struct *img,
								  gboolean           slide_selected,
								  gint               no_points )
{
	/* Modes of operation:
	 *   3 - disable all
	 *   2 - enable duration, zoom and add
	 *   1 - disable only navigation
	 *   0 - enable all
	 */
	gint mode = 3;

	if( slide_selected )
	{
		switch( no_points )
		{
			case 0:
				mode = 2;
				break;

			case 1:
				mode = 1;
				break;

			default:
				mode = 0;
				break;
		}
	}

	/* Disable all - this is starting state */
	gtk_widget_set_sensitive( img->ken_left,     FALSE );
	gtk_widget_set_sensitive( img->ken_entry,    FALSE );
	gtk_widget_set_sensitive( img->ken_right,    FALSE );
	gtk_widget_set_sensitive( img->ken_duration, FALSE );
	gtk_widget_set_sensitive( img->ken_zoom,     FALSE );
	gtk_widget_set_sensitive( img->ken_add,      FALSE );
	gtk_widget_set_sensitive( img->ken_remove,   FALSE );

	/* Enabler */
	switch( mode ) /* THIS SWITCH IS IN FALL-THROUGH MODE!! */
	{
		case 0: /* Enable all */
			gtk_widget_set_sensitive( img->ken_left,     TRUE );
			gtk_widget_set_sensitive( img->ken_entry,    TRUE );
			gtk_widget_set_sensitive( img->ken_right,    TRUE );
			// fall through
		case 1: /* Disable navigation only */
			gtk_widget_set_sensitive( img->ken_remove,   TRUE );
			// fall through
		case 2: /* Only adding is enabled */
			gtk_widget_set_sensitive( img->ken_add,      TRUE );
			gtk_widget_set_sensitive( img->ken_zoom,     TRUE );
			gtk_widget_set_sensitive( img->ken_duration, TRUE );
			// fall through
		case 3: /* Disable all */
			break;
	}
}

void
img_subtitle_update_sensitivity( img_window_struct *img,
								 gint               mode )
{
	/* Modes:
	 *  0 - disable all
	 *  1 - enable all
	 *  2 - enable all but text field
	 */

	/* Text view is special, since it cannot handle multiple slides */
	gtk_widget_set_sensitive( img->sub_textview,
							  ( mode == 2 ? FALSE : (gboolean)mode ) );

	/* Let's delete the textbuffer when no slide is selected */
	if( mode == 0 || mode == 2 )
	{
		g_signal_handlers_block_by_func( (gpointer)img->slide_text_buffer,
										 (gpointer)img_queue_subtitle_update,
										 img );
		g_object_set( G_OBJECT( img->slide_text_buffer ), "text", "", NULL );
		g_signal_handlers_unblock_by_func( (gpointer)img->slide_text_buffer,
										   (gpointer)img_queue_subtitle_update,
										   img );
	}

	/* Animation duration is also special, since it shoudl be disabled when None
	 * animation is selected. */
	if( gtk_combo_box_get_active( GTK_COMBO_BOX( img->sub_anim ) ) && mode )
		gtk_widget_set_sensitive( img->sub_anim_duration, TRUE );
	else
		gtk_widget_set_sensitive( img->sub_anim_duration, FALSE );

	gtk_widget_set_sensitive( img->sub_font,    (gboolean)mode );

	if ( img->current_slide && img->current_slide->pattern_filename)
	{
		gtk_widget_set_sensitive( img->sub_color,  FALSE );
		gtk_widget_set_tooltip_text( img->sub_color, _("Please delete the pattern to choose the color") );
	}
	else
		gtk_widget_set_sensitive( img->sub_color,   (gboolean)mode );

    gtk_widget_set_sensitive( img->sub_brdr_color, (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_bgcolor, (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_border_color, (gboolean)mode );
	gtk_widget_set_sensitive( img->pattern_image, (gboolean)mode );
	gtk_widget_set_sensitive( img->border_top, (gboolean)mode );
	gtk_widget_set_sensitive( img->border_bottom, (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_border_width, (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_anim,    (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_posX,     (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_posY,     (gboolean)mode );
	gtk_widget_set_sensitive( img->sub_angle,    (gboolean)mode );
	gtk_widget_set_sensitive( img->x_justify,    (gboolean)mode );
	gtk_widget_set_sensitive( img->y_justify,    (gboolean)mode );
	gtk_widget_set_sensitive( img->reset_angle,    (gboolean)mode );
}

void
img_update_sub_properties( img_window_struct *img,
						   TextAnimationFunc  UNUSED(anim),
						   gint               anim_id,
						   gdouble            anim_duration,
						   const gchar       *desc,
						   gdouble           *color,
						   gdouble           *brdr_color,
						   gdouble           *bg_color,
						   gdouble           *border_color,
						   gboolean			top_border,
						   gboolean			bottom_border,
						   gint	             border_width,
						   gint	             alignment)
{
	GList        *selected,
				 *tmp;
	GtkTreeIter   iter;
	GtkTreeModel *model;

	/* Get all selected slides */
	selected = gtk_icon_view_get_selected_items(
					GTK_ICON_VIEW( img->active_icon ) );
	if( ! selected )
		return;

	model = GTK_TREE_MODEL( img->thumbnail_model );

	for( tmp = selected; tmp; tmp = g_list_next( tmp ) )
	{
		slide_struct *slide;

		gtk_tree_model_get_iter( model, &iter, (GtkTreePath *)tmp->data );
		gtk_tree_model_get( model, &iter, 1, &slide, -1 );
		img_set_slide_text_info( slide, NULL, NULL, NULL, img->current_slide->pattern_filename,
								 anim_id, anim_duration,	img->current_slide->posX,
															img->current_slide->posY, 
															img->current_slide->subtitle_angle,
								 desc, color, brdr_color, bg_color, border_color, top_border, bottom_border, border_width, alignment, img );
	}

	GList *node9;
	for(node9 = selected;node9 != NULL;node9 = node9->next) {
		gtk_tree_path_free(node9->data);
	}
	g_list_free( selected );
}

static void
img_toggle_mode( GtkCheckMenuItem  *item,
				 img_window_struct *img )
{
	gint mode;
	mode = ( gtk_check_menu_item_get_active( item ) ? 0 : 1 );

	img_switch_mode( img, mode );
}

void
img_switch_mode( img_window_struct *img,
				 gint               mode )
{
	GtkIconView *from,      /* Iconviews for selection synchronization */
				*to;
	GList       *selection, /* Selection to be copied from 'from' to 'to' */
				*tmp;       /* Iterator */

	if( img->mode == mode )
		return;

	img->mode = mode;

	gtk_widget_hide( img->active_icon );
	switch( mode )
	{
		case 0: /* Preview mode */
			gtk_widget_hide( img->over_root );
			gtk_widget_show( img->prev_root );
			gtk_widget_show( img->thum_root );
			img->active_icon = img->thumbnail_iconview;
			from = GTK_ICON_VIEW( img->over_icon );
			break;

		case 1: /* Overview mode */
			gtk_widget_hide( img->prev_root );
			gtk_widget_hide( img->thum_root );
			gtk_widget_show( img->over_root );
			img->active_icon = img->over_icon;
			from = GTK_ICON_VIEW( img->thumbnail_iconview );
			break;
		default: /* Impossible */
			g_assert(FALSE);
			return;
	}
	gtk_widget_show( img->active_icon );
	to = GTK_ICON_VIEW( img->active_icon );

	/* Synchronize selections */
	selection = gtk_icon_view_get_selected_items( from );
	if( ! selection )
	{
		gtk_icon_view_unselect_all( to );
		return;
	}

	g_signal_handlers_block_by_func( from,
									 img_iconview_selection_changed, img );
	g_signal_handlers_block_by_func( to,
									 img_iconview_selection_changed, img );

	gtk_icon_view_unselect_all( to );
	for( tmp = g_list_next( selection ); tmp; tmp = g_list_next( tmp ) )
	{
		GtkTreePath *path = (GtkTreePath *)tmp->data;
		gtk_icon_view_select_path( to, path );
		gtk_tree_path_free( path );
	}

	g_signal_handlers_unblock_by_func( from,
									   img_iconview_selection_changed, img );
	g_signal_handlers_unblock_by_func( to,
									   img_iconview_selection_changed, img );

	gtk_icon_view_select_path( to, (GtkTreePath *)selection->data );
	gtk_icon_view_set_cursor( to, (GtkTreePath *)selection->data, NULL, FALSE );
	gtk_tree_path_free( (GtkTreePath *)selection->data );

	g_list_free( selection );
}

static void img_report_slides_transitions(img_window_struct *img)
{
	static GtkWidget *viewport;
	GtkWidget        *label;
	GHashTable       *trans_hash;
	GList            *values,
					 *tmp;
	GtkTreeModel     *model;
	GtkTreeIter       iter;
	gboolean          flag;
	gint              i;

#define GIP( val ) GINT_TO_POINTER( ( val ) )
#define GPI( val ) GPOINTER_TO_INT( ( val ) )

	if (img->report_dialog == NULL)
	{
		GtkWidget *vbox, *swindow;

		img->report_dialog = gtk_dialog_new_with_buttons(
					_("Slides Transitions Report Dialog"),
					GTK_WINDOW( img->imagination_window ),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					"_Close", GTK_RESPONSE_ACCEPT,
					NULL );
		gtk_container_set_border_width( GTK_CONTAINER( img->report_dialog ),
										10 );		
		gtk_window_set_default_size( GTK_WINDOW( img->report_dialog ),
									 480, 370 );
		gtk_window_set_modal( GTK_WINDOW( img->report_dialog ), FALSE );

		g_signal_connect( G_OBJECT( img->report_dialog ), "delete-event",
						  G_CALLBACK( gtk_widget_hide_on_delete ), NULL );
		g_signal_connect( G_OBJECT( img->report_dialog ), "response",
						  G_CALLBACK( gtk_widget_hide_on_delete ), NULL );

		vbox = gtk_dialog_get_content_area( GTK_DIALOG( img->report_dialog ) );
		swindow = gtk_scrolled_window_new( NULL, NULL );
		gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swindow ),
										GTK_POLICY_AUTOMATIC,
										GTK_POLICY_AUTOMATIC);
		gtk_box_pack_start( GTK_BOX( vbox ), swindow, TRUE, TRUE, 0 );

		viewport = gtk_viewport_new( NULL, NULL );
		gtk_viewport_set_shadow_type( GTK_VIEWPORT( viewport ),
									  GTK_SHADOW_NONE);
		gtk_container_add( GTK_CONTAINER( swindow ), viewport );
	}

	/* Delete previous shown rows */
	if( img->vbox_slide_report_rows )
	{
		gtk_widget_destroy( img->vbox_slide_report_rows );
		img->vbox_slide_report_rows = NULL;
	}

	model = GTK_TREE_MODEL(img->thumbnail_model);
	if( gtk_tree_model_get_iter_first( model, &iter ) == 0)
		return;

	/* Hash table is used only for quick way of accessing transition info.
	 * Information is stored inside array of 3 gpointers */
	trans_hash = g_hash_table_new( g_direct_hash, NULL );

	for( flag = TRUE, i = 0;
		 flag;
		 flag = gtk_tree_model_iter_next( model, &iter ), i++ )
	{
		slide_struct *slide;
		gpointer     *info;

		gtk_tree_model_get( model, &iter, 1, &slide, -1 );
		if( slide->transition_id < 1 )
			continue;

		info = g_hash_table_lookup( trans_hash, GIP( slide->transition_id ) );
		if( ! info )
		{
			/* Create new info element */
			info = g_slice_alloc0( sizeof( gpointer ) * 3 );
			info[0] = GIP( slide->transition_id );
			g_hash_table_insert( trans_hash, GIP( slide->transition_id ),
											 info );
		}

		/* Increment counter */
		info[1] = GIP( GPI( info[1] ) + 1 );

		/* Append another element to glist */
		info[2] = g_list_append( (GList *)info[2], GIP( i ) );
	}

	/* Set the vertical box container that was previously
	 * destroyed so to allow update in real time */
	img->vbox_slide_report_rows = gtk_box_new(GTK_ORIENTATION_VERTICAL,
						  15);
	gtk_container_add( GTK_CONTAINER( viewport ), img->vbox_slide_report_rows );

	label = gtk_label_new( _("\n<span weight='bold'>Note:</span>\n\n"
							 "Slides whose transition is applied only once are "
							 "not shown here.\n"
							 "Click on the slide to have Imagination "
							 "automatically select it." ) );
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(label), 0.5);
	gtk_label_set_use_markup( GTK_LABEL( label ), TRUE );
	gtk_box_pack_start( GTK_BOX( img->vbox_slide_report_rows ), label,
						FALSE, FALSE, 0);

	/* Get information and free hash table */
	values = g_hash_table_get_values( trans_hash );
	g_hash_table_destroy( trans_hash );

	/* Sort values list here */
	values = g_list_sort( values, img_sort_report_transitions );

	/* Display results */
	for( tmp = values; tmp; tmp = g_list_next( tmp ) )
	{
		gpointer *info = tmp->data;

		if( GPI( info[1] ) > 1 )
		{
			GList     *tmp1;
			GtkWidget *hbox_rows,
					  *frame,
					  *image,
					  *nr_label;
			gchar     *filename,
					  *nr;

			hbox_rows = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						15);
			gtk_box_pack_start( GTK_BOX( img->vbox_slide_report_rows ),
								hbox_rows, FALSE, FALSE, 0 );

			frame = gtk_frame_new( NULL );
			gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_NONE );
			gtk_box_pack_start( GTK_BOX( hbox_rows ), frame, FALSE, FALSE, 0 );

#if PLUGINS_INSTALLED
			filename =
				g_strdup_printf( "%s/imagination/pixmaps/imagination-%d.png",
								 DATADIR, GPI( info[0] ) );
#else /* PLUGINS_INSTALLED */
			filename =
				g_strdup_printf( "./pixmaps/imagination-%d.png",
								 GPI( info[0] ) );
#endif
			image = gtk_image_new_from_file( filename );
			g_free( filename );
			gtk_container_add( GTK_CONTAINER( frame ), image );

			nr = g_strdup_printf( "(%d)", GPI( info[1] ) );
			nr_label = gtk_label_new( nr );
			gtk_box_pack_start( GTK_BOX( hbox_rows ), nr_label,
								FALSE, FALSE, 0 );
			g_free( nr );

			for( tmp1 = (GList *)info[2]; tmp1; tmp1 = g_list_next( tmp1 ) )
			{
				GtkWidget   *button,
							*image;
				GdkPixbuf   *pixbuf;
				GtkTreePath *path;
				GtkTreeIter  iter;

				path = gtk_tree_path_new_from_indices( GPI( tmp1->data ), -1 );
				gtk_tree_model_get_iter( model, &iter, path );
				gtk_tree_path_free( path );

				gtk_tree_model_get( model, &iter, 0, &pixbuf, -1 );

				button = gtk_button_new();
				g_object_set_data( G_OBJECT( button ), "index", tmp1->data );
				g_signal_connect( G_OBJECT( button ), "clicked",
								  G_CALLBACK( img_select_slide_from_slide_report_dialog ), img );
				gtk_box_pack_start( GTK_BOX( hbox_rows ), button,
									FALSE, FALSE, 0 );

				image = gtk_image_new_from_pixbuf( pixbuf );
				g_object_unref( G_OBJECT( pixbuf ) );
				gtk_container_add( GTK_CONTAINER( button ), image );
			}
		}
		g_list_free( (GList *)info[2] );
		g_slice_free1( sizeof( gpointer ) * 3, info );
	}

	if( gtk_widget_get_visible( img->report_dialog ) )
		gtk_widget_show_all( img->report_dialog );

#undef GIP
#undef GPI
}

static void img_select_slide_from_slide_report_dialog(GtkButton *button, img_window_struct *img)
{
	GtkTreePath *path;
	gint slide = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "index"));

	gtk_icon_view_unselect_all(GTK_ICON_VIEW (img->active_icon));
	path = gtk_tree_path_new_from_indices(slide, -1);
	gtk_icon_view_set_cursor (GTK_ICON_VIEW (img->active_icon), path, NULL, FALSE);
	gtk_icon_view_select_path (GTK_ICON_VIEW (img->active_icon), path);
	gtk_icon_view_scroll_to_path (GTK_ICON_VIEW (img->active_icon), path, FALSE, 0, 0);
	gtk_tree_path_free (path);
}

static void img_show_slides_report_dialog(GtkMenuItem * UNUSED(item), img_window_struct *img)
{
	img_report_slides_transitions(img);
	gtk_widget_show_all(img->report_dialog);
}

static gint
img_sort_report_transitions( gconstpointer a,
							 gconstpointer b )
{
	gint val_a = GPOINTER_TO_INT( ( (gpointer *)a )[1] ),
		 val_b = GPOINTER_TO_INT( ( (gpointer *)b )[1] );

	return( val_a - val_b );
}

static void
img_toggle_frame_rate( GtkCheckMenuItem  *item,
					   img_window_struct *img )
{
	gpointer tmp;

	if( ! gtk_check_menu_item_get_active( item ) )
		return;

	tmp = g_object_get_data( G_OBJECT( item ), "index" );
	img->preview_fps = GPOINTER_TO_INT( tmp );
}


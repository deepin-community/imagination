/*
 *  Copyright (C) 2009-2018 Giuseppe Torelli <colossus73@gmail.com>
 *  Copyright (c) 2009 Tadej Borovšak 	<tadeboro@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#include "slideshow_project.h"
#include "video_formats.h"

static gboolean img_populate_hash_table( GtkTreeModel *, GtkTreePath *, GtkTreeIter *, GHashTable ** );

void
img_save_slideshow( img_window_struct *img,
					const gchar       *output,
					gboolean		  relative )
{
	GKeyFile *img_key_file;
	gchar *conf, *path, *filename, *file, *font_desc;
	gint count = 0;
	gsize len;
	GtkTreeIter iter;
	slide_struct *entry;
	GtkTreeModel *model;

	model = GTK_TREE_MODEL( img->thumbnail_model );
	if (!gtk_tree_model_get_iter_first (model, &iter))
		return;

	img_key_file = g_key_file_new();

	/* Slideshow settings */
	g_key_file_set_comment(img_key_file, NULL, NULL, comment_string, NULL);

    g_key_file_set_string(img_key_file, "slideshow settings",
                          "video codec",
                          video_format_list[img->video_format_index].config_name);

                          g_key_file_set_integer(img_key_file, "slideshow settings",
                          "video width", img->video_size[0]);
    g_key_file_set_integer(img_key_file, "slideshow settings",
                          "video height", img->video_size[1]);

    g_key_file_set_string(img_key_file, "slideshow settings",
                "fps",
                video_format_list[img->video_format_index].fps_list[img->fps_index].name);

    if (NULL != video_format_list[img->video_format_index].aspect_ratio_list)
        g_key_file_set_string(img_key_file, "slideshow settings",
                "aspect ratio",
                video_format_list[img->video_format_index].aspect_ratio_list[img->aspect_ratio_index].name);

    if (NULL != video_format_list[img->video_format_index].bitratelist)
		g_key_file_set_string(img_key_file, "slideshow settings", "bitrate",video_format_list[img->video_format_index].bitratelist[img->bitrate_index].value);

	/* Save last slide setting (bye bye transition) */
	g_key_file_set_boolean( img_key_file, "slideshow settings",
										 "blank slide", img->bye_bye_transition);

	g_key_file_set_double_list( img_key_file, "slideshow settings",
								"background color", img->background_color, 3 );
	g_key_file_set_boolean(img_key_file,"slideshow settings", "distort images", img->distort_images);

	g_key_file_set_integer(img_key_file, "slideshow settings", "number of slides", img->slides_nr);

	/* Slide settings */
	do
	{
		count++;
		gtk_tree_model_get(model, &iter,1,&entry,-1);
		conf = g_strdup_printf("slide %d",count);

        if (entry->load_ok)
            filename = entry->o_filename;
        else
            filename = entry->original_filename;

		if (relative && filename)
		{
			gchar *_filename;
			_filename = g_path_get_basename(filename);
			filename = _filename;
		}
        if (filename)
		{
			/* Save original filename and rotation */
			g_key_file_set_string( img_key_file, conf,
								   "filename", filename);
			g_key_file_set_integer( img_key_file, conf, "angle", entry->angle );
		}
		else
		{
			/* We are dealing with an empty slide */
			gdouble *start_color = entry->g_start_color,
					*stop_color  = entry->g_stop_color,
					*start_point = entry->g_start_point,
					*stop_point  = entry->g_stop_point;

			g_key_file_set_integer(img_key_file, conf, "gradient",	entry->gradient);
			g_key_file_set_double_list(img_key_file, conf, "start_color", start_color, 3 );
			g_key_file_set_double_list(img_key_file, conf, "stop_color" , stop_color , 3 );
			g_key_file_set_double_list(img_key_file, conf, "start_point", start_point, 2 );
			g_key_file_set_double_list(img_key_file, conf, "stop_point" , stop_point , 2 );
		}

		/* Duration */
		g_key_file_set_double(img_key_file,conf, "duration",		entry->duration);

		/* Flipped horizontally */
		g_key_file_set_boolean(img_key_file,conf, "flipped",	entry->flipped);

		/* Transition */
		g_key_file_set_integer(img_key_file,conf, "transition_id",	entry->transition_id);
		g_key_file_set_integer(img_key_file,conf, "speed", 			entry->speed);

		/* Stop points */
		g_key_file_set_integer(img_key_file,conf, "no_points",		entry->no_points);
		if (entry->no_points > 0)
		{
			gint    point_counter;
			gdouble my_points[entry->no_points * 4];

			for( point_counter = 0;
				 point_counter < entry->no_points;
				 point_counter++ )
			{
				ImgStopPoint *my_point = g_list_nth_data(entry->points,point_counter);
				my_points[ (point_counter * 4) + 0] = (gdouble)my_point->time;
				my_points[ (point_counter * 4) + 1] = my_point->offx;
				my_points[ (point_counter * 4) + 2] = my_point->offy;
				my_points[ (point_counter * 4) + 3] = my_point->zoom;
			}
			g_key_file_set_double_list(img_key_file,conf, "points", my_points, (gsize) entry->no_points * 4);
		}

		/* Subtitle */
		if( entry->subtitle )
		{
			entry->subtitle[26] =  32;
			entry->subtitle[27] =  32;
			entry->subtitle[28] =  32;
			entry->subtitle[29] =  32;

			g_key_file_set_string (img_key_file, conf,"text", (gchar*)entry->subtitle);
			g_key_file_set_integer(img_key_file,conf, "text length", entry->subtitle_length - 30);

			entry->subtitle[26] = (entry->subtitle_length >> 24) & 0xFF;
			entry->subtitle[27] = (entry->subtitle_length >> 16) & 0xFF;
			entry->subtitle[28] = (entry->subtitle_length >> 8) & 0xFF;
			entry->subtitle[29] = entry->subtitle_length & 0xFF;

			if( entry->pattern_filename )
			{
				if (relative)
				{
					gchar *dummy;
					dummy = g_path_get_basename(entry->pattern_filename);
					g_key_file_set_string (img_key_file, conf,"pattern filename", dummy);
					g_free(dummy);
				}
				else
					g_key_file_set_string (img_key_file, conf,"pattern filename", entry->pattern_filename);
			}
			font_desc = pango_font_description_to_string(entry->font_desc);
			g_key_file_set_integer(img_key_file,conf, "anim id",		entry->anim_id);
			g_key_file_set_integer(img_key_file,conf, "anim duration",	entry->anim_duration);
			g_key_file_set_integer(img_key_file,conf, "posX",		entry->posX);
			g_key_file_set_integer(img_key_file,conf, "posY",		entry->posY);
			g_key_file_set_integer(img_key_file,conf, "subtitle angle",		entry->subtitle_angle);
			g_key_file_set_string (img_key_file, conf,"font",			font_desc);
			g_key_file_set_double_list(img_key_file, conf,"font color",entry->font_color,4);
			g_key_file_set_double_list(img_key_file, conf,"font bgcolor",entry->font_brdr_color,4);
			g_key_file_set_double_list(img_key_file, conf,"font bgcolor2",entry->font_bg_color,4);
			g_key_file_set_double_list(img_key_file, conf,"border color",entry->border_color,3);
			g_key_file_set_boolean(img_key_file, conf,"top border",entry->top_border);
			g_key_file_set_boolean(img_key_file, conf,"bottom border",entry->bottom_border);
			g_key_file_set_integer(img_key_file, conf,"border width",entry->border_width);
			g_key_file_set_integer(img_key_file, conf,"alignment",entry->alignment);
			g_free(font_desc);
		}
			g_free(conf);
	}
	while (gtk_tree_model_iter_next (model,&iter));
	count = 0;

	/* Background music */
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(img->music_file_treeview));
	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		g_key_file_set_integer(img_key_file, "music", "fadeout duration", img->audio_fadeout);
		g_key_file_set_integer(img_key_file, "music", "number", gtk_tree_model_iter_n_children(model, NULL));
		do
		{
			count++;
			gtk_tree_model_get(model, &iter, 0, &path, 1, &filename ,-1);
			conf = g_strdup_printf("music_%d",count);
			if (! relative)
			{
				file = g_build_filename(path, filename, NULL);
				g_free(path);
				g_free(filename);
			}
			else
				file = filename;

			g_key_file_set_string(img_key_file, "music", conf, file);
			g_free(file);
			g_free(conf);
		}
		while (gtk_tree_model_iter_next (model, &iter));
	}

	/* Write the project file */
	conf = g_key_file_to_data(img_key_file, &len, NULL);
	g_file_set_contents( output, conf, len, NULL );
	g_free (conf);

	g_key_file_free(img_key_file);

	if( img->project_filename )
		g_free( img->project_filename );
	img->project_filename = g_strdup( output );
	img->project_is_modified = FALSE;
	img_refresh_window_title(img);
}

void
img_append_slides_from( img_window_struct *img, const gchar *input )
{
	GdkPixbuf *thumb;
	slide_struct *slide_info;
	GtkTreeIter iter;
	GKeyFile *img_key_file;
	gchar *dummy, *slide_filename, *time;
	GtkWidget *dialog;
	gint number,i, n_invalid, transition_id, no_points, previous_nr_of_slides, border_width, alignment;
	guint speed;
	GtkTreeModel *model;
	void (*render);
	GHashTable *table;
	gchar      *spath, *conf, *project_current_dir;
	gdouble    duration, *color, *font_color, *font_brdr_color, *font_bg_color, *border_color;
	gboolean   first_slide = TRUE;
    gchar      *video_config_name, *aspect_ratio, *fps;
    gchar      *bitrate;

	/* Cretate new key file */
	img_key_file = g_key_file_new();
	if( ! g_key_file_load_from_file( img_key_file, input,
									 G_KEY_FILE_KEEP_COMMENTS, NULL ) )
	{
		g_key_file_free( img_key_file );
		return;
	}

	/* Are we able to load this project? */
	dummy = g_key_file_get_comment( img_key_file, NULL, NULL, NULL);

	if( strncmp( dummy, comment_string, strlen( comment_string ) ) != 0 )
	{
		dialog = gtk_message_dialog_new(
				GTK_WINDOW( img->imagination_window ), GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
				_("This is not an Imagination project file!") );
		gtk_window_set_title( GTK_WINDOW( dialog ), "Imagination" );
		gtk_dialog_run( GTK_DIALOG( dialog ) );
		gtk_widget_destroy( GTK_WIDGET( dialog ) );
		g_free( dummy );
		return;
	}
	g_free( dummy );

	project_current_dir = g_path_get_dirname(input);

	/* Create hash table for efficient searching */
	table = g_hash_table_new_full( g_direct_hash, g_direct_equal,
								   NULL, g_free );
	model = gtk_combo_box_get_model( GTK_COMBO_BOX( img->transition_type ) );
	gtk_tree_model_foreach( model,
							(GtkTreeModelForeachFunc)img_populate_hash_table,
							&table );

	/* Set the slideshow options */
   /* Video Codec */
	video_config_name = g_key_file_get_string(img_key_file, "slideshow settings",
                          "video codec", NULL);
	i = 0;
	while (video_config_name != NULL
          && video_format_list[i].name != NULL
          && strcmp (video_format_list[i].config_name, video_config_name) != 0)
		i++;
	if (video_config_name == NULL || video_format_list[i].name == NULL)
	{
		img_message(img, FALSE, "Could not find a video format, guessing VOB\n");
		img->video_format_index = 0; /* index for VOB format*/
	}
	else
      img->video_format_index = i;

	/* Video Size */
	img->video_size[0] = g_key_file_get_integer(img_key_file, "slideshow settings",
                          "video width", NULL);
	img->video_size[1] = g_key_file_get_integer(img_key_file, "slideshow settings",
                          "video height", NULL);

	/* Set the max value of slide subtitles hrange scale
	 * according to the new video size */
	gtk_adjustment_set_upper( img->sub_posX_adj, (gdouble)img->video_size[0]);
	gtk_adjustment_set_upper( img->sub_posY_adj, (gdouble)img->video_size[1]);

	/* fps */
	fps = g_key_file_get_string(img_key_file, "slideshow settings", "fps", NULL);
	i = 0;
	while (fps != NULL
          && strcmp (video_format_list[img->video_format_index].fps_list[i].name, fps) != 0)
      i++;

	if (fps == NULL
      || video_format_list[img->video_format_index].fps_list[i].name == NULL)
	{
		img_message(img, FALSE, "Could not find a fps, set to default\n");
		img->fps_index = 0; /* index for VOB format*/
	}
	else
		img->fps_index = i;
	img->export_fps = video_format_list[img->video_format_index].fps_list[img->fps_index].numerator;
	img->export_fps /= video_format_list[img->video_format_index].fps_list[img->fps_index].denominator;

  /* Aspect ratio */
  if (NULL != video_format_list[img->video_format_index].aspect_ratio_list)
  {
      aspect_ratio = g_key_file_get_string(img_key_file, "slideshow settings",
                              "aspect ratio", NULL);
      i = 0;
      while (aspect_ratio != NULL
             && video_format_list[img->video_format_index].aspect_ratio_list[i].name != NULL
             && strcmp (video_format_list[img->video_format_index].aspect_ratio_list[i].name, aspect_ratio) != 0)
          i++;

      if (aspect_ratio == NULL
          || video_format_list[img->video_format_index].aspect_ratio_list[i].name == NULL)
      {
          img_message(img, FALSE, "Could not find an aspect ratio, set to default\n");
          img->aspect_ratio_index = 0; /* index for VOB format*/
      }
      else
        img->aspect_ratio_index = i;
   }
        
   /* Bitrate */
   if (NULL != video_format_list[img->video_format_index].bitratelist)
   {
      bitrate = g_key_file_get_string(img_key_file, "slideshow settings",
                                    "bitrate", NULL);
      i = 0;
      while (bitrate != NULL
			&& video_format_list[img->video_format_index].bitratelist[i].name != NULL
			&& strcmp (video_format_list[img->video_format_index].bitratelist[i].value, bitrate) != 0)
         i++;
 
      if (video_format_list[img->video_format_index].bitratelist[i].name == NULL)
      {
           img_message(img, FALSE, "Could not find a bitrate, set to default\n");
           img->bitrate_index = 0; /* index for VOB format*/
      }
      else
        img->bitrate_index = i;
    }

	img->video_ratio = (gdouble)img->video_size[0] / img->video_size[1];

    img_zoom_fit(NULL, img);

	/* Make loading more efficient by removing model from icon view */
	g_object_ref( G_OBJECT( img->thumbnail_model ) );
	gtk_icon_view_set_model( GTK_ICON_VIEW( img->thumbnail_iconview ), NULL );
	gtk_icon_view_set_model( GTK_ICON_VIEW( img->over_icon ), NULL );


	gchar *subtitle = NULL, *pattern_name = NULL, *font_desc;
	gdouble *my_points = NULL, *p_start = NULL, *p_stop = NULL, *c_start = NULL, *c_stop = NULL;
	gsize length;
	gint anim_id,anim_duration, posx, posy, gradient = 0, subtitle_length, subtitle_angle;
	GdkPixbuf *pix = NULL;
    gboolean      load_ok, flipped, img_load_ok, top_border, bottom_border;
	gchar *original_filename = NULL;
	GtkIconTheme *icon_theme;
	GtkIconInfo  *icon_info;
	const gchar  *icon_filename;
	ImgAngle   angle = 0;

		/* Load last slide setting (bye bye transition) */
		img->bye_bye_transition = g_key_file_get_boolean( img_key_file, "slideshow settings",
										 "blank slide", NULL);
		
		/* Load project backgroud color */
		color = g_key_file_get_double_list( img_key_file, "slideshow settings",
											"background color", NULL, NULL );
		img->background_color[0] = color[0];
		img->background_color[1] = color[1];
		img->background_color[2] = color[2];
		g_free( color );

		/* Loads the thumbnails and set the slides info */
		number = g_key_file_get_integer( img_key_file, "slideshow settings",
										 "number of slides", NULL);
		/* Store the previous number of slides and set img->slides_nr so to have the correct number of slides displayed on the status bar */
		previous_nr_of_slides = img->slides_nr;
		img->slides_nr = number;

		gtk_widget_show( img->progress_bar );
		n_invalid = 0;
		for( i = 1; i <= number; i++ )
		{
			conf = g_strdup_printf("slide %d", i);
			slide_filename = g_key_file_get_string(img_key_file,conf,"filename", NULL);

			if( slide_filename )
			{
				if ( g_path_is_absolute(slide_filename) == FALSE)
					original_filename = g_strconcat(project_current_dir, "/", slide_filename, NULL);
				else
					original_filename = g_strdup (slide_filename);

				angle = (ImgAngle)g_key_file_get_integer( img_key_file, conf,
														  "angle", NULL );
				load_ok = img_scale_image( original_filename, img->video_ratio,
										   88, 0, img->distort_images,
										   img->background_color, &thumb, NULL );
                img_load_ok = load_ok;
                if (! load_ok)
                {
                    icon_theme = gtk_icon_theme_get_default();
                    icon_info = gtk_icon_theme_lookup_icon(icon_theme,
                                                           "image-missing",
                                                           256,
                                                           GTK_ICON_LOOKUP_FORCE_SVG);
                    icon_filename = gtk_icon_info_get_filename(icon_info);

                    img_message(img, TRUE, _("Slide %i: can't load image %s\n"), i, slide_filename);

                    g_free (slide_filename);
                    slide_filename = g_strdup(icon_filename);
                    load_ok = img_scale_image( slide_filename, img->video_ratio,
                                                88, 0, img->distort_images,
                                                img->background_color, &thumb, NULL );

                }
			}
			else
			{
				angle = 0;
				/* We are loading an empty slide */
				gradient = g_key_file_get_integer(img_key_file, conf, "gradient", NULL);
				c_start = g_key_file_get_double_list(img_key_file, conf, "start_color", NULL, NULL);
				c_stop  = g_key_file_get_double_list(img_key_file, conf, "stop_color", NULL, NULL);
				p_start = g_key_file_get_double_list(img_key_file, conf, "start_point", NULL, NULL);
				p_stop = g_key_file_get_double_list(img_key_file, conf, "stop_point", NULL, NULL);

				/* Create thumbnail */
				load_ok = img_scale_gradient( gradient, p_start, p_stop,
											  c_start, c_stop, 88, 49,
											  &thumb, NULL );
                /* No image is loaded, so img_load_ok is OK if load_ok is */
                img_load_ok = load_ok;
			}

			/* Try to load image. If this fails, skip this slide */
			if( load_ok )
			{
				duration	  = g_key_file_get_double(img_key_file, conf, "duration", NULL);
				flipped		  = g_key_file_get_boolean(img_key_file, conf, "flipped", NULL);
				transition_id = g_key_file_get_integer(img_key_file, conf, "transition_id", NULL);
				speed 		  =	g_key_file_get_integer(img_key_file, conf, "speed",	NULL);

				/* Load the stop points if any */
				no_points	  =	g_key_file_get_integer(img_key_file, conf, "no_points",	NULL);
				if (no_points > 0)
					my_points = g_key_file_get_double_list(img_key_file, conf, "points", &length, NULL);

				/* Load the slide text related data */
				subtitle	  =	g_key_file_get_string (img_key_file, conf, "text",	NULL);
				subtitle_length	  =	g_key_file_get_integer (img_key_file, conf, "text length",	NULL);
				pattern_name  =	g_key_file_get_string (img_key_file, conf, "pattern filename",	NULL);
				anim_id 	  = g_key_file_get_integer(img_key_file, conf, "anim id", 		NULL);
				anim_duration = g_key_file_get_integer(img_key_file, conf, "anim duration",	NULL);
				posx     	  = g_key_file_get_integer(img_key_file, conf, "posX",		NULL);
				posy       	  = g_key_file_get_integer(img_key_file, conf, "posY",		NULL);
				subtitle_angle= g_key_file_get_integer(img_key_file, conf, "subtitle angle",		NULL);
				font_desc     = g_key_file_get_string (img_key_file, conf, "font", 			NULL);
				font_color 	  = g_key_file_get_double_list(img_key_file, conf, "font color", NULL, NULL );
                font_brdr_color  = g_key_file_get_double_list(img_key_file, conf, "font bgcolor", NULL, NULL );
                font_bg_color = g_key_file_get_double_list(img_key_file, conf, "font bgcolor2", NULL, NULL );
                border_color = g_key_file_get_double_list(img_key_file, conf, "border color", NULL, NULL );
                top_border = g_key_file_get_boolean(img_key_file, conf, "top border", NULL);
                bottom_border = g_key_file_get_boolean(img_key_file, conf, "bottom border", NULL);
                border_width = g_key_file_get_integer(img_key_file, conf, "border width", NULL);
                alignment = g_key_file_get_integer(img_key_file, conf, "alignment", NULL);

				/* Get the mem address of the transition */
				spath = (gchar *)g_hash_table_lookup( table, GINT_TO_POINTER( transition_id ) );
				if (spath) {
				  gtk_tree_model_get_iter_from_string( model, &iter, spath );
				  gtk_tree_model_get( model, &iter, 2, &render, 0, &pix, -1 );
				} else {
				  img_message(img, TRUE, _("Slide %d: Unknown transition id %d\n"), i, transition_id);
				  transition_id = -1;
				}

				slide_info = img_create_new_slide();
				if( slide_info )
				{
					if( slide_filename )
						img_set_slide_file_info( slide_info, original_filename );
					else {
						g_return_if_fail(c_start && c_stop && p_start && p_stop);
						img_set_slide_gradient_info( slide_info, gradient,
													 c_start, c_stop,
													 p_start, p_stop );
					}

                    /* Handle load errors */
                    slide_info->load_ok = img_load_ok;
                    slide_info->original_filename = original_filename;

					
					/* If image has been flipped or rotated, do it now too. */
					if( (flipped || angle) && ! g_strrstr(icon_filename,"image-missing"))
					{
						img_rotate_flip_slide( slide_info, angle, flipped, NULL );
						g_object_unref( thumb );
						img_scale_image( slide_info->p_filename, img->video_ratio,
										 88, 0, img->distort_images,
										 img->background_color, &thumb, NULL );
					}
					gtk_list_store_append( img->thumbnail_model, &iter );                   
					gtk_list_store_set( img->thumbnail_model, &iter,
										0, thumb,
										1, slide_info,
										-1 );
					g_object_unref( G_OBJECT( thumb ) );

					/* Set duration */
					img_set_slide_still_info( slide_info, duration, img );

					/* Set transition */
					img_set_slide_transition_info( slide_info,
												   img->thumbnail_model, &iter,
												   pix, spath, transition_id,
												   render, speed, img );

					/* Set stop points */
					if( no_points > 0 )
					{
						img_set_slide_ken_burns_info( slide_info, 0,
													  length, my_points );
						g_free( my_points );
					}

					/* Set subtitle */
					if (subtitle)
					{
						gtk_list_store_set( img->thumbnail_model, &iter, 3, TRUE, -1 );
						if ( pattern_name && g_path_is_absolute(pattern_name) == FALSE)
						{
							gchar *_pattern_filename;
							_pattern_filename = g_strconcat(project_current_dir, "/", pattern_name, NULL);
							g_free(pattern_name);
							pattern_name = _pattern_filename;
						}
						/* Does the slide have a foreground color? */
						img_check_for_rtf_colors(img, subtitle);

						if (strstr((const gchar*)subtitle, "GTKTEXTBUFFERCONTENTS-0001"))
						{
							subtitle[26] = (subtitle_length >> 24);
							subtitle[27] = (subtitle_length >> 16) & 0xFF;
							subtitle[28] = (subtitle_length >> 8) & 0xFF;
							subtitle[29] = subtitle_length & 0xFF;

							slide_info->subtitle_length = subtitle_length + 30;
						}
						slide_info->subtitle = (guint8*)subtitle;

						img_set_slide_text_info( slide_info, img->thumbnail_model,
												 &iter, NULL, pattern_name, anim_id,
												 anim_duration, posx, posy, subtitle_angle,
												 font_desc, font_color, font_brdr_color, font_bg_color, border_color, 
												 top_border, bottom_border, border_width, alignment, img );
					}
					/* If we're loading the first slide, apply some of it's
				 	 * data to final pseudo-slide */
					if( first_slide && img->bye_bye_transition)
					{
						first_slide = FALSE;
						img->final_transition.speed  = slide_info->speed;
						img->final_transition.render = slide_info->render;
					}
				}
				if (pix)
					g_object_unref( G_OBJECT( pix ) );
				g_free( font_desc );
			}
			else
            {
	      n_invalid++;
	      img_message(img, TRUE, _("Can't load image %s\n"), slide_filename);
            }

			img_increase_progressbar(img, i);
			g_free(slide_filename);
			g_free(conf);
		}
	
	img->slides_nr += previous_nr_of_slides - n_invalid;

	img->distort_images = g_key_file_get_boolean( img_key_file,
												  "slideshow settings",
												  "distort images", NULL );
	gtk_widget_hide(img->progress_bar);

	gtk_icon_view_set_model( GTK_ICON_VIEW( img->thumbnail_iconview ),
							 GTK_TREE_MODEL( img->thumbnail_model ) );
	gtk_icon_view_set_model( GTK_ICON_VIEW( img->over_icon ),
							 GTK_TREE_MODEL( img->thumbnail_model ) );
	g_object_unref( G_OBJECT( img->thumbnail_model ) );

	/* Loads the audio files in the liststore */
	img_set_fadeout_duration(img,
	    g_key_file_get_integer(img_key_file, "music", "fadeout duration",
	      NULL));

	number = g_key_file_get_integer(img_key_file, "music", "number", NULL);
	for (i = 1; i <= number; i++)
	{
		dummy = g_strdup_printf("music_%d", i);
		slide_filename = g_key_file_get_string(img_key_file, "music", dummy, NULL);
		if ( g_path_is_absolute(slide_filename) == FALSE)
		{
			gchar *_slide_filename;
			_slide_filename = g_strconcat(project_current_dir, "/", slide_filename, NULL);
			g_free(slide_filename);
			slide_filename = _slide_filename;
		}
		img_add_audio_files(slide_filename, img);

		/* slide_filename is freed in img_add_audio_files */
		g_free(dummy);
	}
	g_key_file_free (img_key_file);
	img_set_total_slideshow_duration(img);

	img_set_statusbar_message(img, 0);

	g_hash_table_destroy( table );

	/* Update incompatibilities display */
	img_update_inc_audio_display( img );

	time = img_convert_seconds_to_time(img->total_music_secs);
	gtk_label_set_text(GTK_LABEL(img->music_time_data), time);
	g_free(time);
	g_free(project_current_dir);

	img->project_is_modified = TRUE;
}

void
img_load_slideshow( img_window_struct *img, const gchar *input ) {
	img_close_slideshow(NULL, img);
	img_append_slides_from(img, input);
	
	/* Select the first slide */
	img_goto_first_slide(NULL, img);
	img->project_is_modified = FALSE;

	/* If we made it to here, we succesfully loaded project, so it's safe to set
	 * filename field in global data structure. */
	if( img->project_filename )
		g_free( img->project_filename );
	img->project_filename = g_strdup( input );

	if (img->project_current_dir)
		g_free(img->project_current_dir);
	img->project_current_dir = g_path_get_dirname(input);

	img_refresh_window_title(img);
}

static gboolean img_populate_hash_table( GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GHashTable **table )
{
	gint         id;

	gtk_tree_model_get( model, iter, 3, &id, -1 );

	/* Leave out family names, since hey don't get saved. */
	if( ! id )
		return( FALSE );

	/* Freeing of this memory is done automatically when the list gets
	 * destroyed, since we supplied destroy notifier handler. */
	g_hash_table_insert( *table, GINT_TO_POINTER( id ), (gpointer)gtk_tree_path_to_string( path ) );

	return( FALSE );
}


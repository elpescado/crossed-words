/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <string.h>

#include <gtk/gtk.h>

#include "sc-move.h"
#include "scx-board-view.h"
#include "scx-painter.h"

#include "common.h"


G_DEFINE_TYPE (ScxBoardView, scx_board_view, GTK_TYPE_DRAWING_AREA)

struct _ScxBoardViewPrivate
{
	/* Private members go here */
	ScBoard  *board;
	gboolean  show_move;
	ScMove    move;

	/* Selection */
	gboolean selection_enabled;
	gboolean has_selection;
	gint     selection_x;
	gint     selection_y;

	GdkGC *tile_gc;
	GdkGC *tile_backgrounds[5];
	ScxPainter *painter;

	gboolean disposed;
};



const int TILE_SIZE = 48;
const int TILE_SPACING = 4;

#define SCX_BOARD_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_BOARD_VIEW, ScxBoardViewPrivate))

static gboolean
scx_board_view_button_pressed (GtkWidget      *widget,
                               GdkEventButton *event,
                               ScxBoardView   *self);



GtkWidget*
scx_board_view_new (void)
{
	ScxBoardView *self = g_object_new (SCX_TYPE_BOARD_VIEW, NULL);

	gtk_widget_set_size_request (GTK_WIDGET (self), 960, 800);

	return GTK_WIDGET (self);
}


static void
scx_board_view_init (ScxBoardView *self)
{
	self->priv = SCX_BOARD_VIEW_GET_PRIVATE (self);
	ScxBoardViewPrivate *priv = self->priv;

	priv->painter = scx_painter_new (GTK_WIDGET (self));

	gtk_widget_add_events (GTK_WIDGET (self), GDK_BUTTON_PRESS_MASK);
	g_signal_connect (self, "button-press-event",
	                  G_CALLBACK (scx_board_view_button_pressed), self);

	priv->disposed = FALSE;
}

void
scx_board_view_set_board (ScxBoardView *self,
                          ScBoard      *board)
{
	ScxBoardViewPrivate *priv = self->priv;
	
	if (priv->board)
		g_object_unref (priv->board);

	priv->board = g_object_ref (board);

	gtk_widget_queue_draw (GTK_WIDGET (self));
}


void
scx_board_view_set_move (ScxBoardView *self,
                        ScMove       *move)
{
	ScxBoardViewPrivate *priv = self->priv;
	
	if (move) {
		priv->show_move = TRUE;
		memcpy (&(priv->move), move, sizeof (ScMove));
	} else {
		priv->show_move = FALSE;
	}

	gtk_widget_queue_draw (GTK_WIDGET (self));
}


void
scx_board_view_enable_selection (ScxBoardView *self)
{
	ScxBoardViewPrivate *priv = self->priv;

	priv->selection_enabled = TRUE;
}


void
scx_board_view_disable_selection (ScxBoardView *self)
{
	ScxBoardViewPrivate *priv = self->priv;

	priv->selection_enabled = FALSE;
	priv->has_selection = FALSE;
}


gboolean
scx_board_view_get_selection (ScxBoardView *self, gint *i, gint *j)
{
	ScxBoardViewPrivate *priv = self->priv;

	if (priv->has_selection) {
		if (i) *i = priv->selection_x;
		if (j) *j = priv->selection_y;
		return TRUE;
	} else {
		return FALSE;
	}
}


void
scx_board_view_set_selection (ScxBoardView *self, gint i, gint j)
{
	ScxBoardViewPrivate *priv = self->priv;
	
	priv->has_selection = TRUE;
	priv->selection_x = i;
	priv->selection_y = j;

	gtk_widget_queue_draw (GTK_WIDGET (self));
}


static gboolean
scx_board_view_button_pressed (GtkWidget      *widget,
                               GdkEventButton *event,
                               ScxBoardView   *self)
{
	ScxBoardViewPrivate *priv = self->priv;

	int board_size = (TILE_SIZE + TILE_SPACING) * 15 - TILE_SPACING;
	int ox = (GTK_WIDGET(self)->allocation.width - board_size) / 2;
	int oy = (GTK_WIDGET(self)->allocation.height - board_size) / 2;

	int x = (int) event->x - ox;
	int y = (int) event->y - oy;


	if (x > 0 && x < board_size && y > 0 && y < board_size) {
		int i = x / (TILE_SIZE + TILE_SPACING);
		int j = y / (TILE_SIZE + TILE_SPACING);

		g_print ("Click!!! @ %d, %d => %d, %d => %d, %d\n", (int)event->x, (int)event->y, x, y, i, j);

		if (priv->selection_enabled)
			scx_board_view_set_selection (self, i, j);
	}

	return TRUE;
}



static void
set_color (GdkGC *gc, const gchar *color_name)
{
	GdkColor color;
	
	gdk_color_parse (color_name, &color);
	gdk_gc_set_rgb_fg_color (gc, &color);
}


static void
scx_board_view_init_gcs (ScxBoardView *self)
{
	ScxBoardViewPrivate *priv = self->priv;
	GdkColor color;
	
	
	priv->tile_gc = gdk_gc_new (gtk_widget_get_window (GTK_WIDGET (self)));
	gdk_color_parse ("#222222", &color);
	gdk_gc_set_rgb_fg_color (priv->tile_gc, &color);
}


static void
scx_board_view_draw_tile (ScxBoardView  *self,
                          Letter        *l,
						  gint           i,
						  gint           j,
						  gint           style_id)
{
	ScxBoardViewPrivate *priv = self->priv;

	int board_size = (TILE_SIZE + TILE_SPACING) * 15 - TILE_SPACING;
	int ox = (GTK_WIDGET(self)->allocation.width - board_size) / 2;
	int oy = (GTK_WIDGET(self)->allocation.height - board_size) / 2;
	int x = ox + i * (TILE_SIZE + TILE_SPACING);
	int y = oy + j * (TILE_SIZE + TILE_SPACING);

	scx_painter_draw_tile (priv->painter, l, x, y, style_id);
}


static gboolean
scx_board_view_expose_event (ScxBoardView        *self,
                             GdkEventExpose      *event)
{
	ScxBoardViewPrivate *priv = self->priv;
	g_print ("Expose!!!\n");
	
	if (priv->board == NULL)
		return TRUE;

	scx_board_view_init_gcs (self);

	gchar *field_colors[] = {
		"#d3d7cf",	/* Normal tile   */
		"#729fcf",  /* Double letter */
		"#204a87",  /* Triple letter */
		"#ef2929",  /* Double word   */
		"#a40000"  /* Triple word   */
	};

	/* Draw board */
	int i;
	int j;
	int board_size = (TILE_SIZE + TILE_SPACING) * 15 - TILE_SPACING;
	int ox = (GTK_WIDGET(self)->allocation.width - board_size) / 2;
	int oy = (GTK_WIDGET(self)->allocation.height - board_size) / 2;

	/* Draw board background */
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			int x = ox + i * (TILE_SIZE + TILE_SPACING);
			int y = oy + j * (TILE_SIZE + TILE_SPACING);

			set_color (priv->tile_gc, "#2e3436");
			gdk_draw_rectangle (gtk_widget_get_window (GTK_WIDGET (self)),
			                    priv->tile_gc,
								FALSE, x, y, TILE_SIZE, TILE_SIZE);

			
			int mod = sc_board_get_field_modifier (priv->board, i, j);
			set_color (priv->tile_gc, field_colors[mod]);
			gdk_draw_rectangle (gtk_widget_get_window (GTK_WIDGET (self)),
			                    priv->tile_gc,
								TRUE, x+1, y+1, TILE_SIZE-1, TILE_SIZE-1);

		}
	}

	/* Draw tiles */
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			int x = ox + i * (TILE_SIZE + TILE_SPACING);
			int y = oy + j * (TILE_SIZE + TILE_SPACING);

			Letter *l = sc_board_get_letter (priv->board, i, j);

			if (l) {
				scx_board_view_draw_tile (self, l, i, j, 0);
			}
		}
	}

	/* Draw move proposal */
	if (priv->show_move) {
		int di = (priv->move.orientation == SC_HORIZONTAL) ? 1 : 0;
		int dj = (priv->move.orientation == SC_HORIZONTAL) ? 0 : 1;
		int k;

		for (k = 0, i = priv->move.x, j = priv->move.y ; k < priv->move.n_letters; k++, i += di, j += dj) {
			int x = ox + i * (TILE_SIZE + TILE_SPACING);
			int y = oy + j * (TILE_SIZE + TILE_SPACING);

			Letter *l = alphabet_lookup_letter (sc_board_get_alphabet (priv->board), priv->move.letters[k]);
			if (l && sc_board_get_letter (priv->board, i, j) == NULL) {
				scx_board_view_draw_tile (self, l, i, j, 1);
			}
		}
	}


	/* Draw selection */
	if (priv->has_selection) {
		int x = ox + priv->selection_x * (TILE_SIZE + TILE_SPACING);
		int y = oy + priv->selection_y * (TILE_SIZE + TILE_SPACING);

		set_color (priv->tile_gc, "#000000");
		gdk_draw_rectangle (gtk_widget_get_window (GTK_WIDGET (self)),
		                    priv->tile_gc,
							FALSE, x-1, y-1, TILE_SIZE+2, TILE_SIZE+2);
		gdk_draw_rectangle (gtk_widget_get_window (GTK_WIDGET (self)),
		                    priv->tile_gc,
							FALSE, x-2, y-2, TILE_SIZE+4, TILE_SIZE+4);

	}



	return TRUE;
}


static void
scx_board_view_dispose (GObject *object)
{
	ScxBoardView *self = (ScxBoardView*) object;
	ScxBoardViewPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_board_view_parent_class)->dispose (object);
}


static void
scx_board_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_board_view_parent_class)->finalize (object);
}

	
static void
scx_board_view_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxBoardView* self = SCX_BOARD_VIEW (object);
	ScxBoardViewPrivate* priv = self->priv;

	G_UNUSED (priv);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_board_view_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxBoardView* self = SCX_BOARD_VIEW (object);
	ScxBoardViewPrivate* priv = self->priv;

	G_UNUSED (priv);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_board_view_class_init (ScxBoardViewClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gobject_class->get_property = scx_board_view_get_property;
	gobject_class->set_property = scx_board_view_set_property;
	gobject_class->dispose = scx_board_view_dispose;
	gobject_class->finalize = scx_board_view_finalize;

	widget_class->expose_event = scx_board_view_expose_event;

	g_type_class_add_private (klass, sizeof (ScxBoardViewPrivate));
}


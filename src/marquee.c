#include <pebble.h>


#define SCREEN_WIDTH   144
#define SCREEN_HEIGHT  168

#define MARQUEE_BOX_WIDTH   1400
#define MARQUEE_BOX_HEIGHT  44

#define ANIMATION_DURATION  10000
#define ANIMATION_DELAY     750


static Window            *window          = NULL ;
static TextLayer         *text_layer      = NULL ;
static PropertyAnimation *prop_animation  = NULL ;
const char               *text            = "I wish I could fit on the screen but I can't, so I have to scroll." ;


static void
destroy_property_animation
( PropertyAnimation *prop_animation )
{
  if (prop_animation == NULL)  return;

  if (animation_is_scheduled( &prop_animation->animation ))
    animation_unschedule( &prop_animation->animation );

  property_animation_destroy( prop_animation );
  prop_animation = NULL;
}


void
window_load
( Window *window )
{ GRect marquee_box = GRect( 0, 0, MARQUEE_BOX_WIDTH, MARQUEE_BOX_HEIGHT ) ;
  GFont font = fonts_get_system_font( FONT_KEY_BITHAM_42_BOLD ) ;

  text_layer = text_layer_create( marquee_box ) ;
  text_layer_set_text( text_layer, text ) ;
  text_layer_set_font( text_layer, font ) ;

  GSize text_size
  = graphics_text_layout_get_content_size( text
                                         , font
                                         , marquee_box
                                         , GTextOverflowModeFill
                                         , GTextAlignmentLeft
                                         )
  ;

  layer_add_child( window_get_root_layer( window )
                 , text_layer_get_layer( text_layer )
                 ) ;

  Layer *layer = text_layer_get_layer( text_layer ) ;

  GRect  from_rect  = GRect( 0                                    // x0
                           , 0                                    // y0
                           , SCREEN_WIDTH                         // w0
                           , text_size.h                          // h0
                           )
  ;

  GRect  to_rect    = GRect( SCREEN_WIDTH - text_size.w           // x1
                           , 0                                    // y1
                           , text_size.w + SCREEN_WIDTH           // w1
                           , text_size.h                          // h1
                           )
  ;

  prop_animation = property_animation_create_layer_frame( layer
                                                        , &from_rect
                                                        , &to_rect
                                                        ) ;

  struct Animation *animation = &prop_animation->animation ;

  animation_set_duration( animation, ANIMATION_DURATION ) ;
  animation_set_delay( animation, ANIMATION_DELAY ) ;
  animation_set_curve( animation, AnimationCurveLinear ) ;
  animation_schedule( animation ) ;
}

void
window_unload
( Window *window )
{ destroy_property_animation( prop_animation ) ;
  text_layer_destroy( text_layer ) ;
}


void
init
( void )
{	window = window_create( ) ;

  window_set_window_handlers( window
                            , (WindowHandlers)
                              { .load    = window_load
                              , .unload  = window_unload
                              }
                            ) ;

	window_stack_push( window, false ) ;
}


void
deinit
( void )
{ window_stack_remove( window, false ) ;
  window_destroy( window ) ;
}


int
main
( void )
{	init( ) ;
	app_event_loop( ) ;
	deinit( ) ;
}
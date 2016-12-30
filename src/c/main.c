#include <pebble.h>

static Window *s_main_window;
static Layer *layer;

static GPoint *ellipsePoints;
static GPathInfo *ellipse;
static GPath *my_path = NULL;

static char *line1[] = {
        "sgeorgia", "london",
        "cairo"
    };

static char *line2[] = {
        "rio", "azores",
        "paris", "moscow"
    };

static int16_t fast_hypot(int32_t y, int32_t x){
  if (x == 0 && y==0){
    return 0;
  }
  if (x<0){
    x = -x;
  }
  if (y<0){
    y = -y;
  }
  int32_t alpha = atan2_lookup(y, x);
  if (x>y){
    return x*TRIG_MAX_RATIO/cos_lookup(alpha);
  }
  return y*TRIG_MAX_RATIO/sin_lookup(alpha);
}

static void create_ellipse_points(int16_t a, int16_t b, int16_t num_points, int16_t shift){
  ellipsePoints = malloc(num_points*sizeof *ellipsePoints);
    //int32_t focal_point = fast_hypot()
  for (int16_t i = 0; i<num_points; ++i)  {
    ellipsePoints[i] = GPoint(a*cos_lookup(i*TRIG_MAX_ANGLE/num_points)/TRIG_MAX_RATIO +shift, b*sin_lookup(i*TRIG_MAX_ANGLE/num_points)/TRIG_MAX_RATIO);
  }
  ellipse = malloc(sizeof *ellipse);
  ellipse->num_points = num_points;
  ellipse->points = ellipsePoints;
}

static void draw_ellipse(GContext *ctx, int32_t angle){
  
}

static void my_layer_draw(Layer *layer, GContext *ctx) {
  int16_t corn_rad = 5;
  int16_t offset = 28;
  GRect bounds = layer_get_bounds(layer);
  GRect rect_bound = GRect(bounds.origin.x+offset,bounds.origin.y+offset,bounds.size.w-offset*2,bounds.size.h-offset*2);

  // Draw a black filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_antialiased(ctx, true);
  graphics_draw_round_rect(ctx, rect_bound, corn_rad);
  
  graphics_context_set_text_color(ctx, GColorBlack);
  GSize textBoxSize = GSize(38,10);
  GRect textBounds;
  textBounds.size = textBoxSize;
  //graphics_draw_text(ctx, "Sgeorgia London Cairo \n Rio Azores Paris Moscow", fonts_get_system_font(FONT_KEY_GOTHIC_14), textBounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  //graphics_draw_text(ctx, "Sgeorgia London Cairo \n Rio Azores Paris Moscow", fonts_get_system_font(FONT_KEY_GOTHIC_14), textBounds, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  
  GPoint center = GPoint(bounds.size.w/2,bounds.size.h/2);
  GPoint sector;
  int32_t sector_length = 100;
  
  //for (int16_t i = 0; i<24; ++i){
  //  int32_t angle = TRIG_MAX_ANGLE * i / 24;
  // sector.y = (-cos_lookup(angle) * sector_length / TRIG_MAX_RATIO) + center.y;
  //  sector.x = (sin_lookup(angle) * sector_length / TRIG_MAX_RATIO) + center.x;
  //  graphics_draw_line(ctx,center,sector);
  //}
  
  //int32_t offset = bounds.size.h/2;
  int32_t height_offset = bounds.size.h/2-12;
  for (int16_t i = 0; i<=2; i++){
    int16_t sector_num = (i-1)*2;
    int32_t angle = TRIG_MAX_ANGLE * sector_num / 24;
    textBounds.origin.x = (sin_lookup(angle) * height_offset / cos_lookup(angle)) + center.x - textBounds.size.w/2;
    graphics_draw_text(ctx,line1[i], fonts_get_system_font(FONT_KEY_GOTHIC_14), textBounds, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }

  height_offset -= 12;
  textBounds.origin.y = 12;
  for (int16_t i = 0; i<=3; i++){
    int16_t sector_num = (i-1)*2-1;
    //int16_t sector_num = i-3;
    int32_t angle = TRIG_MAX_ANGLE * sector_num / 24;
    textBounds.origin.x = (sin_lookup(angle) * height_offset / cos_lookup(angle)) + center.x - textBounds.size.w/2;
    graphics_draw_text(ctx,line2[i], fonts_get_system_font(FONT_KEY_GOTHIC_14), textBounds, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }

  my_path = gpath_create(ellipse);
  //gpath_rotate_to(s_my_path_ptr, TRIG_MAX_ANGLE / 360 * 15);
  gpath_move_to(my_path, center);
  
  for (int16_t i = 0; i<12; ++i){
    if (rand()%3 == 0){
      gpath_rotate_to(my_path, i*TRIG_MAX_RATIO/12);
      //graphics_context_set_stroke_color(ctx, GColorOrange);
      gpath_draw_outline(ctx, my_path);
    }
  }
  //graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw a white filled circle a radius of half the layer height
  //graphics_context_set_fill_color(ctx, GColorWhite);
  //const int16_t half_h = bounds.size.h / 2;
  //graphics_fill_circle(ctx, GPoint(half_h, half_h), half_h);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(layer, my_layer_draw);
  layer_add_child(window_layer, layer);

}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  layer_destroy(layer);
  //text_layer_destroy(s_text_layer);
}

static void init() {
  // Create main Window
  create_ellipse_points(60, 30, 160, 0);
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
  gpath_destroy(my_path);
  //free(line1);
  //free(line2);
  free(ellipsePoints);
  free(ellipse);
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
#include "plane.h"
#include "config.h"
#include <stdio.h>

static enum plane_control_st_t {
  FLYING,
  EXPLODING_PLANE,
  EXPLOSION_DISPLAY_PLANE,
  DEAD_PLANE
} currentState;

#define PLANE_Y 30
#define PLANE_X_LOCATION_ORIGIN 300
#define PLANE_X1 5
#define PLANE_X2 10
#define PLANE_X3 10
#define PLANE_Y2 10
#define PLANE_Y3 10
#define LEFT_HAND_SIDE_OF_SCREEN 0
#define PLANE_WAIT_TIME 200
#define INTIALIZER 0
///////////////PROTOTYPES/////////////////
static void draw_plane();
bool is_plane_dead();
bool returns_wait_time();

static uint16_t counter = 0;
static bool now_plane_can_be_initialized = false;
static int16_t plane_x1;                //= 0;
static int16_t plane_x2;                //= 0;
static int16_t plane_x3;                //= 0;
static int16_t plane_y1;                //= 0;
static int16_t plane_y2;                //= 0;
static int16_t plane_y3;                //= 0;
static int16_t plane_x_location_origin; //= 0;
// static int16_t plane_y_location_origin = PLANE_Y; //= 30;
display_point_t plane_location;
// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile) {

  plane_x_location_origin = PLANE_X_LOCATION_ORIGIN;

  plane_x1 = plane_x_location_origin - PLANE_X1;
  plane_x2 = plane_x_location_origin + PLANE_X2;
  plane_x3 = plane_x_location_origin + PLANE_X3;

  plane_y1 = PLANE_Y;
  plane_y2 = PLANE_Y - PLANE_Y2;
  plane_y3 = PLANE_Y + PLANE_Y3;

  currentState = FLYING;
}
bool plane_explode_me = false;
// State machine tick function
void plane_tick() {
  /////////TRANSITION SWITCH STATEMENT(MEALY OUTPUTS)//////////
  switch (currentState) {
  case FLYING:
    counter = INTIALIZER;
    ////checks if the explode me for plane is true or not then transitions to
    /// dead state
    if (plane_explode_me == true) {
      display_fillTriangle(plane_x1, plane_y1, plane_x2, plane_y2, plane_x2,
                           plane_y3, DISPLAY_BLACK);
      // printf("plane has exploded\n");
      now_plane_can_be_initialized = false;
      currentState = DEAD_PLANE;
    } else if (plane_x3 <= LEFT_HAND_SIDE_OF_SCREEN) {
      display_fillTriangle(plane_x1, plane_y1, plane_x2, plane_y2, plane_x2,
                           plane_y3, DISPLAY_BLACK);
      now_plane_can_be_initialized = false;
      // printf("Plane has reached the end\n");
      currentState = DEAD_PLANE;
    }
    break;
  case EXPLODING_PLANE:
    break;
  case EXPLOSION_DISPLAY_PLANE:
    break;
  case DEAD_PLANE:

    break;
  default:
    break;
  }

  ////////ACTION SWITCH STATEMENT(MOORE OUTPUTS)//////////////
  switch (currentState) {
  case FLYING:
    // printf("I believe I can fly \n");
    draw_plane();

    break;
  case EXPLODING_PLANE:
    break;
  case EXPLOSION_DISPLAY_PLANE:
    break;
  case DEAD_PLANE:

    plane_explode_me = false;
    // printf("The plane is Dead\n");
    counter++;
    // waits sometime before the plane can be initialized
    if (counter > PLANE_WAIT_TIME) {
      now_plane_can_be_initialized = true;
    }
    // currentState = FLYING;
    break;
  default:
    break;
  }
}

/// draws the plane
static void draw_plane() {
  // printf("I belive I can touch the sky\n");
  display_fillTriangle(plane_x1, plane_y1, plane_x2, plane_y2, plane_x2,
                       plane_y3, DISPLAY_BLACK);
  plane_x_location_origin -= (CONFIG_PLANE_DISTANCE_PER_TICK);

  plane_x1 = plane_x_location_origin - PLANE_X1;
  // printf("Drawing plane_x1:%d\n", plane_x1);
  plane_x2 = plane_x_location_origin + PLANE_X2;
  // printf("Drawing plane_x2:%d\n", plane_x2);
  plane_x3 = plane_x_location_origin + PLANE_X3;
  // printf("Drawing plane_x3:%d\n", plane_x3);
  // plane_y1 = plane_y_location_origin - 10;
  // plane_y2 = plane_y_location_origin;
  // plane_y3 = plane_y_location_origin + 10;
  display_fillTriangle(plane_x1, plane_y1, plane_x2, plane_y2, plane_x2,
                       plane_y3, DISPLAY_WHITE);
}
// Trigger the plane to expode
void plane_explode() { plane_explode_me = true; }

// Get the XY location of the plane
// DOESNT MATTER FOR THE FIRST TEST///////////
display_point_t plane_getXY() {
  plane_location.x = plane_x_location_origin;
  plane_location.y = PLANE_Y;
  return plane_location;
}

/// checks if the plane is dead and returns a boolean
bool is_plane_dead() {
  /// checks the state of the plane
  if (currentState == DEAD_PLANE) {
    return true;
  } else {
    return false;
  }
}
//// returns the wait so that the plane can be initialized
bool returns_wait_time() { return now_plane_can_be_initialized; }
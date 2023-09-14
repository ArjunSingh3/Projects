#include "gameControl.h"
#include "config.h"
#include "missile.h"
#include "plane.h"
#include "touchscreen.h"
#include <stdio.h>

#define HALF 2
#define X_CURSOR_SHOT 30
#define X_CURSOR_SHOT_VALUE 70
#define X_CURSOR_IMPACT 100
#define X_CURSOR_IMPACT_VALUE 150
#define Y_CURSOR 5
#define TEXT_SIZE 1

///////// ARRAY TO KEEP TRACK OF ALL THE 12 MISSILES /////////
missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];
missile_t *enemy_missiles = &(missiles[CONFIG_MAX_PLANE_MISSILES]);
missile_t *player_missiles =
    &(missiles[CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLANE_MISSILES]);
missile_t *plane_missile = &(missiles[0]);

//////////PROTOTYPE/////////////////
bool is_plane_dead();
bool returns_wait_time();
////////KEEPS TRACK OF THE LOCATION////////////
volatile display_point_t point;
volatile display_point_t point_plane;
static int16_t shot_score = 0;
static int16_t impact_score = 0;
static int16_t shots = 0;
static uint16_t plane_missile_counter = 0;
// bool enemy_missile_checked = false;

/////PROTOTYPES FOR THE BOOLEAN FUNCTION//////////////
bool missile_is_in_reach(missile_t *missile1, missile_t *missile2);
bool plane_is_in_reach(missile_t *missile1, display_point_t plane_location);

///// INTIALIZES THE GAME CONTROL
void gameControl_init() {
  /////INITIALIZE ALL THE MISSILES USING THE INIT FUNCTIONS
  for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++)
    missile_init_dead(&missiles[i]);

  display_fillScreen(DISPLAY_BLACK);
  plane_init(plane_missile);
}

//////BOOLEAN TO KEEP TRACK OF MISSIELS BEING TICKED////////
bool tick_half = true;

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick() {
  point_plane = plane_getXY();
  display_setTextSize(TEXT_SIZE);
  display_setTextColor(DISPLAY_BLACK);
  // srand(time(0));
  /////THE TICKS ARE DIVIDED INTO TWO, BECAUSE ALL MISSILES DO NOT HAVE THE
  /// TIME
  /// TO BE TICKED TOGETHER///////////////////////////
  ///////TICKS ALL THE MISSILES/////////////////////////
  if (tick_half) {
    /// Ticks one half of the missiles
    for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES / HALF; i++) {
      missile_tick(&missiles[i]);
    }
  }
  //// TICKS THE SECOND HALF OF THE MISSILES///////
  if (!tick_half) {
    /////ticks the othe rhalf of the missiles
    for (uint16_t i = CONFIG_MAX_TOTAL_MISSILES / HALF;
         i < CONFIG_MAX_TOTAL_MISSILES; i++) {
      missile_tick(&missiles[i]);
      // missile_tick(plane_missile);
    }
  }
  ///////////////////TICKING THE PLANE//////////////////////////////
  plane_tick();
  /////////////////RE-INITIALIZES ALL THE ENEMY
  /// MISSILES/////////////////////////////
  display_setCursor(X_CURSOR_IMPACT_VALUE, Y_CURSOR);
  ///////checks for impact score and re-initializes dead enemy missiles
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
    // cehcks if impacted it true
    if (enemy_missiles[i].impacted) {
      display_printlnDecimalInt(impact_score);
      impact_score++;
      enemy_missiles[i].impacted = false;
    }
    /// cehcks if a missile is dead to be re-initialized
    if (missile_is_dead(&enemy_missiles[i])) {
      missile_init_enemy(&enemy_missiles[i]);
    }
  }
  // checks if plane missile is impacted or not and increments
  if (plane_missile->impacted) {

    // display_setCursor(X_CURSOR_IMPACT_VALUE, Y_CURSOR);
    display_printlnDecimalInt(impact_score);

    impact_score++;
    plane_missile->impacted = false;
  }

  // enemy_missile_checked = !enemy_missile_checked;
  // plane_missile_counter++;
  ////ONLY LAUNCHES THE MISSILE IF PLANE IS FLYING/////////////////
  // checks if plane is dead or alive and also the plane missile/
  if (missile_is_dead(plane_missile) && is_plane_dead() == false) {
    missile_init_plane(&missiles[0], point_plane.x, point_plane.y);
  }

  ///////////////////////////////////////////////////////
  /////////////RE_INITIALIZES THE PLANE IF IT IS DEAD///////////////
  if (is_plane_dead() == true && returns_wait_time()) {

    plane_init(plane_missile);
    plane_tick();
  }

  ////////////////////////////////////////////////////
  /////////////////////////DETECT TOUCH FOR MISSILE
  /// LAUNCH////////////////////////////
  if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
    //// checks if the player missile is dead and goes over the player missiels
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
      /////checks if the player missiel is dead so that it can inncrement the
      /// shot score and re-initialize th eplayer missile
      if (missile_is_dead(&player_missiles[i])) {

        // resets cursors for each message
        display_setCursor(X_CURSOR_SHOT_VALUE, Y_CURSOR);
        display_printlnDecimalInt(shots);
        shots++;
        point = touchscreen_get_location();
        missile_init_player(&player_missiles[i], point.x, point.y);
        break;
      }
    touchscreen_ack_touch();
  }
  //////////////////////////////////////////////////////////////
  ////////////////////DISPLAYING SHOTS TAKEN///////////////////

  display_setTextColor(DISPLAY_WHITE);
  // resets cursors for each message
  if (tick_half) {
    display_setCursor(X_CURSOR_SHOT, Y_CURSOR);
    display_print("SHOTS:");
  }
  display_setCursor(X_CURSOR_SHOT_VALUE, Y_CURSOR);
  display_printlnDecimalInt(shots);
  /////////////PRINTING IMPACTS///////////////////////
  ////// RESETS CURSOR FOR EACH MESSAGE////////////////
  if (!tick_half) {
    display_setCursor(X_CURSOR_IMPACT, Y_CURSOR);
    display_print("IMPACTS:");
  }
  display_setCursor(X_CURSOR_IMPACT_VALUE, Y_CURSOR);
  display_printlnDecimalInt(impact_score);
  // printf("shots: %d\n", shots);
  ///////////////////////////////////////////////////////////////////////
  // plane_init(plane_missile);
  ///// ITERATES OVER THE ENEMY MISSILES AND ALL THE OTHER MISSILES FOR
  /// EXPLODING IN CHAIN////////
  for (uint8_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLANE_MISSILES;
       i++) {
    // checking for all other missiles that are having an explosion
    for (uint8_t j = 0; j < CONFIG_MAX_TOTAL_MISSILES; j++) {

      ////CHECKS FOR IF THE MISSILE IS FLYING OR NOT/////////
      if (!missile_is_flying(&missiles[i])) {
        continue;
      }
      ////CHECKS FOR IF ANY MISSILE IS EXPLODING
      if (!missile_is_exploding(&missiles[j])) {
        continue;
      }
      ////CHECKS IF A MISSILE IS IN REACH OR NOT TO SET EXPLODE-ME TO TRUE////
      if (missile_is_in_reach(&missiles[i], &missiles[j])) {
        missile_trigger_explosion(&missiles[i]);
      }
      ////////CHECKING TO EXPLODE PLANE/////////////////////////

      if (plane_is_in_reach(&missiles[j], point_plane)) {
        // printf("inside exposion true\n");
        plane_explode();
      }
    }
  }
  //// ALTERNATES THE MISSIELS TO BE TICKED/////
  tick_half = !tick_half;
}

/////// BOOLEAN FUNCTION THAT RETURNS TRUE OR FALSE DEPENDING ON IF A
/// FLYING//////////////////////////////////////////////
/// MISSILE IS IN THE EXPLODING ZONE OF AN EXPLODING
/// MISSILE/////////////////////////////////////////////
bool missile_is_in_reach(missile_t *missile1, missile_t *missile2) {

  return (((missile1->y_current - missile2->y_current) *
           (missile1->y_current - missile2->y_current)) +
              ((missile1->x_current - missile2->x_current) *
               (missile1->x_current - missile2->x_current)) <
          ((missile2->radius) * (missile2->radius)));
}

/////// BOOLEAN FUNCTION THAT RETURNS TRUE OR FALSE DEPENDING ON IF A
/// FLYING//////////////////////////////////////////////
/// PLANE IS IN THE EXPLODING ZONE OF AN EXPLODING
/// MISSILE/////////////////////////////////////////////
bool plane_is_in_reach(missile_t *missile1, display_point_t plane_location) {
  // printf("plane X: %d\n", plane_location.x);
  // printf("plane Y: %d\n", plane_location.y);
  // printf("missile X: %d\n", missile1->x_current);
  // printf("missile Y: %d\n", missile1->y_current);
  double x_loc = plane_location.x - missile1->x_current;
  double y_loc = plane_location.y - missile1->y_current;
  double r_dist = missile1->radius;
  // printf("distance to plane: %f\n", (x_loc * x_loc) + (y_loc * y_loc));
  // printf("radius of explosion: %f\n", (r_dist * r_dist));
  return (((x_loc * x_loc) + (y_loc * y_loc)) < (r_dist * r_dist));
}
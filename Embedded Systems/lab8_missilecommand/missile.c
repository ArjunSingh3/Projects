#include "missile.h"
#include "config.h"
#include "display.h"
#include <math.h>
#include <stdio.h>

// All printed messages for states are provided here.
#define INIT_ST_MSG "init state\n"
#define MOVING_ST_MSG "moving\n"
#define EXPLODING_GROWING_ST_MSG "exploding\n"
#define EXPLODING_SHRINKING_ST_MSG "absorbing\n"
#define DEAD_ST_MSG "dead missile\n"
#define ZERO_INIT 0
#define ENEMY_Y_ORIGIN 5
#define SQUARED 2
#define FIRST_PARTITION_SCREEN 120
#define SECOND_PARTITION_SCREEN 240
#define MISSILE_LAUNCHER1 90
#define MISSILE_LAUNCHER2 180
#define MISSILE_LAUNCHER3 270

#define TWICE 2
static void debugStatePrint2(missile_t *missile);
static uint8_t i = ZERO_INIT;
static uint8_t missileNum = ZERO_INIT;
bool was_in_explode_st = true;
static int16_t impacts = 0;
uint16_t missile_plane_counter = 0;

typedef enum {
  INITIALIZING,
  MOVING,
  EXPLODING_GROWING,
  EXPLODING_SHRINKING,
  DEAD
} missile_status_t;
////////// State Machine INIT Functions //////////
// Unlike most state machines that have a single `init` function, our missile
// will have different initializers depending on the missile type.

////////////////////////PROTOTYPES/////////////////////////////////////////////////
void drawLine_missile(missile_t *missile);
void increasing_Circle_missile(missile_t *missile);
void decreasing_Circle_missile(missile_t *missile);
bool return_impacts_score();
/////////////////////////////////////////////////////////////////////////////////
///////////////////MISSILE_INIT_DEAD(POINTER);///////////////////////////////////

// Initialize the missile as a dead missile.  This is useful at the start of the
// game to ensure that player and plane missiles aren't moving before they
// should.
void missile_init_dead(missile_t *missile) {
  /////////////////////////////////////////////////////////////////////////////////////////////////////LOOOOOOOOOOOKED//////DONE////////
  /// AT THIS!!!!!!!!!!!!!!!!!!!/////////////////////
  missile->currentState = DEAD;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////MISSILE_INIT_ENEMY(POINTER);//////////////////////////////////

// Initialize the missile as an enemy missile.  This will randomly choose the
// origin and destination of the missile.  The origin should be somewhere near
// the top of the screen, and the destination should be the very bottom of the
// screen.
void missile_init_enemy(missile_t *missile) {

  missile->type = MISSILE_TYPE_ENEMY;
  ///////SETTING ORIGIN LOCATION/////////////
  missile->x_origin = rand() % DISPLAY_WIDTH;
  missile->y_origin = ENEMY_Y_ORIGIN;
  ///////SETTING DESTINATION LOCATION///////////
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;
  ///////CURRENT STATE///////////////
  missile->radius = ZERO_INIT;
  missile->length = ZERO_INIT;
  missile->total_length =
      sqrt(pow(((missile->y_dest) - (missile->y_origin)), SQUARED) +
           pow(((missile->x_dest) - (missile->x_origin)), SQUARED));
  missile->currentState = INITIALIZING;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////MISSILE_INIT_PLAYER(POINTER);////////////////////////////////

// Initialize the missile as a player missile.  This function takes an (x, y)
// destination of the missile (where the user touched on the touchscreen).  The
// origin should be the closest "firing location" to the destination (there are
// three firing locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest) {
  missile->type = MISSILE_TYPE_PLAYER;
  ///////SETTING ORIGIN LOCATION/////////////

  //////////3 POINTS TO FIRE PLAYER MISSILES//////////////////////////
  if (x_dest < FIRST_PARTITION_SCREEN) {
    missile->x_origin = MISSILE_LAUNCHER1;
  } else if (x_dest > FIRST_PARTITION_SCREEN &&
             x_dest < SECOND_PARTITION_SCREEN) {
    missile->x_origin = MISSILE_LAUNCHER2;
  } else {
    missile->x_origin = MISSILE_LAUNCHER3;
  }

  missile->y_origin = DISPLAY_HEIGHT;
  ///////SETTING DESTINATION LOCATION///////////
  missile->x_dest = x_dest;
  missile->y_dest = y_dest;
  ///////CURRENT STATE///////////////
  missile->radius = ZERO_INIT;
  missile->length = ZERO_INIT;
  missile->total_length =
      sqrt(pow(((missile->y_dest) - (missile->y_origin)), SQUARED) +
           pow(((missile->x_dest) - (missile->x_origin)), SQUARED));
  missile->currentState = INITIALIZING;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////DONT
/// WORRY ABOUT THIS FUNCTION/////////////////////////////////////////////
// Initialize the missile as a plane missile.  This function takes an (x, y)
// location of the plane which will be used as the origin.  The destination can
// be randomly chosed along the bottom of the screen.
void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y) {

  missile->type = MISSILE_TYPE_PLANE;
  ///////SETTING ORIGIN LOCATION/////////////
  missile->x_origin = plane_x;
  missile->y_origin = plane_y;
  ///////SETTING DESTINATION LOCATION///////////
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;
  ///////CURRENT STATE///////////////
  missile->radius = ZERO_INIT;
  missile->length = ZERO_INIT;
  missile->total_length =
      sqrt(pow(((missile->y_dest) - (missile->y_origin)), SQUARED) +
           pow(((missile->x_dest) - (missile->x_origin)), SQUARED));
  missile->currentState = INITIALIZING;
}

////////// State Machine TICK Function //////////
void missile_tick(missile_t *missile) {
  //   debugStatePrint2(missile);
  /////TRANSITION STATES SWITCH STATEMENT/////////////
  switch (missile->currentState) {
  case INITIALIZING:
    // printf("initialized\n");
    missileNum = ZERO_INIT;

    /////MISSILE EXPLODE BOOLEAN////////////
    missile->explode_me = false;
    /////MISSILE CURRENT LOCATION X_VARIABLE_AXES////////
    missile->x_current = missile->x_origin;
    /////MISSILE CURRENT LOCATION Y_VARIABLE_AXES////////
    missile->y_current = missile->y_origin;
    /////MISSILE TOTAL LENGTH CALCULATION//////////////
    //////MISSILE LENGTH////////////////////
    missile->length = ZERO_INIT;
    // printf("length at init = %f\n", missile->length);

    missile->radius = ZERO_INIT;
    /////MISSILE IMPACT BOOLEAN/////////////
    missile->impacted = false;

    missile->currentState = MOVING;

    break;
  case MOVING:
    //////////CHOSSING BETWEEN PLAYER AND ENEMY
    /// MISSILE///////////////////////////////
    if (missile->explode_me == true) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_BLACK);
      missile->radius = ZERO_INIT;
      // missile->explode_me = true;
      missile->impacted = false;
      missile->currentState = EXPLODING_GROWING;

    } else if (missile->length >= missile->total_length &&
               missile->type == MISSILE_TYPE_PLAYER) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_BLACK);
      missile->radius = ZERO_INIT;
      missile->impacted = false;
      // was_in_explode_st = true;
      missile->currentState = EXPLODING_GROWING;
    } else if (missile->length >= missile->total_length) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_BLACK);
      // was_in_explode_st = false;
      missile->impacted = true;
      missile->currentState = DEAD;
    }
    break;
  case EXPLODING_GROWING:
    ///////////////// CHECKING IF THE CIRCLE HAS BEEN FULLY
    /// DRAWN///////////////////////
    if (missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS) {
      missile->currentState = EXPLODING_SHRINKING;
    }
    break;

  case EXPLODING_SHRINKING:
    /////////////////checks if the zero has stopped exloding to be able to go to
    /// dead state///////////////////////////////////////////////
    if (missile->radius <= ZERO_INIT) {
      missile->currentState = DEAD;
      missile->explode_me = false;
      missile->impacted = false;
    }
    break;
  case DEAD:
    /*if (was_in_explode_st == false) {
      impacts++;
    }*/
    break;
  default:
    printf("IN DEFAULT STATE");
    break;
  }

  ///// ACTION STATES SWITCH STATEMENT///////////////
  switch (missile->currentState) {
  case INITIALIZING:

    break;
  case MOVING:
    // printf("missile length: %d\n", missile->length);
    // printf("missile total_length: %d\n", missile->total_length);
    drawLine_missile(missile);
    break;
  case EXPLODING_GROWING:
    ///////DRAWS THE CIRCLE BIGGER AND BIGGER//////////
    increasing_Circle_missile(missile);
    break;
  case EXPLODING_SHRINKING:
    //////DRAWS THE CRICLE SMALLER AND SMALLER/////////////////
    decreasing_Circle_missile(missile);
    break;
  case DEAD:
    //////DEAD/////////

    // was_in_explode_st = true;
    break;

  // Error handling
  default:
    printf("IN DEFAULT STATE ");
    break;
  }
}

// Return whether the given missile is dead.
bool missile_is_dead(missile_t *missile) {
  //////CHECKS IF THE MISSILE IS IN DEAD STATE OR NOT///////////////
  if (missile->currentState == DEAD) {
    return true;
  } else {
    return false;
  }
}

// Return whether the given missile is exploding.  This is needed when detecting
// whether a missile hits another exploding missile.
bool missile_is_exploding(missile_t *missile) {
  ////////CEHCKS IF THE MISSILE IS IN EXPLODING STATE OR
  /// NOT//////////////////////
  if (missile->currentState == EXPLODING_GROWING ||
      missile->currentState == EXPLODING_SHRINKING) {
    return true;
  } else {
    return false;
  }
}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile) {
  /////////CHECKS IF MISSILE IS FLYING OR NOT////////////////
  if (missile->currentState == MOVING) {
    return true;
  } else {
    return false;
  }
}

// Used to indicate that a flying missile should be detonated.  This occurs when
// an enemy or plane missile is located within an explosion zone.
void missile_trigger_explosion(missile_t *missile) {
  // display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
  //                 missile->y_current, DISPLAY_BLACK);
  missile->explode_me = true;
}

////////DRAWS LINE FOR THE MISSILES/////////////////
void drawLine_missile(missile_t *missile) {
  double percentage = 0;
  //////////CHOOSES WHICH MISSILE TYPE IT IS AND DRWAS THE LINE
  /// ACCORDINGLY//////////////////////
  switch (missile->type) {
  case MISSILE_TYPE_ENEMY:

    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, DISPLAY_BLACK);

    ///////UPDATING THE LENGTH///////////////////////
    missile->length += (TWICE * CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK);

    ////////////CALCULATING THE PERCENTAGE///////////////////////////
    percentage = ((missile->length) / (missile->total_length));

    ////////////////CALCULATING THE NEW CURRENT X,Y////////////////////////
    missile->x_current = missile->x_origin +
                         (percentage * (missile->x_dest - missile->x_origin));
    missile->y_current = missile->y_origin +
                         (percentage * (missile->y_dest - missile->y_origin));

    //////DRAWING NEW MISSILE LINE///////////////////
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, CONFIG_COLOR_ENEMY);

    break;
  case MISSILE_TYPE_PLAYER:
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, DISPLAY_BLACK);

    ///////UPDATING THE LENGTH///////////////////////
    missile->length += (TWICE * CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK);

    ////////////CALCULATING THE PERCENTAGE///////////////////////////
    percentage = ((missile->length) / (missile->total_length));

    ////////////////CALCULATING THE NEW CURRENT X,Y////////////////////////
    missile->x_current = missile->x_origin +
                         (percentage * (missile->x_dest - missile->x_origin));
    missile->y_current = missile->y_origin +
                         (percentage * (missile->y_dest - missile->y_origin));

    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, CONFIG_COLOR_PLAYER);
    break;
  case MISSILE_TYPE_PLANE:
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, DISPLAY_BLACK);

    ///////UPDATING THE LENGTH///////////////////////
    missile->length += (TWICE * CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK);

    ////////////CALCULATING THE PERCENTAGE///////////////////////////
    percentage = (missile->length / missile->total_length);

    ////////////////CALCULATING THE NEW CURRENT X,Y////////////////////////
    missile->x_current = missile->x_origin +
                         (percentage * (missile->x_dest - missile->x_origin));
    missile->y_current = missile->y_origin +
                         (percentage * (missile->y_dest - missile->y_origin));

    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, CONFIG_COLOR_PLANE);
    break;
  }
}

//////DRAWS THE CIRCLE FOR EXPLOSION OF MISSILES//////////////////
void increasing_Circle_missile(missile_t *missile) {
  /////CHOOSES WHICH CIRCLE TO DRAW FOR THE RESPECTIVE MISSILE////////////////
  switch (missile->type) {
  case MISSILE_TYPE_ENEMY:
    if (missile->radius < CONFIG_EXPLOSION_MAX_RADIUS) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, CONFIG_COLOR_ENEMY);
      missile->radius += (TWICE * CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK);
    }
    break;
  case MISSILE_TYPE_PLAYER:
    // CHECKS IF THE RADIUS IS GREATER THAN ZERO////////
    if (missile->radius < CONFIG_EXPLOSION_MAX_RADIUS) {
      //   printf("drawing a circle");
      missile->radius += (TWICE * CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK);
      //   printf("radius: %d", missile->radius);
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, CONFIG_COLOR_PLAYER);
    }
    break;
  case MISSILE_TYPE_PLANE:
    // CHECKS IF THE RADIUS IS GREATER THAN ZERO////////
    if (missile->radius < CONFIG_EXPLOSION_MAX_RADIUS) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, CONFIG_COLOR_PLANE);
      missile->radius += (TWICE * CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK);
    }

    break;
  }
}

//////DRAWS THE DECREASING CIRCLE//////////////////////
void decreasing_Circle_missile(missile_t *missile) {
  display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                     DISPLAY_BLACK);
  missile->radius -= (TWICE * CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK);
  /////// CHOOSES THE APPROPRIATE MISILE TO ABSORB THE
  /// EXPLOSION//////////////////////
  switch (missile->type) {
  case MISSILE_TYPE_ENEMY:
    // CHECKS IF THE RADIUS IS GREATER THAN ZERO////////
    if (missile->radius > 0) {

      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, CONFIG_COLOR_ENEMY);
    }
    // display_fillCircle(missile->x_current,
    // missile->y_current,missile->radius, DISPLAY_BLACK);
    // display_fillCircle(missile->x_current,
    // missile->y_current,missile->radius, CONFIG_COLOR_ENEMY);
    break;
  case MISSILE_TYPE_PLAYER:
    // CHECKS IF THE RADIUS IS GREATER THAN ZERO////////
    if (missile->radius > 0) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, CONFIG_COLOR_PLAYER);
    }
    // display_fillCircle(missile->x_current,
    // missile->y_current,missile->radius, DISPLAY_BLACK);
    // display_fillCircle(missile->x_current,
    // missile->y_current,missile->radius, CONFIG_COLOR_PLAYER);
    break;
  case MISSILE_TYPE_PLANE:
    // CHECKS IF THE RADIUS IS GREATER THAN ZERO////////
    if (missile->radius > 0) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, CONFIG_COLOR_PLANE);
    }
    // display_fillCircle(missile->x_current,
    // missile->y_current,missile->radius, DISPLAY_BLACK);
    // display_fillCircle(missile->x_current,
    // missile->y_current,missile->radius, CONFIG_COLOR_PLANE);
    break;
  }
}

bool return_impacts_score() { return was_in_explode_st; }
// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
static void debugStatePrint2(missile_t *missile) {

  static missile_status_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != missile->currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        missile->currentState; // keep track of the last state that you were in.
    switch (missile->currentState) { // This prints messages based upon the
                                     // state that you were in.
    case INITIALIZING:
      // printf(INIT_ST_MSG);
      break;
    case MOVING:
      // printf("length: %f\n", missile->length);
      // printf("total Length: %d\n", missile->total_length);
      // printf(MOVING_ST_MSG);
      // printf("%d",i++);
      break;
    case EXPLODING_GROWING:
      // printf("radius: %f", missile->radius);
      // printf(EXPLODING_GROWING_ST_MSG);
      break;
    case EXPLODING_SHRINKING:
      // printf(EXPLODING_SHRINKING_ST_MSG);
      break;
    case DEAD:
      // printf(DEAD_ST_MSG);
      break;
    }
  }
}
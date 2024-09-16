#include "touchscreen.h"
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define DISPLAY_TIME 0.05
#define WAITING_ST_MSG "in waiting state\n"
#define ADC_SETTLING_MSG "ADC settling state\n"
#define PRESSED_STATE_MSG "touchscreen is pressed\n"

// Prototype for the debugging software state machine
static void debugStatePrint();
// declaring states in a enum class
static enum touchControl_st_t {
  WAITING,
  ADC_SETTLING,
  PRESSED_ST
} currentState;

display_point_t point;
// variables used in the state machine
// adc_settle_ticks is the benchmark that waits for 50ms
static uint8_t adc_settle_ticks = 0;
// adc_timer variable keeps track of how long a touchscreen has been pressed
static uint8_t adc_timer = 0;
// bool variable chanegs depending on if touchscreen is pressed for more than
// 50ms or not
static bool pressed = FALSE;

// FSM initiating function, initializes the state and all the variables
// including
void touchscreen_init(double period_seconds) {
  currentState = WAITING;
  adc_settle_ticks = (DISPLAY_TIME / period_seconds);
}

// FSM for touchscreen
void touchscreen_tick() {

  // debugging state machine is called
  // debugStatePrint();

  // PERFORM STATE UPDATE FIRST
  switch (currentState) {
  case WAITING:
    // checks if display is touched or not
    if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = ADC_SETTLING;
    } else {
      currentState = WAITING;
    }
    break;
  case ADC_SETTLING:
    // checks toucscreen time and transitions if 50ms has passed or not
    if (!display_isTouched()) {
      // printf("display not touched in ADC SETTLING");
      currentState = WAITING;
    } else if (display_isTouched() && (adc_timer == adc_settle_ticks)) {
      // touchscreen_get_location();
      uint8_t z;
      display_getTouchedPoint(&point.x, &point.y, &z);
      // point{x, y} = {point.x, point.y};
      currentState = PRESSED_ST;
      pressed = TRUE;
    } else {
      currentState = ADC_SETTLING;
    }
    break;
  case PRESSED_ST:
    // checks if needs to go to waiting when touchscreen is not touched anymore
    if (!display_isTouched()) {
      currentState = WAITING;
    } else {
      currentState = PRESSED_ST;
    }
    break;
  default:
    printf("transition switch statement problem\n");
    break;
  }

  // PERFORM STATE ACTION NEXT
  switch (currentState) {
  case WAITING:
    adc_timer = 0;
    break;
  case ADC_SETTLING:
    adc_timer += 1;
    break;
  case PRESSED_ST:
    pressed = TRUE;
    break;
  default:
    printf("stuck in output switch statement\n");
    break;
  }
}

// get status function, is dependenty on the FSM_ticks and returns the status to
// outside programs
touchscreen_status_t touchscreen_get_status() {
  switch (currentState) {
  case WAITING:
    // checks if touchscreen pressed was true before or not
    if (pressed == TRUE) {
      return TOUCHSCREEN_RELEASED;
    } else {
      return TOUCHSCREEN_IDLE;
    }
    break;
  case ADC_SETTLING:
    return TOUCHSCREEN_IDLE;
    break;
  case PRESSED_ST:
    return TOUCHSCREEN_PRESSED;
    break;
  default:
    printf("no status returned\n");
    break;
  }
}

// debug state print dependent on FSM_tick
static void debugStatePrint() {
  static enum touchControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case WAITING:
      printf(WAITING_ST_MSG);
      break;
    case ADC_SETTLING:
      printf(ADC_SETTLING_MSG);
      break;
    case PRESSED_ST:
      printf(PRESSED_STATE_MSG);
      break;
    }
  }
}

// acknowledges the touch
void touchscreen_ack_touch() { pressed = FALSE; }

// function gets the location where the touchscreen is touched and returns a
// point with x and y values
display_point_t touchscreen_get_location() {
  /*static uint16_t x, y;
  static uint8_t z;
  // gets the location and stores it in x,y:
  display_getTouchedPoint(&x, &y, &z);
  display_point_t point = {x, y};*/
  return point;
}
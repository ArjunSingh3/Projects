#include "switches.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"

#define INITIAL_INPUT 0
#define TRI_REGISTER_NUMBER 0x04
#define DATA_REGISTER_NUMBER 0x0000
#define ALL_INPUTS 0xFFFF

uint32_t readRegister2(uint32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

void writeRegister2(uint32_t offset, uint32_t value) {
  return Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

void switches_init() {
  leds_init(INITIAL_INPUT);
  writeRegister2(TRI_REGISTER_NUMBER, ALL_INPUTS);
}

uint8_t switches_read() {

  // change unsigned short to uint16_t or something
  unsigned short input_SWITCHES = INITIAL_INPUT;
  input_SWITCHES = readRegister2(DATA_REGISTER_NUMBER);
  uint8_t SWITCH0 = (uint8_t)(input_SWITCHES & SWITCHES_SW0_MASK);
  // or unsigned char BTN0 = (uint8_t)(input & BUTTONS_BTN0_MASK);

  uint8_t SWITCH1 = (uint8_t)(input_SWITCHES & SWITCHES_SW1_MASK);
  // or unsigned char BTN1 = (uint8_t)(input & BUTTONS_BTN1_MASK);

  uint8_t SWITCH2 = (uint8_t)(input_SWITCHES & SWITCHES_SW2_MASK);
  // or unsigned char BTN2 = (uint8_t)(input & BUTTONS_BTN2_MASK);

  uint8_t SWITCH3 = (uint8_t)(input_SWITCHES & SWITCHES_SW3_MASK);
  // or unsigned char BTN3 = (uint8_t)(input & BUTTONS_BTN3_MASK);

  uint8_t switches_UP = SWITCH0 + SWITCH1 + SWITCH2 + SWITCH3;

  return switches_UP;
}

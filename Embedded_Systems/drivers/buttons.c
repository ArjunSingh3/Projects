#include "buttons.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"

#define INITIAL_INPUT 0
#define DATA_REGISTER_NUMBER 0x0000
#define TRI_REGISTER_NUMBER 0x04
#define ALL_INPUTS 0xFFFF

uint32_t readRegister1(uint32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

void writeRegister1(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

void buttons_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);

  writeRegister1(TRI_REGISTER_NUMBER, ALL_INPUTS);
}

uint8_t buttons_read() {

  // change unsigned short to uint16_t or something
  unsigned short input_buttons = INITIAL_INPUT;
  input_buttons = readRegister1(DATA_REGISTER_NUMBER);
  uint8_t BTN0 = (uint8_t)(input_buttons & BUTTONS_BTN0_MASK);
  // or unsigned char BTN0 = (uint8_t)(input & BUTTONS_BTN0_MASK);

  uint8_t BTN1 = (uint8_t)(input_buttons & BUTTONS_BTN1_MASK);
  // or unsigned char BTN1 = (uint8_t)(input & BUTTONS_BTN1_MASK);

  uint8_t BTN2 = (uint8_t)(input_buttons & BUTTONS_BTN2_MASK);
  // or unsigned char BTN2 = (uint8_t)(input & BUTTONS_BTN2_MASK);

  uint8_t BTN3 = (uint8_t)(input_buttons & BUTTONS_BTN3_MASK);
  // or unsigned char BTN3 = (uint8_t)(input & BUTTONS_BTN3_MASK);

  uint8_t buttons_pushed = BTN0 + BTN1 + BTN2 + BTN3;

  return buttons_pushed;
}

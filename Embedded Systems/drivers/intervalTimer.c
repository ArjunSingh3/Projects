#include "intervalTimer.h"
#include "stdio.h"
#include "xil_io.h"
#include "xparameters.h"

#define TCSR0_OFFSET 0x00
#define TCSR1_OFFSET 0x10
#define TLR0_OFFSET 0x04
#define TLR1_OFFSET 0x14
#define TCR0_OFFSET 0x08
#define TCR1_OFFSET 0x18
#define DEFAULT_VALUE 0x00
#define RELOAD_BIT 0x020
#define NOT_VALUE 0x01
#define CASCADE_BIT 0x800
#define SHIFT_BIT 32
#define COUNT_DOWN_ENABLES 0x812
#define INTERVAL_TIMER_START_BIT 0x080
#define ENABLE_INTERRUPT_BIT 0x040
#define ACK_INTERRUPT_BIT 0x100
#define ENT0_BIT 7
#define LOAD0_BIT 5
#define ENIT0_BIT 6
#define array_size

uint64_t frequencyArray[array_size] = {XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ,
                                       XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ,
                                       XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ};

static uint32_t readRegister(uint8_t timerNumber, uint32_t addr) {
  switch (timerNumber) {
  case XPAR_AXI_TIMER_0_DEVICE_ID:
    return Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + addr);
    break;
  case XPAR_AXI_TIMER_1_DEVICE_ID:
    return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + addr);
    break;
  case XPAR_AXI_TIMER_2_DEVICE_ID:
    return Xil_In32(XPAR_AXI_TIMER_2_BASEADDR + addr);
    break;
  }
}

static void writeRegister(uint8_t timerNumber, uint32_t addr, uint32_t value) {

  switch (timerNumber) {
  case XPAR_AXI_TIMER_0_DEVICE_ID:
    Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR + addr, value);
    break;
  case XPAR_AXI_TIMER_1_DEVICE_ID:
    Xil_Out32(XPAR_AXI_TIMER_1_BASEADDR + addr, value);
    break;
  case XPAR_AXI_TIMER_2_DEVICE_ID:
    Xil_Out32(XPAR_AXI_TIMER_2_BASEADDR + addr, value);
    break;
  }
}

void intervalTimer_initCountUp(uint32_t timerNumber) {
  // Cascading bit

  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) & DEFAULT_VALUE);
  writeRegister(timerNumber, TCSR1_OFFSET,
                readRegister(timerNumber, TCSR1_OFFSET) & DEFAULT_VALUE);

  writeRegister(timerNumber, TLR0_OFFSET,
                readRegister(timerNumber, TLR0_OFFSET) & DEFAULT_VALUE);
  writeRegister(timerNumber, TLR1_OFFSET,
                readRegister(timerNumber, TLR1_OFFSET) & DEFAULT_VALUE);

  writeRegister(timerNumber, TCSR0_OFFSET,
                (readRegister(timerNumber, TCSR0_OFFSET) | CASCADE_BIT));
  printf("Cascade bit check: %x\n", readRegister(timerNumber, TCSR0_OFFSET));

  // printf("setting the cascading bit to 1: %x\n", readRegister(timerNumber,
  // 0x00)|0x800); printf("Load register bits1: %x\n", readRegister(timerNumber,
  // 0x04)); printf("load register bits2: %x\n", readRegister(timerNumber,
  // 0x14));

  intervalTimer_reload(timerNumber);
}

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade modemake
//  - The timer counts down
//  - The timer automatically reloads when reaching zero
// 2. Initialize LOAD registers with appropriate values, given the `period`.
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  writeRegister(timerNumber, TCSR0_OFFSET, COUNT_DOWN_ENABLES);

  uint64_t period_in_hex_ticks = (frequencyArray[timerNumber] * period);

  writeRegister(timerNumber, TLR0_OFFSET, (period_in_hex_ticks));
  writeRegister(timerNumber, TLR1_OFFSET, (period_in_hex_ticks) >> SHIFT_BIT);

  intervalTimer_reload(timerNumber);
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_start(uint32_t timerNumber) {
  writeRegister(
      timerNumber, TCSR0_OFFSET,
      (readRegister(timerNumber, TCSR0_OFFSET) | INTERVAL_TIMER_START_BIT));
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_stop(uint32_t timerNumber) {
  writeRegister(
      timerNumber, TCSR0_OFFSET,
      (readRegister(timerNumber, TCSR0_OFFSET) & ~(0x01 << ENT0_BIT)));
}

// This function is called whenever you want to reload the Counter values
// from the load registers.  For a count-up timer, this will reset the
// timer to zero.  For a count-down timer, this will reset the timer to
// its initial count-down value.  The load registers should have already
// been set in the appropriate `init` function, so there is no need to set
// them here.  You just need to enable the load (and remember to disable it
// immediately after otherwise you will be loading indefinitely).
void intervalTimer_reload(uint32_t timerNumber) {

  // intervalTimer_initCountUp(timerNumber);
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) | RELOAD_BIT);
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) & ~(0x01 << LOAD0_BIT));
  printf("Reading register after reloading: %x\n",
         readRegister(timerNumber, TCSR0_OFFSET));
  writeRegister(timerNumber, TCSR1_OFFSET,
                readRegister(timerNumber, TCSR1_OFFSET) | RELOAD_BIT);
  writeRegister(timerNumber, TCSR1_OFFSET,
                readRegister(timerNumber, TCSR1_OFFSET) & ~(0x01 << LOAD0_BIT));
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode you will need to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t val0 = readRegister(timerNumber, TCR0_OFFSET);
  uint64_t val1 = readRegister(timerNumber, TCR1_OFFSET);
  uint64_t total_bits = ((val0) | (val1 << SHIFT_BIT));

  double inSeconds =
      (((double)total_bits) / ((double)frequencyArray[timerNumber]));

  return inSeconds;
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber) {
  uint32_t store_val = readRegister(timerNumber, TCSR0_OFFSET);
  writeRegister(timerNumber, TCSR0_OFFSET, store_val | ENABLE_INTERRUPT_BIT);
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber) {
  uint32_t store_val = readRegister(timerNumber, TCSR0_OFFSET);
  writeRegister(timerNumber, TCSR0_OFFSET, store_val & ~(0x01 << ENIT0_BIT));
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber) {
  uint32_t store_val = readRegister(timerNumber, TCSR0_OFFSET);
  writeRegister(timerNumber, TCSR0_OFFSET, store_val | ACK_INTERRUPT_BIT);
}

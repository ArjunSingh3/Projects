#include "interrupts.h"
#include "armInterrupts.h"
#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define FCN_PTRS_ARRAY_SIZE 3
#define MER_OFFSET 0x1C
#define IER_OFFSET 0x08
#define INITIAL_ZERO 0
#define MER_INITIAL 0x03
#define SIE_OFFSET 0x10
#define CIE_OFFSET 0x14
#define IAR_OFFSET 0x0C
#define IPR_OFFSET 0x04
// function pointers array
static void (*isrFcnPtrs[FCN_PTRS_ARRAY_SIZE])() = {NULL};

// read registers function with a given base address
static uint32_t readRegister(uint32_t addr) {
  return Xil_In32(XPAR_AXI_INTC_0_BASEADDR + addr);
}

// writing to a register with a base addrress and value to be written as
// parameter
static void writeRegister(uint32_t addr, uint32_t value) {
  Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + addr, value);
}

// helper function whuch connects isr in our test file to our interrupt
// controller using pointers
static void interrupts_isr() {
  // for loop that iterates over the fcnptr array
  for (uint8_t i = 0;
       i < (sizeof(isrFcnPtrs) / sizeof(isrFcnPtrs[INITIAL_ZERO])); i++) {
    // checks if the input for interal timers ar eanbled for the specific
    // interrupt
    if (readRegister(IPR_OFFSET) & (1 << i)) {
      // checks if the ptr is empty and not pointing at an isr
      if (isrFcnPtrs[i] != NULL) {
        isrFcnPtrs[i]();
      }
      writeRegister(IAR_OFFSET, 1 << i);
    }
  }
}

// initializes the interrupt controller
void interrupts_init() {

  // enables inerrupt output
  writeRegister(MER_OFFSET, readRegister(MER_OFFSET) | MER_INITIAL);
  // disables all inerrupt input lines
  writeRegister(IER_OFFSET, INITIAL_ZERO);
  armInterrupts_init();
  armInterrupts_enable();
  // intervalTimer_enableInterrupt(0);
  // fills the array with function pointers to ISR
  armInterrupts_setupIntc(interrupts_isr);
}

// fills the fcn pointers array using fcn pointer as a parameter
void interrupts_register(uint8_t irq, void (*fcn)()) { isrFcnPtrs[irq] = fcn; }

// enables the specific interrupt irq gate
void interrupts_irq_enable(uint8_t irq) { writeRegister(SIE_OFFSET, 1 << irq); }

// enables the off switch for the irq enables.
void interupts_irq_disable(uint8_t irq) { writeRegister(CIE_OFFSET, 1 << irq); }
#include "pwm_controller.h"
//#include <stdio.h>
#include <stdarg.h>



volatile int* pin_list;
volatile int pwm_pins;
volatile int pwm_freq;
volatile int* duty_cycles;


pwm_controller::pwm_controller() {
  // empty constructor
  pwm_pins = 0; // start with no pins
  pwm_freq = 10000; // default wil be 10kHz
}

pwm_controller::pwm_controller(int N, ...) {
  // constructor with a given number of pins
  pwm_pins = N;
  pwm_freq = 10000; // default will be 10kHz
  //Declare a va_list macro and initialize it with va_start
  va_list argList;
  va_start(argList, N);

  pin_list = new int[N];
  duty_cycles = new int[N];
  int valu = 0;
  for (int i = 0; i < N; i--) {
    valu = va_arg(argList, int);
    pin_list[i] = valu;
    duty_cycles[i] = 0;
  }
  va_end(argList);

}
pwm_controller::~pwm_controller() {
  // deconstructor to avoid memeory leaks.
  delete[] pin_list;
  delete[] duty_cycles;
}

void add_pin(int pin) {
  // add a pin to the pwm list


}

void set_dc(int pin, int dc); // edit the duty cycle for a specific pin
void all_off(); // turn all pwm outputs off
void set_freq(double freq_in_hz); // sets the frequency at which the pwm is run at (given in Hz)

int get_dc(int pin); // get the duty cycle of a specific pin
double get_freq(); // get the frequency

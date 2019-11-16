#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H
#include "Arduino.h"
#include <stdio.h>

class pwm_controller {
  public:
    pwm_controller(); // empty constructor
    pwm_controller(int N, ...); // constructor with a given number of pins
    ~pwm_controller(); // deconstructor to avoid memeory leaks.

    void add_pin(int pin); // add a pin to the pwm list

    void set_dc(int pin, int dc); // edit the duty cycle for a specific pin
    void all_off(); // turn all pwm outputs off
    void set_freq(double freq_in_hz); // sets the frequency at which the pwm is run at (given in Hz)

    int get_dc(int pin); // get the duty cycle of a specific pin
    double get_freq(); // get the frequency

  private:
    volatile int* pin_list;
    volatile int pwm_pins;
    volatile int pwm_freq;
    volatile int* duty_cycles;
  };
  #endif

#include "motor_controller.h"
#include "Arduino.h"
#include "PID_v1.h"

/*
  This mapping function is the same as the arduino map function as outlined
  here (https://www.arduino.cc/reference/en/language/functions/math/map/) but
  is written for double type instead of long.
*/
double my_map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Motor::Motor(int pwm_pin, int dir_pin, int enc_1, int enc_2)
{
  begin(pwm_pin, dir_pin, enc_1, enc_2);
}

Motor::Motor()
{
  // empty construtor
  // for use with begin
}

void Motor::begin(int pwm_pin, int dir_pin, int enc_1, int enc_2)
{
  // setting up the pins
  _pwm_pin = pwm_pin;
  _dir_pin = dir_pin;
  _enc_1_pin = enc_1;
  _enc_2_pin = enc_2;
  pinMode(_pwm_pin, OUTPUT);
  pinMode(_dir_pin, OUTPUT);
  pinMode(_enc_1_pin, INPUT_PULLUP);
  pinMode(_enc_2_pin, INPUT_PULLUP);


  /*
    kp, ki, kd stand for Proportional gain, Integral Gain, and Derivative gain
    these are used in the PID to make it respond better.
    For more information on this look here
    (https://robotics.stackexchange.com/questions/167/what-are-good-strategies-for-tuning-pid-loops)
  */
  _kp = 0.01;
  _ki = 1;
  _kd = 0;

  // input, output, and setpoint are used in the PID
  _input = 0;
  _output = 0;
  _setpoint = 0;
  last_count = 0;
  _total_count = 0;
  // _pid.SetSampleTime(1); // this is untested
  _pid = new PID(&_input, &_output, &_setpoint, _kp, _ki, _kd, P_ON_E, DIRECT);
  sample_time = 50;
  _pid->SetSampleTime(sample_time);
  SetOutputLimits(-255, 255);

  output_channel = new DimmerZero(_pwm_pin);
  output_channel->init();
}

void Motor::isrA() {
  if (digitalRead(_enc_1_pin) != digitalRead(_enc_2_pin)) {
    count++;
  }
  else {
    count--;
  }
}

void Motor::isrB() {
  if (digitalRead(_enc_1_pin) == digitalRead(_enc_2_pin)) {
    count++;
  }
  else {
    count--;
  }
}

/*
  This function is used to manually set the speed of the motor.
  Speed should be between -255 and 255 however it is constrained to that in the
  output so if you give it something over, its no big deal.
*/
void Motor::Manual(double speed)
{
  _pid->SetMode(MANUAL);
  _output = speed;
  Output();
}


/*
  This function is called all the time, Essentially it makes sure we have the
  latest feedback value and then react accordingly.
*/
void Motor::Update()
{
  Feedback();
  Output();
}

/*
  Set the new setpoint for the PID. If the PID is disabled, this still sets
  but does nothing
*/
void Motor::SetSetpoint(int new_setpoint)
{
  _setpoint = new_setpoint;
}

/*
  Turning on the PID, This will cause it to start moving next time
  output (or update) is calculated
*/
void Motor::EnablePID()
{
  _pid->SetMode(AUTOMATIC);
}
/*
  Turning off the PID, This will force the motor to stop and be set to manual mode
*/
void Motor::DisablePID()
{
  Manual(0);
  _pid->SetMode(MANUAL);
}

// This sets the output of the PID, external use of this should not be needed
void Motor::SetOutputLimits(int lower_bound, int upper_bound)
{
  _pid->SetOutputLimits(lower_bound, upper_bound);
}

// Return if the PID is turned on
bool Motor::PidEnabled()
{
  return _pid->GetMode() == 0 ? false : true;
}

/*
  This reads an analog in and then maps it into an angle.
  This is assuming that the input is linear.
*/
void Motor::Feedback()
{
  noInterrupts();
  this_time = millis();
  _input = (count);
  _input *= 60.0 / (1296 * sample_time * 0.001); // (tics/0.1sec) * (1 rev / 1296 tics) * (60s/ 1 min) = rpm
  last_time = this_time;
  interrupts();
}


/*
  The output method is the one place where the object goes to make any changes
  to the pins. This includes any digital writes or analog writes.

  By keeping all of the outputs in one place it is easier to implement soft
  limits.

  This is where most of the magic happens
*/
int Motor::Output()
{
  // pid_computed is if the PID actually computed the output or not.
  // only really used in debugging. Could be removed with no harm.
  int pid_computed = 0;

  /*
    If the PID is in manual mode Compute returns right away not calculating
    anything, this means that if you set the output elsewhere it wont be changed
    by calling comput
  */
  pid_computed = _pid->Compute();

  /*
    With our linear actuators if we have a output of less than 80 they dont move
    and just make a bad noise. This just adds a bit of protection for our ears.
  */
  _output = constrain(_output, -255, 255);
  //_output = abs(_output) > 80 ? _output : 0;
  output_channel->setValue(my_map(abs(_output), 0, 255, 0, output_channel->getMaxValue()));


  /*
     This block sets the default direction for the linear actuator.

     I think this one line could replace this block but its untested
     _output>0 ? digitalWrite(_dir_pin,1) : digitalWrite(_dir_pin,0);
  */

  if (_output > 0) {
    digitalWrite(_dir_pin, 1);
    digitalWrite(6, 0);
  }
  else {
    digitalWrite(_dir_pin, 0);
    digitalWrite(6, 1);
  }
  if (pid_computed == 1) {
    last_last_count = last_count;
    last_count = count;
    _total_count += count;
    count = 0;
  }
  return pid_computed;
}

/*
   This allows for soft limits to be set so that the arm will not go past
   certain angles. This is useful for demos to prevent crashing.
*/
void Motor::SetSoftLimits(int lower_bound, int upper_bound)
{
  _upper_soft_limit = upper_bound;
  _lower_soft_limit = lower_bound;
}

void Motor::SetPIDGains(double new_kp, double new_ki, double new_kd)
{
  _kp = new_kp;
  _ki = new_ki;
  _kd = new_kd;
  _pid->SetTunings(_kp, _ki, _kd);
}

void Motor::SetWheelSize(double new_wheel_size)
{
  _wheel_size = new_wheel_size;
}
/*
   BELOW THIS POINT IS ALL GETTERS
*/
double Motor::GetPGain()
{
  return _kp;
}
double Motor::GetIGain()
{
  return _ki;
}
double Motor::GetDGain()
{
  return _kd;
}
double Motor::GetInput()
{
  return _input;
}
double Motor::GetAngle()
{
  return _input;
}
double Motor::GetRawInput()
{
  return _input;
}
double Motor::GetSetpoint()
{
  return _setpoint;
}
double Motor::GetOutput()
{
  return _output;
}

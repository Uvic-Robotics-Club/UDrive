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
  x = constrain(x, -in_max, in_max);
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template <class T>
T limit_rate(T current, T last, T rate) {
  T difference = current - last;
  bool dir = current > last; // true is going up. false is going down
  if (abs(difference) > rate) {
    if (dir) {
      // this is going up
      current = last + rate;
    }
    else {
      current = last - rate;
    }
  }
  return current;
}

Motor::Motor(int pwm_pin_1, int pwm_pin_2, int enc_1, int enc_2)
{
  begin(pwm_pin_1, pwm_pin_2, enc_1, enc_2);
}

Motor::Motor()
{
  // empty construtor
  // for use with begin
}

void Motor::begin(int pwm_pin_1, int pwm_pin_2, int enc_1 = -1, int enc_2 = -1)
{
  // setting up the pins
  _pwm_pin_a = pwm_pin_1;
  _pwm_pin_b = pwm_pin_2;
  _enc_1_pin = enc_1;
  _enc_2_pin = enc_2;
  pinMode(_pwm_pin_a, OUTPUT);
  pinMode(_pwm_pin_b, OUTPUT);
  if (_enc_1_pin == -1 or _enc_2_pin == -1) {}
  else {
    pinMode(_enc_1_pin, INPUT_PULLUP);
    pinMode(_enc_2_pin, INPUT_PULLUP);
  }
  _position = false;


  /*
    kp, ki, kd stand for Proportional gain, Integral Gain, and Derivative gain
    these are used in the PID to make it respond better.
    For more information on this look here
    (https://robotics.stackexchange.com/questions/167/what-are-good-strategies-for-tuning-pid-loops)
  */
  _kp = 0.43;
  _ki = 0.3;
  _kd = 0;

  // input, output, and setpoint are used in the PID
  _input = 0;
  _output = 0;
  _setpoint = 0;
  last_count = 0;
  _total_count = 0;
  _wheel_size = 0.07; // 70mm or 7 cm
  // _pid.SetSampleTime(1); // this is untested
  _pid = new PID(&_input, &_output, &_setpoint, _kp, _ki, _kd, P_ON_E, DIRECT);
  sample_time = 50;
  _pid->SetSampleTime(sample_time);
  output_channel_a = new DimmerZero(_pwm_pin_a, false);
  output_channel_a->init();
  output_channel_b = new DimmerZero(_pwm_pin_b, false);
  output_channel_b->init();
  SetOutputLimits(-100, 100);
  this_time = micros();
  last_time = this_time;

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
  _output = my_map(speed, -100, 100, -100, 100);
  Output();
  _position = false;
}

void Motor::Velocity(double speed) {
  _setpoint = speed;
  _position = false;
  EnablePID();

}

void Motor::Position(double distance) {
  _setpoint = distance/10.0;
  _position = true;
  EnablePID();
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
  if (_pid->GetMode() != AUTOMATIC) {
    _pid->SetMode(AUTOMATIC);
    last_last_count = last_count;
    last_count = count;
    _total_count += count;
    count = 0;
  }
}
/*
  Turning off the PID, This will force the motor to stop and be set to manual mode
*/
void Motor::DisablePID()
{
  Manual(0);
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
  this_time = micros();
  //_input = count;
  if (not _position) {
    _input = count * 60.0 / (1288.848 * sample_time * 0.001); // (tics/0.1sec) * (1 rev / 1296 tics) * (60s/ 1 min) = rpm
    //_input *= _wheel_size * PI; // m per rev, _input is now in m
  }
  else {
    //sample_time = (this_time - last_time);
    //sample_time *= 0.000001; // micro into seconds
    //_input /= sample_time; // whatever per sec, _input is now in m/s
    _input = count * 0.0007759; // 1288.848 revs per ticks, _input is now in revolutions
  }
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
  //last_last_count = limit_rate(_output, last_last_count, 10.0);
  if (_output > 0) {
    output_channel_a->setValue(
      my_map(abs(_output), 0, 100, 0, output_channel_a->getMaxValue())
    );
    output_channel_b->setValue(
      0
    );
  }
  else {
    output_channel_a->setValue(
      0
    );
    output_channel_b->setValue(
      my_map(abs(_output), 0, 100, 0, output_channel_b->getMaxValue())
    );
  }

  //last_last_count = _output;


  if (pid_computed == 1) {
    last_last_count = last_count;
    last_count = _input;

    //_total_count += count;
    if (! _position) {
      count = 0;
    }
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
double Motor::GetWheelSize() {
  return _wheel_size;
}
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
double Motor::GetLastInput()
{
  return last_count;
}


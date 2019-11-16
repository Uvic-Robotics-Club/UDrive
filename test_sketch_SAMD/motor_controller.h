#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H
#include "PID_v1.h"
#include "Arduino.h"
#include <DimmerZero.h>

class Motor {
  public:
    // THIS IS IN ORDER IN THE CPP CLASS
    Motor(int pwm_pin, int dir_pin, int enc_1, int enc_2);
    Motor();
    void begin(int pwm_pin, int dir_pin, int enc_1, int enc_2);
    void Manual(double speed);
    void Velocity(double speed);
    void Position(double distance);
    void Update();
    void SetSetpoint(int new_setpoint);
    void EnablePID();
    void DisablePID();
    // set output limits but this is private
    bool PidEnabled();
    void Feedback();
    void Mapping(double low_raw, double high_raw, double low_angle, double high_angle);
    int Output();
    void SetSoftLimits(int lower_bound, int upper_bound);
    void SetPIDGains(double new_kp, double new_ki, double new_kd);
    void SetWheelSize(double new_wheel_size);
    double GetWheelSize();
    double GetPGain();
    double GetIGain();
    double GetDGain();
    double GetInput();
    double GetAngle();
    double GetRawInput();
    double GetSetpoint();
    double GetOutput();
    double GetLastInput();
    
    void isrA();
    void isrB();
    
    bool _position;
    volatile int count, last_count;
    double last_last_count;
    long _total_count;

  private:
    void SetOutputLimits(int lower_bound, int upper_bound);
    int _pwm_pin_a, _pwm_pin_b; // Pins that we use
    volatile int _enc_1_pin, _enc_2_pin;
    
    int sample_time; // how fast the PID should sample
    double _setpoint, _input, _output; // used for PID
    double this_time, last_time;
    double _wheel_size;
    double _kp, _ki, _kd; // used for PID
    double _lower_soft_limit, _upper_soft_limit; // used for soft limits
    int _lower_absolute_limit, _upper_absolute_limit; // used for mapping voltage into angle
    int _lower_actual_angle, _upper_actual_angle; // used for mapping voltage into angle
    PID* _pid; // is the PID
    DimmerZero* output_channel_a; // cant use  analogWrite with the new uC
    DimmerZero* output_channel_b; // cant use  analogWrite with the new uC
};


#endif

/*
* Declarations for motor drivers. http://www.ti.com/product/DRV8704
*/

#ifndef DRV8704_H
#define DRV8704_H
#include "constants.h"
using namespace DRV8704_CONSTANTS;

struct DRV8704_Status{
    bool overtemp_shutdown;
    bool channel_A_overcurrent_protection;
    bool channel_B_overcurrent_protection;
    bool channel_A_predriver_fault;
    bool channel_B_predriver_fault;
    bool undervoltage_lockout;
};

struct DRV8704_Settings{
    int chip_select_pin;
    double current_sense_resistor;
    int torque;
    ENBL_Values enabled;
    ISGAIN_Values current_gain;
    DTIME_Values dead_time;
    int off_time;
    PWMMODE_Values pwm_mode;
    int blanking_time;
    int mixed_decay_time;
    DECMOD_Values decay_mode;
    OCPTH_Values over_current_protection_voltage_threshold;
    OCPDEG_Values over_current_protection_deglitch_time;
    TDRIVEN_Values gate_drive_sink_time;
    TDRIVEP_Values gate_drive_source_time;
    IDRIVEN_Values gate_drive_peak_sink_current;
    IDRIVEP_Values gate_drive_peak_source_current;
    DRV8704_Status status;
};

class DRV8704{
    public:
        DRV8704();
        ~DRV8704();
        DRV8704_Settings get_settings();
        void default_settings();
        void begin(int chip_select_pin);
        void begin(DRV8704_Settings setup_parameters);
        void sanitize_settings();
        void send_CTRL();
        void send_TORQUE();
        void send_OFF();
        void send_BLANK();
        void send_DECAY();
        void send_DRIVE();
        void reset_STATUS();
        void set_settings(DRV8704_Settings setup_parameters);
        void set_enable(bool enable);
        bool read_status();
        void write_register_16(int address, int data);
        int read_register_16(int address);
        DRV8704_Settings settings;
};

#endif

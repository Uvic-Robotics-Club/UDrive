/*
* Represents a motor driver. http://www.ti.com/product/DRV8704
*/

#include "constants.h"
#include "DRV8704.h"
#include <SPI.h>

using namespace DRV8704_CONSTANTS;

DRV8704::DRV8704(){ // constructor
    settings.current_sense_resistor = 0.0075;// two 15mOhm Resistors in parallel
    default_settings();
}

DRV8704::~DRV8704(){ // de-constructor
    SPI.end();
}

DRV8704_Settings DRV8704::get_settings(){
    return settings;
}

void DRV8704::default_settings(){
    settings.torque = 0xFF; // 0xff is the max
    settings.enabled = ENABLE;
    settings.current_gain = GAIN_40;
    settings.dead_time = DEAD_410;
    settings.off_time = 0x30;
    settings.pwm_mode = PWM_Control_Mode;
    settings.blanking_time = 0x80;
    settings.mixed_decay_time = 0x10;
    settings.decay_mode = SLOW_DECAY;
    settings.over_current_protection_voltage_threshold = THRESHOLD_500mV;
    settings.over_current_protection_deglitch_time = DEGLITCH_2_10us;
    settings.gate_drive_sink_time = DRIVE_SINK_1050_ns;
    settings.gate_drive_source_time = DRIVE_SOURCE_1050_ns;
    settings.gate_drive_peak_sink_current = DRIVE_SINK_CURRENT_400_mA;
    settings.gate_drive_peak_source_current = DRIVE_SOURCE_CURRENT_200_mA;
}

void DRV8704::begin(int chip_select_pin){
    SPI.begin();
    settings.chip_select_pin = chip_select_pin;
    default_settings();
    pinMode(settings.chip_select_pin,OUTPUT);
    digitalWrite(settings.chip_select_pin,HIGH);
    set_settings(settings);
}

void DRV8704::begin(DRV8704_Settings setup_parameters){
    settings = setup_parameters;
    sanitize_settings();
    SPI.begin();
    pinMode(settings.chip_select_pin,OUTPUT);
    digitalWrite(settings.chip_select_pin,HIGH);
    set_settings(settings);
}

void DRV8704::sanitize_settings(){
    settings.torque = settings.torque&0xFF;
    settings.off_time = settings.off_time&0xFF;
    settings.blanking_time = settings.blanking_time&0xFF;
    settings.mixed_decay_time = settings.mixed_decay_time&0xFF;
}

void DRV8704::send_CTRL(){
    static int data_to_send = 0;
    data_to_send = 0;
    data_to_send |= ENBL_Bit&(settings.enabled<<ENBL_OFFSET);
    data_to_send |= ISGAIN_Bit&(settings.current_gain<<ISGAIN_OFFSET);
    data_to_send |= DTIME_Bit&(settings.dead_time<<DTIME_OFFSET);
    data_to_send &= ~CTRL_Reserved_Bit;
    write_register_16(CTRL_Register,data_to_send);
}
void DRV8704::send_TORQUE(){
    static int data_to_send = 0;
    data_to_send = 0;
    data_to_send |= TORQUE_Bit&(settings.torque<<TORQUE_OFFSET);
    data_to_send &= ~TORQUE_Reserved_Bit;
    write_register_16(TORQUE_Register,data_to_send);
}
void DRV8704::send_OFF(){
    static int data_to_send = 0;
    data_to_send = 0;
    data_to_send |= TOFF_Bit&(settings.off_time<<TOFF_OFFSET);
    data_to_send |= PWMMODE_Bit&(settings.pwm_mode<<PWMMODE_OFFSET); // this should always be 1
    data_to_send &= ~OFF_Reserved_Bit;
    write_register_16(OFF_Register,data_to_send);
}

void DRV8704::send_BLANK(){
    static int data_to_send = 0;
    data_to_send = 0;
    data_to_send |= TBLANK_Bit&(settings.blanking_time<<TBLANK_OFFSET);
    data_to_send &= ~BLANK_Reserved_Bit;
    write_register_16(BLANK_Register,data_to_send);
}

void DRV8704::send_DECAY(){
    static int data_to_send = 0;
    data_to_send = 0;
    data_to_send |= TDECAY_Bit&(settings.blanking_time<<TDECAY_OFFSET);
    data_to_send |= DECMOD_Bit&(settings.decay_mode<<DECMOD_OFFSET);
    data_to_send &= ~DECAY_Reserved_Bit;
    write_register_16(DECAY_Register,data_to_send);
}

void DRV8704::send_DRIVE(){
    static int data_to_send = 0;
    data_to_send = 0;
    data_to_send |= OCPTH_Bit&(settings.over_current_protection_voltage_threshold<<OCPTH_OFFSET);
    data_to_send |= OCPDEG_Bit&(settings.over_current_protection_deglitch_time<<OCPDEG_OFFSET);
    data_to_send |= TDRIVEN_Bit&(settings.gate_drive_sink_time<<TDRIVEN_OFFSET);
    data_to_send |= TDRIVEP_Bit&(settings.gate_drive_source_time<<TDRIVEP_OFFSET);
    data_to_send |= IDRIVEN_Bit&(settings.gate_drive_peak_sink_current<<IDRIVEN_OFFSET);
    data_to_send |= IDRIVEP_Bit&(settings.gate_drive_peak_source_current<<IDRIVEP_OFFSET);
    // data_to_send &= ~DRIVE_Reserved_Bit; // no reserved bits
    write_register_16(DRIVE_Register,data_to_send);
}

void DRV8704::reset_STATUS(){
    write_register_16(STATUS_Register,0x00);
}

void DRV8704::set_settings(DRV8704_Settings setup_parameters){
    settings = setup_parameters;
    send_CTRL();
    send_TORQUE();
    send_OFF();
    send_BLANK();
    send_DECAY();
    send_DRIVE();
    reset_STATUS();
}

void DRV8704::set_enable(bool enable){
    settings.enabled = enable==true ? ENABLE:DISABLE;
    send_CTRL();
}

bool DRV8704::read_status(){
    static int results = 0;
    results = 0;
    results = read_register_16(STATUS_Register);
    results = results&~STATUS_Reserved_Bit;
    //settings.status.overtemp_shutdown = results&OTS_Bit;
    settings.status.overtemp_shutdown = results;
    settings.status.channel_A_overcurrent_protection = results&AOCP_Bit;
    settings.status.channel_B_overcurrent_protection = results&BOCP_Bit;
    settings.status.channel_A_predriver_fault = results&APDF_Bit;
    settings.status.channel_B_predriver_fault = results&BPDF_Bit;
    settings.status.undervoltage_lockout = results&UVLO_Bit;
    if (results>0){
        return false;
    }
    return true;
}

void DRV8704::write_register_16(int address, int data){
    static int data_to_send = 0x0000;
    data_to_send = 0x0000; // make sure bit 1 is a zero
    address = address&0b0111;
    data_to_send |= address<<12; // the next three bits are address
    data_to_send |= data&0x0FFF; // the remaining bits are data
    digitalWrite(settings.chip_select_pin,HIGH);
    SPI.transfer16(data_to_send);
    digitalWrite(settings.chip_select_pin,LOW);
}
int DRV8704::read_register_16(int address){
    static int data_received = 0x0000;
    data_received = 0x0000;
    address = address&0b0111;
    digitalWrite(settings.chip_select_pin,HIGH);
    data_received = SPI.transfer16(address|0b1000); // make sure bit 1 is a one
    digitalWrite(settings.chip_select_pin,LOW);
    return data_received;
}

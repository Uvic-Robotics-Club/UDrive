// constants.h
#ifndef constants_h
#define constants_h

namespace DRV8704_CONSTANTS{
// TODO: confirm that I want to have the bits be the starting address vs the mask of the whole bit pattern

// REGISTERS
enum REGISTERS{
    CTRL_Register = 0x00,
    TORQUE_Register = 0x01,
    OFF_Register = 0x02,
    BLANK_Register = 0x03,
    DECAY_Register = 0x04,
    RESERVED_Register = 0x05,
    DRIVE_Register = 0x06,
    STATUS_Register = 0x07
};

// BIT MASKS
enum CTRL_Bits{
    ENBL_Bit          = 0b000000000001,
    ISGAIN_Bit        = 0b001100000000,
    DTIME_Bit         = 0b110000000000,
    CTRL_Reserved_Bit = 0b000011111110
};

enum TORQUE_Bits{
    TORQUE_Bit          = 0b000011111111,
    TORQUE_Reserved_Bit = 0b111100000000
};

enum OFF_Bits{
    TOFF_Bit         = 0b000011111111,
    PWMMODE_Bit      = 0b000100000000,
    OFF_Reserved_Bit = 0b111000000000
};

enum BLANK_Bits{
    TBLANK_Bit         = 0b000011111111,
    BLANK_Reserved_Bit = 0b111100000000
};

enum DECAY_Bits{
    TDECAY_Bit         = 0b000011111111,
    DECMOD_Bit         = 0b011100000000,
    DECAY_Reserved_Bit = 0b100000000000
};

enum DRIVE_Bits{
    OCPTH_Bit   = 0b000000000011,
    OCPDEG_Bit  = 0b000000001100,
    TDRIVEN_Bit = 0b000000110000,
    TDRIVEP_Bit = 0b000011000000,
    IDRIVEN_Bit = 0b001100000000,
    IDRIVEP_Bit = 0b110000000000
};

enum STATUS_Bits{
    OTS_Bit             = 0b000000000001,
    AOCP_Bit            = 0b000000000010,
    BOCP_Bit            = 0b000000000100,
    APDF_Bit            = 0b000000001000,
    BPDF_Bit            = 0b000000010000,
    UVLO_Bit            = 0b000000100000,
    STATUS_Reserved_Bit = 0b111111000000
};

// OFFSETS
enum OFFSETS{
    ENBL_OFFSET = 0,
    ISGAIN_OFFSET = 8,
    DTIME_OFFSET = 10,
    TORQUE_OFFSET =  0,
    TOFF_OFFSET = 0,
    PWMMODE_OFFSET = 8,
    TBLANK_OFFSET = 0,
    TDECAY_OFFSET = 0,
    DECMOD_OFFSET = 8,
    OCPTH_OFFSET = 0,
    OCPDEG_OFFSET = 2,
    TDRIVEN_OFFSET = 4,
    TDRIVEP_OFFSET = 6,
    IDRIVEN_OFFSET = 8,
    IDRIVEP_OFFSET = 10,
    OTS_OFFSET = 0,
    AOCP_OFFSET = 1,
    BOCP_OFFSET = 2,
    APDF_OFFSET = 3,
    DPDF_OFFSET = 4,
    UVLO_OFFSET = 5
};


// CTRL REG
enum ENBL_Values{
    ENABLE = 0b1, // default
    DISABLE = 0b0
};

enum ISGAIN_Values{
    GAIN_5  = 0b00,
    GAIN_10 = 0b01,
    GAIN_20 = 0b10,
    GAIN_40 = 0b11 // default
};

enum DTIME_Values{
    DEAD_410 = 0b00, // default
    DEAD_460 = 0b01,
    DEAD_670 = 0b10,
    DEAD_880 = 0b11
};


// OFF REG
enum PWMMODE_Values{
    PWM_Control_Mode = 0x01 // default
};

// DECAY REG
enum DECMOD_Values{
    SLOW_DECAY  = 0b000, // default
    FAST_DECAY  = 0b010,
    MIXED_DECAY = 0b011,
    AUTO_MIXED  = 0b101
};

// DRIVE REG
enum OCPTH_Values{
    THRESHOLD_250mV  = 0b00,
    THRESHOLD_500mV  = 0b01, // default
    THRESHOLD_750mV  = 0b10,
    THRESHOLD_1000mV = 0b11
};

enum OCPDEG_Values{
    DEGLITCH_1_05us = 0b00,
    DEGLITCH_2_10us = 0b01, // default
    DEGLITCH_4_20us = 0b10,
    DEGLITCH_8_40us = 0b11
};

enum TDRIVEN_Values{
    DRIVE_SINK_263_ns  = 0b00,
    DRIVE_SINK_525_ns  = 0b01,
    DRIVE_SINK_1050_ns = 0b10, // default
    DRIVE_SINK_2100_ns = 0b11
};

enum TDRIVEP_Values{
    DRIVE_SOURCE_263_ns  = 0b00,
    DRIVE_SOURCE_525_ns  = 0b01,
    DRIVE_SOURCE_1050_ns = 0b10, // default
    DRIVE_SOURCE_2100_ns = 0b11
};

enum IDRIVEN_Values{
    DRIVE_SINK_CURRENT_100_mA = 0b00,
    DRIVE_SINK_CURRENT_200_mA = 0b01,
    DRIVE_SINK_CURRENT_300_mA = 0b10,
    DRIVE_SINK_CURRENT_400_mA = 0b11 // default
};

enum IDRIVEP_Values{
    DRIVE_SOURCE_CURRENT_50_mA  = 0b00,
    DRIVE_SOURCE_CURRENT_100_mA = 0b01,
    DRIVE_SOURCE_CURRENT_150_mA = 0b10,
    DRIVE_SOURCE_CURRENT_200_mA = 0b11 // default
};
};


#endif

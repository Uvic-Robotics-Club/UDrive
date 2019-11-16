#include "motor_controller.h"
#include "DRV8704.h"

#define DATAOUT 11  //MOSI
#define DATAIN  12  //MISO
#define SPICLOCK  13  //SCK
#define SLAVESELECT 8  //SS
#define RESET 4
#define SLEEP 5

#define AOUT1 2
#define AOUT2 3
#define BOUT1 6
#define BOUT2 7

#define ENCODERA A0
#define ENCODERB A1
#define ENCODERC A2
#define ENCODERD A3

bool reset_pin = false;
bool sleep_pin = true;
bool send_stuff = false;

bool stringComplete = false;
String toSend = "";
unsigned long time;
unsigned long motor_time;

String sendOut = "";

Motor m1;
Motor m2;
DRV8704 mc;
// DRV8704_Settings mc_settings; // in the future for more testing this will need to be changed

void setup() {
  SerialUSB.begin(115200); // Initialize Serial Monitor USB
  Serial1.begin(11520); // Initialize hardware serial port, pins 0/1
  // Must have both lines!
  while (!SerialUSB) ; // Wait for Serial monitor to open
  // Send a welcome message to the serial monitor:

  m1.begin(AOUT1, AOUT2, ENCODERA, ENCODERB);
  m2.begin(BOUT1, BOUT2, ENCODERC, ENCODERD);
  mc.begin(SLAVESELECT);
  mc.set_enable(true);// untested

  attachInterrupt(digitalPinToInterrupt(ENCODERA), isr_m1_a, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODERB), isr_m1_b, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODERC), isr_m2_a, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODERD), isr_m2_b, CHANGE);

  motor_time = time = millis();
  m1.EnablePID();
  m1.SetSetpoint(0);
  m2.EnablePID();
  m2.SetSetpoint(0);
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, reset_pin); // this is the reset pin
  pinMode(SLEEP, OUTPUT);
  digitalWrite(SLEEP, sleep_pin); // this is the sleep pin // leave this pin alone
  pinMode(SLAVESELECT, OUTPUT);
  delay(10);
}

void isr_m1_a() {
  m1.isrA();
}
void isr_m1_b() {
  m1.isrB();
}
void isr_m2_a() {
  m2.isrA();
}
void isr_m2_b() {
  m2.isrB();
}

char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  // https://forum.arduino.cc/index.php?topic=349764.0
  uint32_t iPart = (uint32_t)val;
  uint32_t dPart = (uint32_t)((val - (double)iPart) * pow(10, prec));

  sprintf(sout, "%d.%d", iPart, dPart);
  return sout;
}

void send_info_over_usb() {
  if (send_stuff) {
    char total_buffer[100];
    char m1_input_buffer[16];
    char m1_setpoint_buffer[16];
    char m1_output_buffer[16];
    char m2_input_buffer[16];
    char m2_setpoint_buffer[16];
    char m2_output_buffer[16];

    dtostrf(m1.GetLastInput(), 8, 3, m1_input_buffer);
    dtostrf(m1.GetSetpoint(), 8, 3, m1_setpoint_buffer);
    dtostrf(m1.GetOutput(), 8, 3, m1_output_buffer);

    dtostrf(m2.GetLastInput(), 8, 3, m2_input_buffer);
    dtostrf(m2.GetSetpoint(), 8, 3, m2_setpoint_buffer);
    dtostrf(m2.GetOutput(), 8, 3, m2_output_buffer);

    sprintf(total_buffer,
            "%s, %s, %s, %s, %s, %s, ",
            m1_input_buffer,
            m1_setpoint_buffer,
            m1_output_buffer,
            m2_input_buffer,
            m2_setpoint_buffer,
            m2_output_buffer);

    SerialUSB.print(String(total_buffer) + sendOut);
    sendOut = "\n";
    send_stuff = false;
    /*
      SerialUSB.print(m1.GetRawInput());
      SerialUSB.print(",");
      SerialUSB.print(m1.GetSetpoint());
      SerialUSB.print(",");
      SerialUSB.print(m1.GetOutput());
      SerialUSB.print(",");
      SerialUSB.print(m2.GetRawInput());
      SerialUSB.print(",");
      SerialUSB.print(m2.GetSetpoint());
      SerialUSB.print(",");
      SerialUSB.println(m2.GetOutput());
    */
    //mc.read_status();
  }
}

void loop() {
  serialEvent();

  m1.Update();
  m2.Update();

  send_info_over_usb();

}

void set_m1(char mode, double value) {
  switch (mode) {
    case 'M':
      m1.Manual(value);
      break;
    case 'V':
      m1.SetSetpoint(value);
      m1.EnablePID();
      break;
    case 'T':
      m1.Position(value);
      m1.EnablePID();
      break;
  };
}

void set_m2(char mode, double value) {
  switch (mode) {
    case 'M':
      m2.Manual(value);
      break;
    case 'V':
      m2.SetSetpoint(value);
      m2.EnablePID();
      break;
    case 'T':
      m2.Position(value);
      m2.EnablePID();
      break;
  };
}

void serialEvent() {
  static int speed;
  static String first_string;
  static String second_string;

  if (SerialUSB.available()) { // If data is sent to the monitor
    toSend = ""; // Create a new string
    while (SerialUSB.available()) { // While data is available
      // Read from SerialUSB and add to the string:
      toSend += (char)SerialUSB.read();
    }
    stringComplete = true;
    int seperator = toSend.indexOf(';');
    if (seperator > 0) {
      first_string = toSend.substring(0, seperator);
      second_string = toSend.substring(seperator + 1);
    }
    else {
      first_string = toSend;
      second_string = "";
    }
    if (first_string.startsWith("V1")) { // velocity
      int first_comma = first_string.indexOf(',', 2);
      set_m1('V', first_string.substring(2, first_comma).toFloat());
    }
    if (second_string.startsWith("V2")) { // velocity
      int first_comma = second_string.indexOf(',', 2);
      set_m2('V', second_string.substring(2, first_comma).toFloat());
    }
    if (first_string.startsWith("P1")) { // PID
      int first_comma = first_string.indexOf(',', 2);
      int second_comma = first_string.indexOf(',', first_comma + 1);
      double kp = (double) first_string.substring(2, first_comma).toFloat();
      double ki = (double) first_string.substring(first_comma + 1, second_comma).toFloat();
      double kd = (double) first_string.substring(second_comma + 1).toFloat();
      m1.SetPIDGains(kp, ki, kd);
    }
    if (second_string.startsWith("P2")) { // PID
      int first_comma = second_string.indexOf(',', 2);
      int second_comma = second_string.indexOf(',', first_comma + 1);
      double kp = (double) second_string.substring(2, first_comma).toFloat();
      double ki = (double) second_string.substring(first_comma + 1, second_comma).toFloat();
      double kd = (double) second_string.substring(second_comma + 1).toFloat();
      m2.SetPIDGains(kp, ki, kd);
    }
    if (first_string.startsWith("M1")) { // manual duty cycle
      int first_comma = first_string.indexOf(',', 2);
      set_m1('M', first_string.substring(2, first_comma).toFloat());
    }
    if (second_string.startsWith("M2")) { // manual duty cycle
      int first_comma = second_string.indexOf(',', 2);
      set_m2('M', second_string.substring(2, first_comma).toFloat());
    }
    //else if (first_string.equals("W")) {
    //  m1.SetWheelSize((double) toSend.substring(1).toFloat());
    //  m2.SetWheelSize(m1.GetWheelSize());
    //}
    if (first_string.startsWith("T1")) {  // set the position that you want to go to (go To)
      int first_comma = first_string.indexOf(',', 2);
      set_m1('T', first_string.substring(2, first_comma).toFloat());
    }
    if (second_string.startsWith("T2")) {  // set the position that you want to go to (go To)
      int first_comma = second_string.indexOf(',', 2);
      set_m2('T', second_string.substring(2, first_comma).toFloat());
    }
    if (first_string.startsWith("Q")) {
      // read the motor driver settings
      mc.read_status();
      DRV8704_Settings _settings = mc.get_settings();
      sendOut = "Settings: ";
      sendOut += String(_settings.status.overtemp_shutdown);
      sendOut += ",";
      sendOut += String(_settings.status.channel_A_overcurrent_protection);
      sendOut += ",";
      sendOut += String(_settings.status.channel_B_overcurrent_protection);
      sendOut += ",";
      sendOut += String(_settings.status.channel_A_predriver_fault);
      sendOut += ",";
      sendOut += String(_settings.status.channel_B_predriver_fault);
      sendOut += ",";
      sendOut += String(_settings.status.undervoltage_lockout);
      sendOut += "\n";
    }
    if (first_string.equals("R")) { // toggle the reset pin
      reset_pin = !reset_pin;
      digitalWrite(RESET, reset_pin);
    }
    if (first_string.equals("S")) {// toggle the sleep pin
      sleep_pin = !sleep_pin;
      digitalWrite(SLEEP, sleep_pin);
    }
    if (first_string.equals("~")) {
      send_stuff = true;
    }
    if (first_string.startsWith("D")) {
      // set the settings for the DRV8704
      // have 14 values in total
      DRV8704_Settings new_settings = mc.get_settings();
      int comma_1 = toSend.indexOf(',', 1);
      int comma_2 = toSend.indexOf(',', comma_1 + 1);
      int comma_3 = toSend.indexOf(',', comma_2 + 1);
      int comma_4 = toSend.indexOf(',', comma_3 + 1);
      int comma_5 = toSend.indexOf(',', comma_4 + 1);
      int comma_6 = toSend.indexOf(',', comma_5 + 1);
      int comma_7 = toSend.indexOf(',', comma_6 + 1);
      int comma_8 = toSend.indexOf(',', comma_7 + 1);
      int comma_9 = toSend.indexOf(',', comma_8 + 1);
      int comma_10 = toSend.indexOf(',', comma_9 + 1);
      int comma_11 = toSend.indexOf(',', comma_10 + 1);
      int comma_12 = toSend.indexOf(',', comma_11 + 1);
      int comma_13 = toSend.indexOf(',', comma_12 + 1);

      int enable = (int)toSend.substring(1, comma_1).toInt();
      int A_gain = (int)toSend.substring(comma_1 + 1, comma_2).toInt();
      int dead_time = (int)toSend.substring(comma_2 + 1, comma_3).toInt();
      int torque = (int)toSend.substring(comma_3 + 1, comma_4).toInt();
      int time_off = (int)toSend.substring(comma_4 + 1, comma_5).toInt();
      int blank_time = (int)toSend.substring(comma_5 + 1, comma_6).toInt();
      int decay_time = (int)toSend.substring(comma_6 + 1, comma_7).toInt();
      int decay_mode = (int)toSend.substring(comma_7 + 1, comma_8).toInt();
      int ocp_thresh = (int)toSend.substring(comma_8 + 1, comma_9).toInt();
      int ocp_time = (int)toSend.substring(comma_9 + 1, comma_10).toInt();
      int gd_sink_time = (int)toSend.substring(comma_10 + 1, comma_11).toInt();
      int gd_source_time = (int)toSend.substring(comma_11 + 1, comma_12).toInt();
      int gd_sink_current = (int)toSend.substring(comma_12 + 1, comma_13).toInt();
      int gd_source_current = (int)toSend.substring(comma_13 + 1).toInt();

      switch (enable) {
        case 0:
          new_settings.enabled = ENBL_Values::DISABLE;
          break;
        default:
          new_settings.enabled = ENBL_Values::ENABLE;
          break;
      };

      switch (A_gain) {
        case 0:
          new_settings.current_gain = ISGAIN_Values::GAIN_5;
          break;
        case 1:
          new_settings.current_gain = ISGAIN_Values::GAIN_10;
          break;
        case 2:
          new_settings.current_gain = ISGAIN_Values::GAIN_20;
          break;
        default:
          new_settings.current_gain = ISGAIN_Values::GAIN_40;
          break;
      };

      switch (dead_time) {
        case 1:
          new_settings.dead_time = DTIME_Values::DEAD_460;
          break;
        case 2:
          new_settings.dead_time = DTIME_Values::DEAD_670;
          break;
        case 3:
          new_settings.dead_time = DTIME_Values::DEAD_880;
          break;
        default:
          new_settings.dead_time = DTIME_Values::DEAD_410;
          break;
      };

      new_settings.off_time = time_off;

      new_settings.blanking_time = blank_time;

      new_settings.mixed_decay_time = decay_time;

      switch (decay_mode) {
        case 1:
          new_settings.decay_mode = DECMOD_Values::FAST_DECAY;
          break;
        case 2:
          new_settings.decay_mode = DECMOD_Values::MIXED_DECAY;
          break;
        case 3:
          new_settings.decay_mode = DECMOD_Values::AUTO_MIXED;
          break;
        default:
          new_settings.decay_mode = DECMOD_Values::SLOW_DECAY;
          break;
      };
      switch (ocp_thresh) {
        case 0:
          new_settings.over_current_protection_voltage_threshold = OCPTH_Values::THRESHOLD_250mV;
          break;
        case 2:
          new_settings.over_current_protection_voltage_threshold = OCPTH_Values::THRESHOLD_750mV;
          break;
        case 3:
          new_settings.over_current_protection_voltage_threshold = OCPTH_Values::THRESHOLD_1000mV;
          break;
        default:
          new_settings.over_current_protection_voltage_threshold = OCPTH_Values::THRESHOLD_500mV;
          break;
      };

      switch (ocp_time) {
        case 0:
          new_settings.over_current_protection_deglitch_time = OCPDEG_Values::DEGLITCH_1_05us;
          break;
        case 2:
          new_settings.over_current_protection_deglitch_time = OCPDEG_Values::DEGLITCH_4_20us;
          break;
        case 3:
          new_settings.over_current_protection_deglitch_time = OCPDEG_Values::DEGLITCH_8_40us;
          break;
        default:
          new_settings.over_current_protection_deglitch_time = OCPDEG_Values::DEGLITCH_2_10us;
          break;
      };

      switch (gd_sink_time) {
        case 0:
          new_settings.gate_drive_sink_time = TDRIVEN_Values::DRIVE_SINK_263_ns;
          break;
        case 1:
          new_settings.gate_drive_sink_time = TDRIVEN_Values::DRIVE_SINK_525_ns;
          break;
        case 3:
          new_settings.gate_drive_sink_time = TDRIVEN_Values::DRIVE_SINK_2100_ns;
          break;
        default:
          new_settings.gate_drive_sink_time = TDRIVEN_Values::DRIVE_SINK_1050_ns;
          break;
      };

      switch (gd_source_time) {
        case 0:
          new_settings.gate_drive_source_time = TDRIVEP_Values::DRIVE_SOURCE_263_ns;
          break;
        case 1:
          new_settings.gate_drive_source_time = TDRIVEP_Values::DRIVE_SOURCE_525_ns;
          break;
        case 3:
          new_settings.gate_drive_source_time = TDRIVEP_Values::DRIVE_SOURCE_2100_ns;
          break;
        default:
          new_settings.gate_drive_source_time = TDRIVEP_Values::DRIVE_SOURCE_1050_ns;
          break;
      };

      switch (gd_sink_current) {
        case 0:
          new_settings.gate_drive_peak_sink_current = IDRIVEN_Values::DRIVE_SINK_CURRENT_100_mA;
          break;
        case 1:
          new_settings.gate_drive_peak_sink_current = IDRIVEN_Values::DRIVE_SINK_CURRENT_200_mA;
          break;
        case 2:
          new_settings.gate_drive_peak_sink_current = IDRIVEN_Values::DRIVE_SINK_CURRENT_300_mA;
          break;
        default:
          new_settings.gate_drive_peak_sink_current = IDRIVEN_Values::DRIVE_SINK_CURRENT_400_mA;
          break;
      };

      switch (gd_source_current) {
        case 0:
          new_settings.gate_drive_peak_source_current = IDRIVEP_Values::DRIVE_SOURCE_CURRENT_50_mA;
          break;
        case 1:
          new_settings.gate_drive_peak_source_current = IDRIVEP_Values::DRIVE_SOURCE_CURRENT_100_mA;
          break;
        case 2:
          new_settings.gate_drive_peak_source_current = IDRIVEP_Values::DRIVE_SOURCE_CURRENT_150_mA;
          break;
        default:
          new_settings.gate_drive_peak_source_current = IDRIVEP_Values::DRIVE_SOURCE_CURRENT_200_mA;
          break;
      };

      new_settings.torque = torque;

      mc.set_settings(new_settings);
    }


  }
}






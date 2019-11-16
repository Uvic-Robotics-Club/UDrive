/*
  This sketch illustrates how to set a timer on an SAMD21 based board in Arduino (Feather M0, Arduino Zero should work)
*/

uint32_t sampleRate = 100000; //sample rate of the sine wave in Hertz, how many times per second the TC5_Handler() function gets called per second basically
volatile int countAmount = 100;
#define LED_PIN 13
#define PWM_1_A 2
#define PWM_1_B 3
#define PWM_2_A 4
#define PWM_2_B 6
#define M1_ENC_A 5
#define M1_ENC_B 7
#define M2_ENC_A 8
#define M2_ENC_B 9
bool state = 0; //just for an example
volatile int duty_1 = 20;
volatile int duty_2 = 0;
volatile int count = 0;
volatile bool dir_1 = 1;
volatile bool dir_2 = 0;
volatile double input, output, setpoint;
volatile double kp=1,ki=10,kd=0;
PID pid(&input, &output, &setpoint, kp, ki, kd, P_ON_E, DIRECT);
setpoint = 50;


void setup() {
  pinMode(LED_PIN, OUTPUT); //this configures the LED pin, you can remove this it's just example code
  pinMode(PWM_1_A, OUTPUT);
  pinMode(PWM_1_B, OUTPUT);
  digitalWrite(PWM_1_A, HIGH);
  digitalWrite(PWM_1_A, LOW);
  //tcConfigure(sampleRate); //configure the timer to run at <sampleRate>Hertz
  //tcStartCounter(); //starts the timer
}

void loop() {
  
  REG_PORT_OUT0 ^= PORT_PA09;
  
}

//this function gets called by the interrupt at <sampleRate>Hertz
void TC5_Handler (void) {

  
  if (duty_1 == count) {
      REG_PORT_OUT0 = 0;
  }
  else {
    // REG_PORT_OUT0 ^= PORT_PA14;
  }
  count  = count + 1;
  if (count > countAmount) {
    count = 0;
    REG_PORT_OUT0 ^= PORT_PA14;
  }
  







  // END OF YOUR CODE
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code
}

/*
    TIMER SPECIFIC FUNCTIONS FOLLOW
    you shouldn't change these unless you know what you're doing
*/

//Configures the TC to generate output events at the sample frequency.
//Configures the TC in Frequency Generation mode, with an event output once
//each time the audio sample frequency period expires.
void tcConfigure(int sampleRate)
{
  // Enable GCLK for TCC2 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
  while (GCLK->STATUS.bit.SYNCBUSY);

  tcReset(); //reset TC5

  // Set Timer counter Mode to 16 bits
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  // Set TC5 mode as match frequency
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  //set prescaler and enable TC5
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
  //set TC5 timer counter based off of the system clock and the user defined sample rate or waveform
  TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / sampleRate - 1);
  while (tcIsSyncing());

  // Configure interrupt request
  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);
  NVIC_SetPriority(TC5_IRQn, 0);
  NVIC_EnableIRQ(TC5_IRQn);

  // Enable the TC5 interrupt request
  TC5->COUNT16.INTENSET.bit.MC0 = 1;
  while (tcIsSyncing()); //wait until TC5 is done syncing
}

//Function that is used to check if TC5 is done syncing
//returns true when it is done syncing
bool tcIsSyncing()
{
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

//This function enables TC5 and waits for it to be ready
void tcStartCounter()
{
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
  while (tcIsSyncing()); //wait until snyc'd
}

//Reset TC5
void tcReset()
{
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing());
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

//disable TC5
void tcDisable()
{
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}

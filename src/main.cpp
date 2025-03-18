#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <Temperature_LM75_Derived.h>

#define BUTTON1 PB3
#define BUTTON2 PB4
#define BUTTON3 PB5
#define LED1 PB13
#define LED2 PB14
#define LED3 PB15

#define READ_TEMP_PERIOD_SECONDS 10
#define READ_ADC_PERIOD_MICROSECONDS 1000

// Function prototype
void readTempIsr();
void readAdcIsr();

TwoWire wire(PB9, PB8);
Adafruit_ADS1115 ads;
Generic_LM75 temperature(&wire);
HardwareSerial serial1(PA10, PA9);

void setup() {

  // Initialize serial port
  serial1.begin(115200);

  // Print Header
  serial1.println("...............................");
  serial1.println("Getting single-ended reading from AIN0");
  serial1.println("ADC Range: +/- 0.256V");

  // Button Digital outputs
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  
  // LED Digital outputs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin(73U, &wire)) {
    serial1.println("Failed to initialize ADS.");
    while (1);
  }

  HardwareTimer *timerTemp = new HardwareTimer(TIM1);
  timerTemp->setMode(1, TIMER_OUTPUT_DISABLED, NC);
  timerTemp->setOverflow(READ_TEMP_PERIOD_SECONDS * 1000000U, MICROSEC_FORMAT);
  timerTemp->attachInterrupt(readTempIsr);
  timerTemp->resume();

  HardwareTimer *timerAdc = new HardwareTimer(TIM2);
  timerAdc->setMode(1, TIMER_OUTPUT_DISABLED, NC);
  timerAdc->setOverflow(READ_ADC_PERIOD_MICROSECONDS * 1000U, MICROSEC_FORMAT);
  timerAdc->attachInterrupt(readAdcIsr);
  timerAdc->resume();
}

// put your main code here, to run repeatedly:
void loop() {

  // control LED
  digitalWrite(LED1, HIGH);
  digitalWrite(LED3, LOW);
  delay(1000);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, LOW);
  delay(1000);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED2, LOW);
  delay(1000);

}

// ISR to read on board temperature sensor
void readTempIsr() {
  serial1.print("Temperature = ");
  serial1.print(temperature.readTemperatureC());
  serial1.println(" C");
}

// ISR to read ADC measurement
void readAdcIsr() {
  
  int16_t adcResult;
  const float adcScaling = 0.1875F/16; /* ADS1115  @ +/- 0.256V gain (16-bit results) */

  adcResult = ads.readADC_SingleEnded(0);

  serial1.print("AIN0: "); 
  serial1.print(adcResult); 
  serial1.print("("); 
  serial1.print(adcResult * adcScaling); 
  serial1.println("mV)");

}

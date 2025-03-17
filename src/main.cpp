#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
TwoWire wire(PB9, PB8);

void setup() {
  Serial.begin(115200);
  Serial.println("Hello!");

  Serial.println("Getting single-ended reading from AIN0");
  Serial.println("ADC Range: +/- 0.256V");

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
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  int16_t results;

  /* Be sure to update this value based on the IC and the gain settings! */
  float multiplier = 0.1875F/16; /* ADS1115  @ +/- 0.256V gain (16-bit results) */

  results = ads.readADC_SingleEnded(0);

  Serial.print("AIN0: "); Serial.print(results); Serial.print("("); Serial.print(results * multiplier); Serial.println("mV)");

  delay(1000);
}


/*
	Copyright (c) 2015, Peter Easton, Whizoo
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.
	* Neither Whizoo nor Peter Easton may be used to endorse or promote products
	derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL PETER EASTON BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

 */

/*
 MSGEQ7 Demo app
 Look for the breakout board on www.whizoo.com
 
 This code runs on an Arduino Duemilanove, but will run on other Arduino models.
 
 Connections:
 - GND to GND on MSGEQ7 breakout board, and LED's
 - 5V to VDD on MSGEQ7 breakout board
 - A0 to OUT on MSGEQ7 breakout board
 - D7 to STROBE on MSGEQ7 breakout board
 - D8 to RESET on MSGEQ7 breakout board
 - D3 to LED 0 (indicator for frequency band 0)
 - D5 to LED 1 (indicator for frequency band 1)
 - D6 to LED 2 (indicator for frequency band 2)
 - D9 to LED 3 (indicator for frequency band 3)
 - D10 to LED 4 (indicator for frequency band 4)
 - D11 to LED 5 (indicator for frequency band 5)
 
*/

// Hardware-specific defines
#define MSGEQ7_STROBE_PIN      7
#define MSGEQ7_RESET_PIN       8
#define MSGEQ7_ANALOG_PIN      A0


#define NUM_FREQUENCY_BANDS    7


// Duemilanove only has 6 PWM outputs, so the last LED won't respond properly.  Your
// board may have more PWM outputs.  Typically you only want to monitor the lowest
// frequency bands because that is where the beat is.
int led[NUM_FREQUENCY_BANDS] = {3, 5, 6, 9, 10, 11, 0};

// There is a concept of "persistence of vision" with LED's.  The LED has to be on long enough
// for the eye to recognise that it is on.  When a high volume is received on a frequency band,
// The LED is turned on (at a high PWM value) and then gradually faded until the next beat in
// that frequency.
int ledPWMValue[NUM_FREQUENCY_BANDS] = {0, 0, 0, 0, 0, 0, 0};
  

void setup() {
  // Set the LED pins as outputs
  for (int i=0; i<NUM_FREQUENCY_BANDS; i++)
    pinMode(led[i], OUTPUT);
  
  // Set up the MSGEQ7 IC
  pinMode(MSGEQ7_ANALOG_PIN, INPUT);
  pinMode(MSGEQ7_STROBE_PIN, OUTPUT);
  pinMode(MSGEQ7_RESET_PIN, OUTPUT);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
}


// This loop executes around 100 times per second
void loop() {
  int frequencyBandVolume;
  
  // Toggle the RESET pin of the MSGEQ7 to start reading from the lowest frequency band
  digitalWrite(MSGEQ7_RESET_PIN, HIGH);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  
  // Read the volume in every frequency band from the MSGEQ7
  for (int i=0; i<NUM_FREQUENCY_BANDS; i++) {
    digitalWrite(MSGEQ7_STROBE_PIN, LOW);
    delayMicroseconds(30); // Allow the output to settle
    frequencyBandVolume = analogRead(MSGEQ7_ANALOG_PIN);
    digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
    
    // The read value is 10-bit (0 to 1024).  PWM needs a value from 0 to 255, so divide by 4
    frequencyBandVolume = frequencyBandVolume >> 2;
    
    // Fade the current LED value for this band
    ledPWMValue[i] = ledPWMValue[i] > 7? ledPWMValue[i] - 7 : 0;
    
    // Don't show the lower values
    if (frequencyBandVolume > 70) {
      // If the new volume is greater than that currently being showed then show the higher volume
      if (frequencyBandVolume > ledPWMValue[i])
        ledPWMValue[i] = frequencyBandVolume;
    }
    
    // Set the LED PWM value to the frequency band's volume
    analogWrite(led[i],  ledPWMValue[i]);
  }
  
  // Wait before executing this loop again
  delay(10);
}



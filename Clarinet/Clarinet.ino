// ****************************************************************************
//       Sketch: WTriggerUno
// Date Created: 4/22/2015
//
//     Comments: Demonstrates basic serial control of the WAV Trigger from an
//               Arduino.
//
//  Programmers: Jamie Robertson, info@robertsonics.com
//
// ****************************************************************************
//
// To use this sketch, you'll need to:
//
// 1) Download and install the AltSoftSerial library.
// 2) Download and install the Metro library.
// 3) Connect 2 wires from the UNO to the WAV Trigger's serial connector:
//
//    Uno           WAV Trigger
//    ===           ===========
//    GND  <------> GND
//    Pin9 <------> RX
//
//    If you want to power the WAV Trigger from the Uno, then close the 5V
//    solder jumper on the WAV Trigger and connect a 3rd wire:
//
//    5V   <------> 5V
//
//    (If you are using an Arduino with extra hardware serial ports, such as
//    an Arduino Mega or Teensy, you don't need AltSoftSerial, and you should
//    edit the wavTrigger.h library file to select the desired serial port
//    according to the documentation contained in that file. And use the
//    appropriate TX/RX pin connections to your Arduino)
//
// 4) Download and install the demo wav files onto the WAV Trigger's microSD
//    card. You can find them here:
//
//    http://robertsonics.com/2015/04/25/arduino-serial-control-tutorial/
//
//    You can certainly use your own tracks instead, although the demo may
//    not make as much sense. If you do, make sure your tracks are at least 10
//    to 20 seconds long and have no silence at the start of the file.

#include <AltSoftSerial.h>    // Arduino build environment requires this
#include <Button.h>

#define LED 13                // our LED

// 5 buttons, debounced
Button button_debouncers[] = {Button(2), Button(3), Button(5), Button(7), Button(8)};

char buttons = 0;

// ****************************************************************************
void setup() {

  // Serial monitor
  Serial.begin(9600);

  // Wait for the serial port to start (max 3 seconds)
  while(!Serial && (millis() < 3000)) {}
  
  for (char i = 0; i < 5; i++)
  {
    button_debouncers[i].begin();
  }
  
  // Initialize the LED pin
  pinMode(LED, OUTPUT);

  // Setup the SPI device
  SPCR |= _BV(SPE);
  pinMode(SS, INPUT);

  Serial.println("Silent Orchestra - Clarinet");
  Serial.print("SS Pin "); Serial.println((int)SS);
  Serial.print("MISO Pin "); Serial.println((int)MOSI);
  pinMode(MISO, OUTPUT);
}

void loop()
{
  bool bUpdate = false;
  
  digitalWrite(LED, (millis() % 1000) < 500);

  for (char i = 0; i < 5; i++)
  {
    if (button_debouncers[i].pressed())
    {
      buttons |= (1 << i);
      Serial.print("Button "); Serial.print((int)i); Serial.println(" pressed");
      bUpdate = true;
    }
  
    if (button_debouncers[i].released())
    {
      buttons &= ~(1 << i);
      Serial.print("Button "); Serial.print((int)i); Serial.println(" released");
      bUpdate = true;
    }
  }
  
  SPDR = buttons;

  if (bUpdate)
  {
    Serial.print("Buttons: "); Serial.println((int)buttons);
  }
}

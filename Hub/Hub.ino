#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <AltSoftSerial.h>
#include <wavTrigger.h>

#define LEDS_PER_STRIP 18
#define KEYS_PER_INSTRUMENT 5

#define PIXEL_COUNT 15 * LEDS_PER_STRIP // 15 strips of LED_PER_STRIP LEDs each
#define PIXEL_PIN 7 // Attached Neopixels to this pin

#define RED 32, 0, 0
#define ORANGE 32, 12, 0
#define YELLOW 32, 32, 0
#define GREEN 0, 32, 0
#define BLUE 0, 0, 32

// These numbers are how we refer to instruments in the code.
#define CLARINET 0
#define HARP 1
#define XYLOPHONE 2

Adafruit_NeoPixel ledStrip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

wavTrigger wTrig;             // Our WAV Trigger object (uses pin 9)

const int clarinetSelectPin = A0; // Set this pin LOW to select the clarinet on SPI
const int harpSelectPin = A1; // Set this pin LOW to select the harp on SPI
const int xyloSelectPin = A2; // Set this pin LOW to select the xylophone on SPI

// These 6 variables hold the values from the SPI for each instrument.
// We store the old and the new value so we can tell when a value changes
char oldClarinet = 0;
char newClarinet = 0;
char oldHarp = 0;
char newHarp = 0;
char oldXylo = 0;
char newXylo = 0;

// If testMode is true, the code will ignore values from SPI and use serial instead.
// This is so we can test the hub without needing all the instruments connected
bool testMode = false;

// The "rainbow" is a 3x5 array of colours
const char rainbow[5][3] = {
  {RED}, {ORANGE}, {YELLOW}, {GREEN}, {BLUE},
};

// This function turns on a single LED strip
static void setStrip(char instrument, char substrip)
{
  int start = (instrument * KEYS_PER_INSTRUMENT * LEDS_PER_STRIP) + (substrip * LEDS_PER_STRIP);
  ledStrip.fill(ledStrip.Color(rainbow[substrip][0], rainbow[substrip][1], rainbow[substrip][2]), start, LEDS_PER_STRIP);
  ledStrip.show();
}

// This function turns off a single LED strip
static void clearStrip(char instrument, char substrip)
{
  int start = (instrument * KEYS_PER_INSTRUMENT * LEDS_PER_STRIP) + (substrip * LEDS_PER_STRIP);
  ledStrip.fill(ledStrip.Color(0, 0, 0), start, LEDS_PER_STRIP);
  ledStrip.show();
}

// Receives SPI input and turns the LEDs on/off and plays WAV fles
static void handle_spi_input(char spi_input, char * oldvalue, char instrument)
{
  static char * INSTRUMENT_NAMES[3] = {"clarinet", "harp", "xylophone"};
  int start_number = instrument * KEYS_PER_INSTRUMENT;

  for (char i = 0; i < 5; i++)
  {
    char mask = (1 << i);
    if ((*oldvalue & mask) != (spi_input & mask))
    {
      // Button state has changed
      if (spi_input & mask)
      {
        // Button pressed, play this sound
        setStrip(instrument, i); // Do LEDs before WAV, so interrupts don't get turned off. This is important.
        wTrig.trackPlayPoly(start_number + i + 1);
        Serial.print("Playing "); Serial.print(INSTRUMENT_NAMES[instrument]); Serial.print(" sound ");
        Serial.println((int)(start_number + i+ 1));
      }
      else
      {
        // Button pressed, stop this sound
        clearStrip(instrument, i); // Do LEDs before WAV, so interrupts don't get turned off. This is important.
        wTrig.trackStop(start_number + i+ 1);
        Serial.print("Stopping "); Serial.print(INSTRUMENT_NAMES[instrument]); Serial.print(" sound ");
        Serial.println((int)(start_number + i+ 1));
      }
    }
  }
  *oldvalue = spi_input;
}

void setup() {

  // Serial monitor
  Serial.begin(9600);

  // Wait for the serial port to start (max 3 seconds)
  while(!Serial && (millis() < 3000)) {}

  Serial.println("Silent Orchestra - Hub");
  Serial.print("SS Pins ");
  Serial.print((int)clarinetSelectPin); Serial.print(", ");
  Serial.print((int)harpSelectPin); Serial.print(", ");
  Serial.println((int)xyloSelectPin);
  Serial.print("MISO Pin "); Serial.println((int)MOSI);
  
  // WAV Trigger startup at 57600
  wTrig.start();
  wTrig.stopAllTracks();
  wTrig.samplerateOffset(0);
  
  // Setup SPI comms with the instruments
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  pinMode(clarinetSelectPin, OUTPUT);
  pinMode(harpSelectPin, OUTPUT);
  pinMode(xyloSelectPin, OUTPUT);
  digitalWrite(clarinetSelectPin, HIGH);
  digitalWrite(harpSelectPin, HIGH);
  digitalWrite(xyloSelectPin, HIGH);

  ledStrip.begin();
  ledStrip.show();
}

void loop()
{
  char spi_byte; // This byte will hold the received value from SPI

  // Select the clarinet and wait a bit for it to pick that up
  digitalWrite(clarinetSelectPin, LOW);
  delay(3);

  if (!testMode) // Only receive if not in test mode
  {
    spi_byte = SPI.transfer(255); // 255 is dummy byte
  }
  else
  {
    spi_byte = newClarinet;
  }

  // Deal with the received SPI byte for the clarinet
  handle_spi_input(spi_byte, &oldClarinet, CLARINET);

  // De-select the clarinet, select that harp
  // and wait a bit for it to pick that up
  digitalWrite(clarinetSelectPin, HIGH);
  digitalWrite(harpSelectPin, LOW);
  delay(3);

  if (!testMode) // Only receive if not in test mode
  {
    spi_byte = SPI.transfer(255); // 255 is dummy byte
  }
  else
  {
    spi_byte = newHarp;
  }

  // Deal with the received SPI byte for the harp
  handle_spi_input(spi_byte, &oldHarp, HARP);

  // De-select the harp, select that xylophone
  // and wait a bit for it to pick that up
  digitalWrite(harpSelectPin, HIGH);
  digitalWrite(xyloSelectPin, LOW);
  delay(3);

  if (!testMode) // Only receive if not in test mode
  {
    spi_byte = SPI.transfer(255); // 255 is dummy byte
  }
  else
  {
    spi_byte = newXylo;
  }

  // Deal with the received SPI byte for the xylophone
  handle_spi_input(spi_byte, &oldXylo, XYLOPHONE);
  digitalWrite(xyloSelectPin, HIGH);
}

/* This code lets us test the hub by sending fake buttons presses
to the software.

Open Serial Monitor and send 1-15 to turn that button on.
Send the same number to turn it off again.
*/
void serialEventRun()
{
  static String recvd = "";

  while (Serial.available())
  {
    char c = Serial.read();

    if (c == '\n')
    {
      if (testMode)
      {
        int selected = recvd.toInt();
        if ((selected >= 1) && (selected <= 5))
        {
          newClarinet ^= (1 << (selected - 1));
        }
        else if ((selected >= 6) && (selected <= 10))
        {
          newHarp ^= (1 << (selected - 6));
        }
        else if ((selected >= 11) && (selected <= 15))
        {
          newXylo ^= (1 << (selected - 11));
        }
      }

      if (recvd.equals("TEST"))
      {
        testMode = !testMode;
        Serial.print("Test mode ");
        Serial.println(testMode ? "on" : "off");
      }

      if (recvd.equals("?"))
      {
        Serial.print("Clarinet: "); Serial.println((int)oldClarinet);
        Serial.print("Harp: "); Serial.println((int)oldHarp);
        Serial.print("Xylophone: "); Serial.println((int)oldXylo);
      }
      recvd = "";
    }
    else
    {
      recvd += c;
    }
  }
}

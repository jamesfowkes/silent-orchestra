# silent-orchestra
A collection of Arduino sketches for the Silent Orchestra project

## Summary

The Silent Orchestra is a set of electronic "instruments", each based on an Arduino micro.

The Clarinet has five arcade-style buttons, each connected to a Digital Input.

The Harp has five metal "strings", which are connected to inputs of an MPR121 capactive touch sensor, interface to the Arduino over I2C.

The Xylophone has five push-to-make "keys" made from desoldering braid. These are each connected to a Digital Input.

Each Arduino is connected to a hub (another Arduino Micro) via SPI.

There is a string of 75 (5 * 5 * 3) neopixels attached to the hub. Each string of 5 neopixels represents a single input on an instrument.

The hub plays sounds via a WAV trigger (https://robertsonics.com/wav-trigger/)

## Communications

Each instrument constantly updates which of its inputs are being pressed/plucked/hit. This information is converted to a bitmap, where bits 0 to 4 are 1 if the respective input is active. This bitmap is written into the SPDR register of the Arduino.

The hub reads one byte from each instrument in turn over SPI. If a new button is registered as pressed:

1. The appropriate sound is played. The WAV trigger is controlled over serial.
2. The appropriate set of neopixels is turned on.

# Neopixels

On each instrument, the keys are associated with a colour. In order: Red, Orange, Yellow, Green Blue.
When the key is pressed, the neopixels associated with that key are lit in that colour.

Indexing LEDs from 0:

LEDS 0 through 24 are for the clarinet.
LEDS 25 through 50 are for the harp.
LEDS 51 through 75 are for the xylophone.

# Sounds 

The .wav tracks on the WAV trigger are named 001xxx.wav through 015xxx.wav (where xxx can be any text).

001 through 005 are for the clarinet.
006 through 010 are for the harp.
011 through 015 are for the xylophone.

The WAV trigger is capable of simultaneous polyphonic play of up to 14 tracks.


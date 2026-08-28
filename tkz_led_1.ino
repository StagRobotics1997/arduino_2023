// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define STRIPLENGTH_SHORT 20  // Popular NeoPixel ring size
#define STRIPLENGTH_LONG 50
#define ANTLER_LED_STRIP 20
#define SHORT_STRIP_COUNT 4
#define CHARGE_COLOR 0, 0, 255
#define WHITE 150,150,150
#define RED 255, 0,0
#define DIM_RED 10,0,0
#define BLUE 0,0,255

// Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(STRIPLENGTH_SHORT* SHORT_STRIP_COUNT + STRIPLENGTH_LONG + ANTLER_LED_STRIP, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 100  // Time (in milliseconds) to pause between pixels

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
}
void charge() {
  pixels.clear();  // Set all pixel colors to 'off'

  int diff = STRIPLENGTH_LONG - STRIPLENGTH_SHORT;
  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    pixels.setPixelColor(i, pixels.Color(CHARGE_COLOR));
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(CHARGE_COLOR));
      }
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
}


void shoot() {
  pixels.clear();  // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one
  int diff = STRIPLENGTH_LONG - STRIPLENGTH_SHORT;
  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(WHITE));
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(WHITE));
      }
      // pixels.setPixelColor(i + STRIPLENGTH_LONG - diff, pixels.Color(150, 150, 150));
      // pixels.setPixelColor(i + STRIPLENGTH_LONG + STRIPLENGTH_SHORT - diff, pixels.Color(150, 150, 150));
    }
  }
  pixels.show();  // Send the updated pixel colors to the hardware.

  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(RED));
    if (i > 0) {
      pixels.setPixelColor(i-1, pixels.Color(DIM_RED));
    }
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
           for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(RED));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff -1, pixels.Color(DIM_RED));
      }
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
  for (int i = 0; i < ANTLER_LED_STRIP; i++) {  // For each pixel...
    pixels.setPixelColor(i + STRIPLENGTH_LONG + STRIPLENGTH_SHORT * SHORT_STRIP_COUNT, pixels.Color(BLUE));
  }
  pixels.show();
  delay(3000);  // Pause before next pass through loop
  // Send the updated pixel colors to the hardware.
}

void idle() {
  pixels.clear();  // Set all pixel colors to 'off'

  int diff = STRIPLENGTH_LONG - STRIPLENGTH_SHORT;
  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    pixels.setPixelColor(i, pixels.Color(50, 50, 50, 50));
    pixels.setPixelColor(i + 1, pixels.Color(255, 255, 255, 255));
    pixels.setPixelColor(i + 2, pixels.Color(50, 50, 50, 50));
    if (i > 0) {
      pixels.setPixelColor(i - 1, pixels.Color(0, 0, 0, 0));
    }
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 1, pixels.Color(0, 0, 0, 0));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(50, 50, 50, 50));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff + 1, pixels.Color(255, 255, 255, 255));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff + 2, pixels.Color(50, 50, 50, 50));
      }
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(10);  // Pause before next pass through loop
  }
  for (int i = STRIPLENGTH_LONG; i > 0; i--) {  // For each pixel...

    pixels.setPixelColor(i, pixels.Color(50, 50, 50, 50));
    pixels.setPixelColor(i - 1, pixels.Color(255, 255, 255, 255));
    pixels.setPixelColor(i - 2, pixels.Color(50, 50, 50, 50));
    if (i > 0) {
      pixels.setPixelColor(i + 1, pixels.Color(0, 0, 0, 0));
    }
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff + 1, pixels.Color(0, 0, 0, 0));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(50, 50, 50, 50));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 1, pixels.Color(255, 255, 255, 255));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 2, pixels.Color(50, 50, 50, 50));
      }
    } else {
      pixels.fill(0, STRIPLENGTH_LONG - 2, STRIPLENGTH_LONG + (STRIPLENGTH_SHORT * SHORT_STRIP_COUNT));
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(10);  // Pause before next pass through loop
  }
}


void loop() {
  // idle();
  // shoot();
  charge();
}

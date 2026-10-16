/*
  NeoPixel "Pulse" Effect
  -----------------------
  Shows a bright pixel with dimmer pixels fading out on either side,
  like a comet, that moves back and forth (or in one direction) along
  the strip. The whole thing also "breathes" — the peak brightness
  itself pulses up and down over time.

  Wiring:
    NeoPixel DIN -> Arduino pin (default D6)
    NeoPixel 5V  -> 5V (use external supply for >8 pixels)
    NeoPixel GND -> GND (common ground with Arduino)

  Library: Adafruit NeoPixel (install via Library Manager)
*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN 3       // Data pin connected to NeoPixel
#define LED_COUNT 30    // Number of pixels in your strip
#define BRIGHTNESS 150  // Overall max brightness (0-255)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ---- Effect settings ----
const uint8_t COMET_WIDTH = 5;                         // how many pixels the falloff spans on each side
const uint8_t BASE_R = 0, BASE_G = 100, BASE_B = 255;  // color (cyan-blue). Change to taste.
const uint16_t MOVE_DELAY_MS = 60;                     // speed the bright center moves
const bool BOUNCE = true;                              // true = back and forth, false = wrap around

// ---- Breathing (overall pulse) settings ----
const uint16_t BREATH_PERIOD_MS = 2000;  // full breathe cycle length
const uint8_t BREATH_MIN = 60;           // minimum peak brightness (0-255)
const uint8_t BREATH_MAX = 255;          // maximum peak brightness (0-255)

int position = 0;
int direction = 1;
unsigned long lastMove = 0;

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();  // initialize all pixels to 'off'
}

void loop() {
  // 1. Compute current "breathing" peak brightness using a sine wave
  float phase = (millis() % BREATH_PERIOD_MS) / (float)BREATH_PERIOD_MS;  // 0.0 - 1.0
  float sineVal = (sin(phase * 2 * PI - PI / 2) + 1.0) / 2.0;             // 0.0 - 1.0 smooth wave
  uint8_t peakBrightness = BREATH_MIN + (uint8_t)(sineVal * (BREATH_MAX - BREATH_MIN));

  // 2. Draw the comet centered at 'position' with falloff over COMET_WIDTH pixels
  strip.clear();
  for (int i = -(int)COMET_WIDTH; i <= (int)COMET_WIDTH; i++) {
    int px = position + i;

    // handle wrap or skip out-of-range pixels
    if (BOUNCE) {
      if (px < 0 || px >= LED_COUNT) continue;
    } else {
      px = ((px % LED_COUNT) + LED_COUNT) % LED_COUNT;  // wrap around
    }

    // falloff: 1.0 at center, decreasing toward the edges
    float dist = abs(i) / (float)COMET_WIDTH;  // 0.0 (center) - 1.0 (edge)
    float falloff = 1.0 - dist;                // linear falloff
    falloff = falloff * falloff;               // square it for a softer, punchier look

    uint8_t r = (uint8_t)(BASE_R * falloff * (peakBrightness / 255.0));
    uint8_t g = (uint8_t)(BASE_G * falloff * (peakBrightness / 255.0));
    uint8_t b = (uint8_t)(BASE_B * falloff * (peakBrightness / 255.0));

    strip.setPixelColor(px, strip.Color(r, g, b));
  }
  strip.show();

  // 3. Move the comet position along the strip
  if (millis() - lastMove >= MOVE_DELAY_MS) {
    lastMove = millis();
    position += direction;

    if (BOUNCE) {
      if (position >= LED_COUNT - 1 || position <= 0) {
        direction *= -1;  // reverse at the ends
      }
    } else {
      if (position >= LED_COUNT) position = 0;
      if (position < 0) position = LED_COUNT - 1;
    }
  }
}

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define POT_PIN A1
#define LED_PIN A5

#define NUM_LEDS 60

Adafruit_NeoPixel _strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t RED = _strip.Color(50, 0, 0);
const uint32_t GREEN = _strip.Color(0, 50, 0);
const uint32_t BLUE = _strip.Color(0, 0, 50);
const uint32_t YELLOW = _strip.Color(50, 50, 0);
const uint32_t MAGENTA = _strip.Color(50, 0, 50);
const uint32_t CYAN = _strip.Color(0, 50, 50);
const uint32_t WHITE = _strip.Color(50, 50, 50);
const uint32_t OFF = _strip.Color(0, 0, 0);

void setup()
{
    _strip.begin();
    _strip.show();
}

void loop() 
{

    ping(RED, 25);
    ping(GREEN, 25);
    ping(BLUE, 25);
    ping(YELLOW, 25);
    ping(MAGENTA, 25);
    ping(CYAN, 25); 
    ping(WHITE, 25);
}

void ping(uint32_t color, uint8_t delayMs)
{
    for(uint32_t i=0; i<_strip.numPixels(); i++)
    {
        _strip.setPixelColor(i, color);
        _strip.show();
        delay(delayMs);
        _strip.setPixelColor(i, OFF);
    }
}


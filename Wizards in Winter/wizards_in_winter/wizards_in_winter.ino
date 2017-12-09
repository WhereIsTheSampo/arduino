#include <Adafruit_NeoPixel.h>  //Library to simplify interacting with the LED strand
#ifdef __AVR__
#include <avr/power.h>   //Includes the library for power reduction registers if your chip supports them. 
#endif    

#define PIN_GATE_IN          2
#define PIN_ENVELOPE_IN     A5
#define PIN_KNOB_IN         A2

#define PIN_NEO_OUT          7
#define NEO_LENGTH          60

#define INITIAL_NOISE_FLOOR 25
#define MAX_BRIGHTNESS      25

Adafruit_NeoPixel _strip = Adafruit_NeoPixel(NEO_LENGTH, PIN_NEO_OUT, NEO_GRB + NEO_KHZ800);

const uint32_t RED = _strip.Color(MAX_BRIGHTNESS, 0, 0);
const uint32_t GREEN = _strip.Color(0, MAX_BRIGHTNESS, 0);
const uint32_t BLUE = _strip.Color(0, 0, MAX_BRIGHTNESS);
const uint32_t YELLOW = _strip.Color(MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
const uint32_t MAGENTA = _strip.Color(MAX_BRIGHTNESS, 0, MAX_BRIGHTNESS);
const uint32_t CYAN = _strip.Color(0, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
const uint32_t WHITE = _strip.Color(MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
const uint32_t OFF = _strip.Color(0, 0, 0);

void setup()
{
    Serial.begin(9600);

    pinMode(PIN_GATE_IN, INPUT);
    pinMode(PIN_NEO_OUT, OUTPUT);

    Serial.println("Initialized");

    _strip.begin();
    _strip.show();
}

float _volume = 0;
float _noiseFloor = INITIAL_NOISE_FLOOR;
float _avgVolume = 0;
float _maxVolume = INITIAL_NOISE_FLOOR;


void ProcessVolume()
{
    //float knob = analogRead(PIN_KNOB_IN);
    _noiseFloor = INITIAL_NOISE_FLOOR; //knob / 1023.0 * 255;
    
    _volume = analogRead(PIN_ENVELOPE_IN);
    
    // clip volume to zero if it's sufficiently quiet
    if (_volume < _avgVolume / 2 || _volume < _noiseFloor)
    {
        _volume = 0;
    }
    // if we have a legit volume, calculate the rolling average
    else
    {
        _avgVolume = (_avgVolume * 4.0 + _volume) / 5.0;
    } 

    _maxVolume = (max(_maxVolume, _volume) * 99.0 + _volume) / 100.0;

    Serial.println("Floor: " + String(_noiseFloor) + ", Vol: " + String(_volume) + ", Avg: " + String(_avgVolume) + ", Max: " + String(_maxVolume));
}

void loop()
{
    ProcessVolume();
    
    VisualizeMagnitude();
}

void VisualizeMagnitude()
{
    int maxPixel = _volume / (_maxVolume + 1) * NEO_LENGTH;

    for (int i = 0; i < maxPixel; i++)
    {
        _strip.setPixelColor(i, RED);
    }

    for (int i = maxPixel; i < NEO_LENGTH; i++)
    {
        _strip.setPixelColor(i, OFF);
    }
    _strip.show();
}


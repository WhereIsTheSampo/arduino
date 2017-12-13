// Wizards in Winter for Advent Calendar House by Bob Reid

// Inspired by and adapted from SparkFun Addressable RGB LED Sound
// and Music Visualizer Tutorial Arduino Code by Michael Bartlett

// ##### Libraries #####

#include <Adafruit_NeoPixel.h>  //Library to simplify interacting with the LED strip
#ifdef __AVR__
#include <avr/power.h>   //Includes the library for power reduction registers if your chip supports them. 
#endif                   //More info: http://www.nongnu.org/avr-libc/user-manual/group__avr__power.htlm


// ##### Constants #####

const uint16_t LED_PIN        = A5;  // Signal pin for LED strip
const uint16_t LED_LENGTH     = 60;  // Total # of LEDs in strip
const uint16_t LED_HALF       = LED_LENGTH / 2;  // Half of LED strip
const uint16_t LED_OFFSET     = 7;   // Offset center point of strip to fit installation
const uint16_t ENVELOPE_PIN   = A0;  // Pin receiving output of sound detector
const uint16_t KNOB_PIN       = A1;  // Pin receiving output of sensitivity pot
const uint16_t MAX_BRIGHTNESS = 25;  // Max LED brightness (0-255)
const uint16_t NOISE_FLOOR    = 15;  // Minimum volume threshold


// not a constant, but used to create constants
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_LENGTH, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t RED     = strip.Color(MAX_BRIGHTNESS, 0, 0);
const uint32_t GREEN   = strip.Color(0, MAX_BRIGHTNESS, 0);
const uint32_t BLUE    = strip.Color(0, 0, MAX_BRIGHTNESS);
const uint32_t YELLOW  = strip.Color(MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
const uint32_t MAGENTA = strip.Color(MAX_BRIGHTNESS, 0, MAX_BRIGHTNESS);
const uint32_t CYAN    = strip.Color(0, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
const uint32_t WHITE   = strip.Color(MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
const uint32_t OFF     = strip.Color(0, 0, 0);


// ##### Globals #####

float volume = 0.0;
float avg_volume = 0.0; 
float max_volume = 0.0;
float knob = 0.0;
bool bump = false;     //Used to pass if there was a "bump" in volume
uint16_t gradient = 0; //Used to iterate and loop through each color palette gradually
//  This array holds the "threshold" of each color function (i.e. the largest number they take before repeating).
//  The values are in the same order as in ColorPalette()'s switch case (Rainbow() is first, etc). This is simply to
//  keep "gradient" from overflowing, the color functions themselves can take any positive value. For example, the
//  largest value Rainbow() takes before looping is 1529, so "gradient" should reset after 1529, as listed.
//     Make sure you add/remove values accordingly if you add/remove a color function in the switch-case in ColorPalette().
uint16_t thresholds[] = {1529, 1019, 764, 764, 764, 1274};
uint8_t palette = 0;  //Holds the current color palette.


//For Traffic() visual
int8_t pos[LED_LENGTH] = { -2};    //Stores a population of color "dots" to iterate across the LED strip.
uint8_t rgb[LED_LENGTH][3] = {0};  //Stores each dot's specific RGB values.

float time_bump = 0; //Holds the time (in runtime seconds) the last "bump" occurred.
float avg_time = 0;  //Holds the "average" amount of time between each "bump" (used for pacing the dot's movement).

uint8_t last = 0;     //Holds the value of volume from the previous loop() pass.


// ##### Standard Functions #####

void setup()
{
    Serial.begin(9600);

    strip.begin();

    // quick test to ensure strip is working
    ColorWipe(WHITE, 10);
    ColorWipe(RED, 10);
    ColorWipe(GREEN, 10);
    ColorWipe(BLUE, 10);
    
    strip.show();
    
    Serial.println("Initialized");
}

void loop()
{
    volume = analogRead(ENVELOPE_PIN);    //Record the volume level from the sound detector
    knob = analogRead(KNOB_PIN) / 1023.0; //Record how far the trimpot is twisted

    if (volume < avg_volume / 2.0 || volume < NOISE_FLOOR)
    {
        volume = 0.0;
    }
    else
    {
        avg_volume = (avg_volume + volume ) / 2.0;
    }
    
    max_volume = max(max_volume, volume);



  //This is where "gradient" is modulated to prevent overflow.
  if (gradient > thresholds[palette]) {
    gradient %= thresholds[palette] + 1;

    //Everytime a palette gets completed is a good time to readjust "maxVol," just in case
    //  the song gets quieter; we also don't want to lose brightness intensity permanently
    //  because of one stray loud sound.
    max_volume = (max_volume + volume) / 2.0;
  }

  //If there is a decent change in volume since the last pass, average it into "avgBump"
  if (volume - last > 10) avg_time = (avg_time + (volume - last)) / 2.0;

  //If there is a notable change in volume, trigger a "bump"
  //  avgbump is lowered just a little for comparing to make the visual slightly more sensitive to a beat.
  bump = (volume - last > avg_time * .9);  

  //If a "bump" is triggered, average the time between bumps
  if (bump) {
    avg_time = (((millis() / 1000.0) - time_bump) + avg_time) / 2.0;
    time_bump = millis() / 1000.0;
  }
  
    //VisualizeMagnitude();
    //VisualizePalettePulse();
    VisualizeTraffic();

      gradient++;    //Increments gradient

  last = volume; //Records current volume for next pass

  delay(30);     //Paces visuals so they aren't too fast to be enjoyable
}


// ##### Visualizations #####

void VisualizeMagnitude()
{
    int max_pixel = volume / max_volume * LED_LENGTH;

    for (int i = 0; i < max_pixel; i++)
    {
        strip.setPixelColor(i, RED);
    }

    for (int i = max_pixel; i < LED_LENGTH; i++)
    {
        strip.setPixelColor(i, OFF);
    }
    
    strip.show();

    Serial.println(
        "Vol: " + String(volume) + 
        ", Avg: " + String(avg_volume) + 
        ", Max: " + String(max_volume) + 
        ", Pixel: " + String(max_pixel));
}

void VisualizePalettePulse() {
  fade(0.75);
  if (bump) gradient += thresholds[palette] / 24;
  if (volume > 0) {
    int start = LED_HALF - (LED_HALF * (volume / max_volume));
    int finish = LED_HALF + (LED_HALF * (volume / max_volume)) + strip.numPixels() % 2;
    for (int i = start; i < finish; i++) {
      uint32_t shifted = shiftPixel(i);
      
      float damp = sin((i - start) * PI / float(finish - start));
      damp = pow(damp, 2.0);

      //This is the only difference from Pulse(). The color for each pixel isn't the same, but rather the
      //  entire gradient fitted to the spread of the pulse, with some shifting from "gradient".
      int val = thresholds[palette] * (i - start) / (finish - start);
      val += gradient;
      uint32_t col = ColorPalette(val);

      uint32_t col2 = strip.getPixelColor(shifted);
      uint8_t colors[3];
      float avgCol = 0, avgCol2 = 0;
      for (int k = 0; k < 3; k++) {
        colors[k] = split(col, k) * damp * knob * pow(volume / max_volume, 2);
        avgCol += colors[k];
        avgCol2 += split(col2, k);
      }
      avgCol /= 3.0, avgCol2 /= 3.0;
      if (avgCol > avgCol2) strip.setPixelColor(shifted, strip.Color(colors[0], colors[1], colors[2]));
    }
  }
  strip.show();
}

//Dots racing into each other
void VisualizeTraffic() {

  //fade() actually creates the trail behind each dot here, so it's important to include.
  fade(0.8);

  //Create a dot to be displayed if a bump is detected.
  if (bump) {

    //This mess simply checks if there is an open position (-2) in the pos[] array.
    int8_t slot = 0;
    for (slot; slot < sizeof(pos); slot++) {
      if (pos[slot] < -1) break;
      else if (slot + 1 >= sizeof(pos)) {
        slot = -3;
        break;
      }
    }

    //If there is an open slot, set it to an initial position on the strip.
    if (slot != -3) {

      //Evens go right, odds go left, so evens start at 0, odds at the largest position.
      pos[slot] = (slot % 2 == 0) ? -1 : strip.numPixels();

      //Give it a color based on the value of "gradient" during its birth.
      uint32_t col = ColorPalette(-1);
      gradient += thresholds[palette] / 24;
      for (int j = 0; j < 3; j++) {
        rgb[slot][j] = split(col, j);
      }
    }
  }

  //Again, if it's silent we want the colors to fade out.
  if (volume > 0) {

    //If there's sound, iterate each dot appropriately along the strip.
    for (int i = 0; i < sizeof(pos); i++) {

      //If a dot is -2, that means it's an open slot for another dot to take over eventually.
      if (pos[i] < -1) continue;

      //As above, evens go right (+1) and odds go left (-1)
      pos[i] += (i % 2) ? -1 : 1;

      //Odds will reach -2 by subtraction, but if an even dot goes beyond the LED strip, it'll be purged.
      if (pos[i] >= strip.numPixels()) pos[i] = -2;

      //Set the dot to its new position and respective color.
      //  I's old position's color will gradually fade out due to fade(), leaving a trail behind it.
      strip.setPixelColor( shiftPixel(pos[i]), strip.Color(
                              float(rgb[i][0]) * pow(volume / max_volume, 2.0) * knob,
                              float(rgb[i][1]) * pow(volume / max_volume, 2.0) * knob,
                              float(rgb[i][2]) * pow(volume / max_volume, 2.0) * knob)
                          );
    }
  }
  strip.show(); //Again, don't forget to actually show the lights!
}


// ##### Helper Functions #####

//Fades lights by multiplying them by a value between 0 and 1 each pass of loop().
void fade(float damper) {

  //"damper" must be between 0 and 1, or else you'll end up brightening the lights or doing nothing.

  for (int i = 0; i < strip.numPixels(); i++) {

    //Retrieve the color at the current position.
    uint32_t col = strip.getPixelColor(i);

    //If it's black, you can't fade that any further.
    if (col == 0) continue;

    float colors[3]; //Array of the three RGB values

    //Multiply each value by "damper"
    for (int j = 0; j < 3; j++) colors[j] = split(col, j) * damper;

    //Set the dampened colors back to their spot.
    strip.setPixelColor(i, strip.Color(colors[0] , colors[1], colors[2]));
  }
}

//This function calls the appropriate color palette based on "palette"
//  If a negative value is passed, returns the appropriate palette withe "gradient" passed.
//  Otherwise returns the color palette with the passed value (useful for fitting a whole palette on the strip).
uint32_t ColorPalette(float num) {
  switch (palette) {
    case 0: return (num < 0) ? Rainbow(gradient) : Rainbow(num);
    default: return Rainbow(gradient);
  }
}

//As mentioned above, split() gives you one 8-bit color value
//from the composite 32-bit value that the NeoPixel deals with.
//This is accomplished with the right bit shift operator, ">>"
uint8_t split(uint32_t color, uint8_t i ) {

  //0 = Red, 1 = Green, 2 = Blue

  if (i == 0) return color >> 16;
  if (i == 1) return color >> 8;
  if (i == 2) return color >> 0;
  return -1;
}

uint32_t shiftPixel(uint32_t original) 
{
    return ( original + 60 - LED_OFFSET ) % LED_LENGTH;
}

//These functions simply take a value and return a gradient color
//  in the form of an unsigned 32-bit integer

//The gradients return a different, changing color for each multiple of 255
//  This is because the max value of any of the 3 RGB values is 255, so it's
//  an intuitive cutoff for the next color to start appearing.
//  Gradients should also loop back to their starting color so there's no jumps in color.

uint32_t Rainbow(unsigned int i) {
  if (i > 1529) return Rainbow(i % 1530);
  if (i > 1274) return strip.Color(255, 0, 255 - (i % 255));   //violet -> red
  if (i > 1019) return strip.Color((i % 255), 0, 255);         //blue -> violet
  if (i > 764) return strip.Color(0, 255 - (i % 255), 255);    //aqua -> blue
  if (i > 509) return strip.Color(0, 255, (i % 255));          //green -> aqua
  if (i > 255) return strip.Color(255 - (i % 255), 255, 0);    //yellow -> green
  return strip.Color(255, i, 0);                               //red -> yellow
}

// ##### Testing Functions #####

// Fill the dots one after the other with a color
void ColorWipe(uint32_t color, int wait) {
  for(int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}
  


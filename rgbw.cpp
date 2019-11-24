// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN     0

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  7

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void fillOuter(uint32_t color, int wait) {
  for(int i=1;i<strip.numPixels();i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

struct vec3;

void ramp_all(vec3 color, int n_step, float total_time, float br_begin, float br_end);


struct vec3 {
  float data[3];

  vec3(float a, float b, float c) {
    data[0] = a;
    data[1] = b;
    data[2] = c;
  }

  vec3(const vec3& other) = default;

  vec3 add(const vec3& other) const {
    vec3 r{*this};
    r.data[0] += other.data[0];
    r.data[1] += other.data[1];
    r.data[2] += other.data[2];
    return r;
  }

  vec3 mult(float f) const {
    vec3 r = *this;
    r.data[0] *= f;
    r.data[1] *= f;
    r.data[2] *= f;
    return r;
  }

  vec3 operator*(float f) {
    vec3 r = *this;
    return r.mult(f);
  }

  vec3 operator-() const {
    return mult(-1);
  }

  vec3 operator*(int f) {
    vec3 r = *this;
    return r.mult(f);
  }

  vec3 operator+(const vec3& other) const {
    return add(other);
  }
  
  vec3 operator-(const vec3& other) const {
    return add(other.mult(-1));
  }

  float norm() const {
    return sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
  }

  uint32_t color(float b = 1.0) const {
    int m = 255;
    return strip.gamma32(strip.Color(data[0]*b*m, data[1]*b*m, data[2]*b*m, 0));
  }

  void print() const {
    //Serial.print("color: ");
    //Serial.print(data[0]);
    //Serial.print(" ");
    //Serial.print(data[1]);
    //Serial.print(" ");
    //Serial.print(data[2]);
    //Serial.print("\n");
  }
};

void ramp_all(vec3 color, int n_step, float total_time, float br_begin, float br_end) {
  float br_step = (br_end - br_begin) / (float)n_step;

  for (int i=0;i<=n_step;i++) {
    // loop over steps
    for (int p=0;p<strip.numPixels();p++) {
      vec3 cur_color = color.mult(br_begin + br_step * i);
      strip.setPixelColor(p, cur_color.color());
      strip.show();
    }
    delay(total_time/n_step);
  }
}

int n_colors = 6;
struct vec3 colors[6] = {
  vec3{1, 0, 0}, 
  vec3{0, 1, 0},
  vec3{0, 0, 1},
  vec3{215, 3, 252},
  vec3{255, 247, 0},
  vec3{0, 255, 225}
};
int color_idx = 0;

void loop() {

  // float r = random(0, 99) / 100.;
  // float g = random(0, 99) / 100.;
  // float b = random(0, 99) / 100.;
  vec3 base = colors[color_idx];

  color_idx = (color_idx+1) % n_colors;

  // vec3 base{r, g, b};
  base = base * (1/base.norm());


  // fillOuter(strip.Color(100, 0, 0), 100);
  // vec3 base{1, 0, 0};
  float br_begin = 0.2;
  float br_end = 1.0;
  fillOuter(base.color(br_begin), 60);


  // ramp
  int n_step = 30;
  float total_time = 50;

  ramp_all(base, n_step, total_time, br_begin, br_end);
  ramp_all(base, n_step, 500, br_end, 0);

  // for (int i=0;i<n_step;i++) {
    // // loop over steps
    // for (int p=0;p<strip.numPixels();p++) {
      // vec3 cur_color = base.mult(br_begin + br_step * i);
      // strip.setPixelColor(p, cur_color.color());
      // strip.show();
    // }
    // delay(total_time/n_step);
  // }
  // delay(500);

  // for (int p=0;p<strip.numPixels();p++) {
    // strip.setPixelColor(p, vec3{0,0,0}.color());
    // strip.show();
  // }

  // fillOuter(strip.Color(0, 0, 0), 0);
  delay(200);


  /* fillOuter(strip.Color(0, 0, 0), 100); */

  // Fill along the length of the strip in various colors...
  /* colorWipe(strip.Color(255,   0,   0)     , 50); // Red */
  /* colorWipe(strip.Color(  0, 255,   0)     , 50); // Green */
  /* colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue */
  /* colorWipe(strip.Color(  0,   0,   0, 255), 50); // True white (not RGB white) */

  /* whiteOverRainbow(75, 5); */

  /* pulseWhite(5); */

  /* rainbowFade2White(3, 3, 1); */
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void whiteOverRainbow(int whiteSpeed, int whiteLength) {

  if(whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;) { // Repeat forever (or until a 'break' or 'return')
    for(int i=0; i<strip.numPixels(); i++) {  // For each pixel in strip...
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) {
        strip.setPixelColor(i, strip.Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    }

    strip.show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if(++head >= strip.numPixels()) {      // Advance head, wrap around
        head = 0;
        if(++loopNum >= loops) return;
      }
      if(++tail >= strip.numPixels()) {      // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}

void pulseWhite(uint8_t wait) {
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}

void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops) {
  int fadeVal=0, fadeMax=100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(wait);

    if(firstPixelHue < 65536) {                              // First loop,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Last loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } else {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }

  for(int k=0; k<whiteLoops; k++) {
    for(int j=0; j<256; j++) { // Ramp up 0 to 255
      // Fill entire strip with white at gamma-corrected brightness level 'j':
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
    delay(1000); // Pause 1 second
    for(int j=255; j>=0; j--) { // Ramp down 255 to 0
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
  }

  delay(500); // Pause 1/2 second
}

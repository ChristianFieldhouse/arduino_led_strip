#include <FastLED.h>
//#include <Serial.h>

#define button_pin 4
#define BUTTON_WAIT 500
int button;
long last_button_press;

#define LED_PIN     5
#define NUM_LEDS    174 
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define MODES 6
CRGB leds[NUM_LEDS];

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.



CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
int updates_per_second;

struct inbuff{
  CHSV next_col;
  int steps;
};

#include "Collider.h";
#include "ThreeCollider.h";
#include "Particles.h";

inbuff next_colour;
int mode;
int chunk_length;
Collider mycollider = Collider(5);
ThreeCollider mythree = ThreeCollider(6, NUM_LEDS/4, NUM_LEDS/3, 3*NUM_LEDS/4);
Particles myparticles = Particles(1);

long mutation_point;
CRGB mutating_p[4];
int mutating_col;
CRGB mutation_target;
CRGB mutation_origin;
#define M_SAT 200

void setup() {
    updates_per_second = 100;
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    last_button_press = millis();
    
    Serial.begin(9600);

    next_colour = {CHSV( random8(), 255, random8()), 1};
    mode = 5;
    chunk_length = 20;

    mycollider = Collider(5);
    myparticles = Particles(3);

    mutating_p[0] = CHSV( random8(), M_SAT + (random8()%2) * (255 - M_SAT), 255);
    mutating_p[1] = CHSV( random8(), M_SAT + (random8()%2) * (255 - M_SAT), 255);
    mutating_p[2] = CHSV( random8(), M_SAT + (random8()%2) * (255 - M_SAT), 255);
    mutating_p[3] = CHSV( random8(), M_SAT + (random8()%2) * (255 - M_SAT), 255);
    mutation_point = millis();
    mutating_col = random8() % 4;
    mutation_target = CHSV( random8(), 255, 255);
    mutation_origin = mutating_p[mutating_col];
}

void loop()
{
   button = digitalRead(button_pin);
   //Serial.println(button);
   if (!button && millis() - last_button_press > BUTTON_WAIT){
    //mode = (mode + 1) % MODES;
    Serial.println("next mode");
    last_button_press = millis();
   }
    
    ChangePalettePeriodically();
    
    static uint8_t startIndex = 0;
    //Serial.println(updates_per_second);
    if (updates_per_second > 0){
      startIndex = startIndex + 1; /* motion speed */
      //Serial.println(mode);
      if (mode == 0){
        StepLEDArray();
      }
      else if (mode == 1){
        FillLEDsFromPaletteColors( startIndex);
      }
      else if (mode == 2){
        //Serial.println("mode 2");
        mycollider.step_leds();
      }
      else if(mode == 3){
        mythree.step_leds();
      }
      else if (mode == 4){
        myparticles.step_leds();
      }
      else if (mode == 5){
        const int static_time = 3000;
        const int mutation_time = 2000;
        if (millis() - mutation_point > mutation_time + static_time){
          mutation_point = millis();
          mutating_col = random8() % 4;
          mutation_target = CHSV( random8(), M_SAT + (random8()%2) * (255 - M_SAT), 255);
          mutation_origin = mutating_p[mutating_col];
        }
        else if (millis() - mutation_point < mutation_time){
          mutating_p[mutating_col] = interp(mutation_origin, mutation_target, float(millis() - mutation_point) / mutation_time);
        }
        for( int i = 0; i < 4; ++i) {
            currentPalette[4*i + 0] = mutating_p[i];
            currentPalette[4*i + 1] = mutating_p[i];
            currentPalette[4*i + 2] = mutating_p[i];
            currentPalette[4*i + 3] = mutating_p[i];
            FillLEDsFromPaletteColors( startIndex);
        }
      }
      //Serial.println(mode);
      FastLED.show();
      //Serial.println("show");
      FastLED.delay(1000 / updates_per_second);
      //Serial.println("delay_done");
    }
    //Serial.println("done mode thing");

    while(Serial.available()){
        char newbyte = Serial.read();
        switch (newbyte)
        {
          case '1':
            currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; break;
          case '2':
            currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND; break;
          case '3':
            currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; break;
          case '4':
            SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; break;
          case '5':
            SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; break;
          case'6':
            SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; break;
          case '7':
            SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; break;
          case '8':
            currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; break;
          case '9':
            currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; break;
          case '0':
            SetupKatiePalette();                      currentBlending = LINEARBLEND; break;
          case 'w':
            if (updates_per_second < 2){
              updates_per_second += 1; break;
            }
            updates_per_second = int(1.5f * updates_per_second);
            if (updates_per_second < 0 || updates_per_second > 10000){
              updates_per_second = 10000;
            }
            Serial.print("updates per second : ");
            Serial.println(updates_per_second);
            break;
          case 's':
            if (updates_per_second == 1){
              updates_per_second = 0; break;
            }
            updates_per_second = int(0.75f * updates_per_second);
            Serial.print("updates per second : ");
            Serial.println(updates_per_second);
            break;
          case 'l':
            mode = (mode + 1) % MODES;
            Serial.print("mode : ");
            Serial.println(mode);
            break;
          case 'i':
            chunk_length += 1; break;
          case 'k':
          
            chunk_length -= 1;
            if (chunk_length < 0){
              chunk_length = 0;
            }
            break;
          case 'r':
            myparticles = Particles(3); break;
        }
        Serial.println(newbyte);
        Serial.println(updates_per_second);
    }
}

CRGB interp(CRGB col0, CRGB col1, float a){
  const uint8_t r = uint8_t(float(col0.r) * (1 - a) + float(col1.r) * a);
  const uint8_t g = uint8_t(float(col0.g) * (1 - a) + float(col1.g) * a);
  const uint8_t b = uint8_t(float(col0.b) * (1 - a) + float(col1.b) * a);
  return CRGB(r, g, b);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void StepLEDArray(){
    for( int i = NUM_LEDS - 1; i > 0 ; --i) {
        leds[i] = leds[i - 1];
    }
    next_colour.steps -= 1;
    if (next_colour.steps == 0){
      leds[0] = next_colour.next_col;
      next_colour.steps = chunk_length;
      next_colour.next_col = CHSV( random8(), 255, 255);
    }
    else{
      leds[0] = leds[1];
    }
    //leds[0] = CHSV( random8(), 255, 255);
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;

    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 4; ++i) {
        const CRGB col = CHSV( random8(), 255, 255);
        currentPalette[4*i + 0] = col;
        currentPalette[4*i + 1] = col;
        currentPalette[4*i + 2] = col;
        currentPalette[4*i + 3] = col;
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

void SetupKatiePalette()
{
    CRGB pink = CHSV( HUE_PINK, 166, 255);
    CRGB green  = CHSV( HUE_GREEN, 166, 255);
    
    currentPalette = CRGBPalette16(
       pink, pink, pink, pink,
       green, green, green, green,
       pink, pink, pink, pink,
       green, green, green, green
    );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};



// Additional notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.

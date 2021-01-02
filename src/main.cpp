#include <Arduino.h>
#include <U8g2lib.h>            // For text on the little on-chip OLED
#include <FastLED.h>

#define OLED_CLOCK  15          // Pins for the OLED display
#define OLED_DATA   4
#define OLED_RESET  16

// How many leds in your strip?
#define NUM_LEDS 96 //96
#define DATA_PIN 5
#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

CRGB leds[NUM_LEDS];

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);

boolean direction = FORWARD;

CRGB randomColor();
CRGB Wheel(byte WheelPos);
void flash(CRGB c, int count, int speed);
void theaterChaseRainbow(int cycles, int speed);
void theaterChase(CRGB c, int cycles, int speed);
void lightning(CRGB c, int simultaneous, int cycles, int speed);
void cylon(CRGB c, int width, int speed);
void swirly(CRGB c, int width, int speed, bool reverse);
void stripes(CRGB c1, CRGB c2, int width);
void disolve(int simultaneous, int cycles, int speed);
void allRandom();
void colorWipe(CRGB c, int speed, int direction);
void rainbow(int cycles, int speed);

void setup() { 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  randomSeed(analogRead(0));
  Serial.begin(9600);
}

void loop() { 

  rainbow(0,MEDIUM);
  delay(3000);
  colorWipe(CRGB::Black,FORWARD,FAST);
  allRandom();
  delay(3000);
  disolve(15,100,MEDIUM);

  for(int i=0; i<3; i++){
    CRGB c1 = randomColor();
    CRGB c2 = randomColor();
    stripes(c1,c2,5);
    delay(2000);
    stripes(c2,c1,5);
    delay(2000);
  }

  for(int i=0; i<5; i++){
    cylon(randomColor(), 10, FAST);
  }

  for(int i=0; i<10; i++){
    swirly(randomColor(), 5,FAST, false);
  }

  lightning(NULL,15,50,MEDIUM);
  lightning(CRGB::White,20,50,MEDIUM);

  for(int i=0; i<3; i++){
    theaterChase(randomColor(),10,SLOW);
  }

  theaterChaseRainbow(1,MEDIUM);

  for(int i=0; i<10; i++){
    swirly(randomColor(), 5,MEDIUM, true);
  }

  rainbow(FAST,1);

  flash(randomColor(),10,SLOW);

  flash(NULL,50,MEDIUM);

  for(int i=0; i<2; i++){ 
    colorWipe(randomColor(),FAST,direction);
    direction = !direction;
  }


}

// Changes all LEDS to given color
void allColor(CRGB c){
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = c;
  }
  FastLED.show();
}

void allRandom(){
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = randomColor();
  }
  FastLED.show(); 
}

// Random disolve colors
void disolve(int simultaneous, int cycles, int speed){
  for(int i=0; i<cycles; i++){
    for(int j=0; j<simultaneous; j++){
      int idx = random(NUM_LEDS);
      leds[idx] = CRGB::Black;
    }
    FastLED.show();
    delay(speed);
  }

  allColor(CRGB::Black);
}

// Flashes given color
// If c==NULL, random color flash
void flash(CRGB c, int count, int speed){
  for(int i=0; i<count; i++){
    if(c){
      allColor(c);
    }
    else{
      allColor(randomColor());
    }
    delay(speed);
    allColor(CRGB::Black);
    delay(speed);
  }
}

// Wipes color from end to end
void colorWipe(CRGB c, int speed, int direction){
  for(int i=0; i<NUM_LEDS; i++){
    if(direction == FORWARD){
      leds[i] = c;
    }
    else{
      leds[NUM_LEDS-1-i] = c;
    }
    FastLED.show();
    delay(speed);
  }
}

// Rainbow colors that slowly cycle across LEDs
void rainbow(int cycles, int speed){ // TODO direction
  if(cycles == 0){
    for(int i=0; i< NUM_LEDS; i++) {
      leds[i] = Wheel(((i * 256 / NUM_LEDS)) & 255);
    }
    FastLED.show();
  }
  else{
    for(int j=0; j<256*cycles; j++) {
      for(int i=0; i< NUM_LEDS; i++) {
        leds[i] = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      }
      FastLED.show();
      delay(speed);
    }
  }
}

// Theater-style crawling lights
void theaterChase(CRGB c, int cycles, int speed){ // TODO direction

  for (int j=0; j<cycles; j++) {  
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        int pos = i+q;
        leds[pos] = c;    //turn every third pixel on
      }
      FastLED.show();

      delay(speed);

      for (int i=0; i < NUM_LEDS; i=i+3) {
        leds[i+q] = CRGB::Black;        //turn every third pixel off
      }
    }
  }
}

// Theater-style crawling lights with rainbow effect
void theaterChaseRainbow(int cycles, int speed){ // TODO direction, duration
  for (int j=0; j < 256 * cycles; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        int pos = i+q;
        leds[pos] = Wheel( (i+j) % 255);    //turn every third pixel on
      }
      FastLED.show();

      delay(speed);

      for (int i=0; i < NUM_LEDS; i=i+3) {
        leds[i+q] = CRGB::Black;  //turn every third pixel off
      }
    }
  }
}

// Random flashes of lightning
void lightning(CRGB c, int simultaneous, int cycles, int speed){
  int flashes[simultaneous];

  for(int i=0; i<cycles; i++){
    for(int j=0; j<simultaneous; j++){
      int idx = random(NUM_LEDS);
      flashes[j] = idx;
      leds[idx] = c ? c : randomColor();
    }
    FastLED.show();
    delay(speed);
    for(int s=0; s<simultaneous; s++){
      leds[flashes[s]] = CRGB::Black;
    }
    delay(speed);
  }
}

// Sliding bar across LEDs
void cylon(CRGB c, int width, int speed){
  // First slide the leds in one direction
  for(int i = 0; i <= NUM_LEDS-width; i++) {
    for(int j=0; j<width; j++){
      leds[i+j] = c;
    }

    FastLED.show();

    // now that we've shown the leds, reset to black for next loop
    for(int j=0; j<width; j++){
      leds[i+j] = CRGB::Black;
    }
    delay(speed);
  }

  // Now go in the other direction.  
  for(int i = NUM_LEDS-width; i >= 0; i--) {
    for(int j=0; j<width; j++){
      leds[i+j] = c;
    }
    FastLED.show();
    for(int j=0; j<width; j++){
      leds[i+j] = CRGB::Black;
    }

    delay(speed);
  }
}

// Continuously moving band of leds
void swirly(CRGB c, int width, int speed, bool reverse){
  // First slide the leds in one direction
  for(int i = 0; i <= NUM_LEDS; i++) {
    for(int j=0; j<width; j++){
      if(!reverse)
        leds[(i+j) % NUM_LEDS] = c;
      else
        leds[(NUM_LEDS-1-i+j) % NUM_LEDS] = c;
    }

    FastLED.show();

    // now that we've shown the leds, reset to black for next loop
    for(int j=0; j<width; j++){
      if(!reverse)
        leds[(i+j) % NUM_LEDS] = CRGB::Black;
      else
        leds[(NUM_LEDS-1-i+j) % NUM_LEDS] = CRGB::Black;
    }
    delay(speed);
  }
}

// Display alternating stripes
void stripes(CRGB c1, CRGB c2, int width){
  for(int i=0; i<NUM_LEDS; i++){
    if(i % (width * 2) < width){
      leds[i] = c1;
    }
    else{
      leds[i] = c2;
    } 
  }
  FastLED.show();
}

// Theater-style crawling of stripes
void stripesChase(CRGB c1, CRGB c2, int width, int cycles, int speed){ // TODO direction

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
CRGB Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

CRGB randomColor(){
  return Wheel(random(256)); 
}












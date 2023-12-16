// Thank you to : https://www.instructables.com/Magic-Rainbow-XmasParty-Lights-NeoPixel-Arduino/
// NB does not work in Arduino IDE

// NeoPixel Rainbow pixel string/Xmas tree LEDs

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN   10          // data to LED string
//switch pins
#define twi   4           // switch input for twinkle (low stops) or smooth option: 7 colours (low) or all
#define opt   9           // switch input for option - random or smooth

// How many NeoPixels are attached to the Arduino - best to keep this = 60.
// If more then connect 60 pixel strings in parallel.
#define NUMPIXELS      60

#define twinkle  // comment out to remove twinkle routine

//#define test    // uncomment out for test mode

// Select one of lines below depending on controller type 
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //WS2812
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ400); //WS2811
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800); //PL9823

int delayval = 20; // delay value ms, LED update approx NUMPIXELS*(delayval +4)/1000 s
int delayval2 = 20;   // additional delay if spd input low.  Ignore is switch not used.

uint32_t white = pixels.Color(255, 255, 255, 0);

uint32_t saved = 0;     // saved colour (twinkle)
int cnt = 0;        // counter for pixel to set to new colour

unsigned int rno = 0x45; // random9 seed
unsigned int cno;       // used to set colours, range 0-0x1FF
unsigned int index;     // used to set colours, range 0-0x1FF
int tw;                 // used in twinkle routine
int fade[NUMPIXELS];  //fade increment
int col[NUMPIXELS];    // holds colour values, shift>>6 to get index
int red;
int green;
int blue;                   // working RGB variables
int i;                      //for counter


unsigned int random9() {

  unsigned int newbit = (((rno >> 8) ^ (rno >> 4)) & 1);
  rno = ((rno << 1) | newbit) & 0x1FF;


//    uint16_t start = 0x1FF;  test
//    uint16_t a = start;
//
//    for(i = 1;; i++) {
//        int newbit = (((a >> 8) ^ (a >> 4)) & 1);
//        a = ((a << 1) | newbit) & 0x1FF;
//        Serial.println(a);
//        if (a == start) {
//          Serial.print("repetition period is ");
//            Serial.println(i);
//            break;
//        }
//    } // for

    return rno;
}



void setup() {

  pixels.begin(); // This initializes the NeoPixel library.
  //pixels.setBrightness(255);    // uncomment to set brightness (0 min to 255 max)
  pixels.show(); // Start all off
//  Serial.begin(115200);   // uncomment for debugging
  for(i=0;i<NUMPIXELS;i++){
    fade[i] = 0;
    col[i] = 0;
  }
  pinMode(twi,INPUT_PULLUP);
  pinMode(opt,INPUT_PULLUP);

  rno = random9();
}

void loop() {

  if(digitalRead(opt)==LOW){
    NeoSmth();
  }
  else {
    NeoRand();
  }

}   // loop()

void NeoSmth() {

  index = cnt;    // set index for 1st LED

  for(i=0;i<NUMPIXELS;i++){

    pixels.setPixelColor(i, Bow(index)); // set colour

    if(digitalRead(twi)==LOW){ index += 65; }  // gives sequence of rainbow colours
    else {index += 7;}           // gives 7 steps between LEDS - so 7 colours over 49 LEDs range on string.


    if(index>=(7*64)) {index -= 7*64; }
    
  }

//  Serial.print(cno);Serial.print(" ");
//  Serial.print(rno);Serial.print(" ");
//  Serial.print(red);Serial.print(" ");
//  Serial.print(green);Serial.print(" ");
//  Serial.println(blue);

  cnt += 1;   // next step

  if(cnt>=(7*64)) {cnt =0; }

  pixels.show();    // This sends the updated pixel colors to the string.

  delay(delayval);  // Delay (milliseconds).

  // Serial.println(millis());        // use for checking loop speed (delay plus code)
  
}

        //  Function to set RGB from index (0 to 7*64-1)
uint32_t Bow(int n) {

  cno = int(n/64);       // find LED colour (0-6)

  rno = n - (cno*64);    // index from LED colour to next colour (0-63)

  switch (cno) {          // update LED colours
      case 0:   // red
      red = 255;
      blue = 0;

      green = rno>>1;     // use shift for div 2
      break;
      
      case 1:   // orange
      red = 255;
      blue = 0;
      
      green = 32 + ((rno*3)>>1);
      break;
      
      case 2:     // yellow
      red = 255;
      blue = 0;
      
      green = 128 + (rno*6);

      if(green>255) {
        red -= (green-255);
        green = 255;
      }      
      break;
      
      case 3:     // green
      red = 0;
      green = 255;
      
      blue = rno*8;
      
      if(blue>255) {
        green -= (blue-255);
        blue = 255;
      }
      break;
      
      case 4:     // blue
      green = 0;
      blue = 255;
      
      red = rno;
      break;
      
      case 5:     // indigo
      green = 0;
      blue = 255;

      red = 64 + ((11*rno)>>1);

      if(red>255) {
        blue -= (red-255);
        red = 255;
      } 
      break;
      case 6:     // violet
      red = 255;
      green = 0;
      
      blue = 96 - ((rno*3)>>1);
  
      break;
      
      default:                  // should not get here
      red = 0;
      green = 0;
      blue = 0;
    }

  return pixels.Color(red, green, blue);
}

void NeoRand() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  for(i=0;i<NUMPIXELS;i++){

    #ifdef twinkle
        if(pixels.getPixelColor(i) == white) pixels.setPixelColor(i, saved); // return colour after sparkle
    #endif

    int c = i%7;            // c=c nominal colour 0-red, 1-orange, etc

    if(i==cnt){
      cno = random9();      // new colour setting (0 - 0x1FF)

      #ifdef test
        cno= 0x100;
      #endif

      fade[i] = cno - (col[i]>>6);   // new fade increment. 64 steps to reach cno
    }

    col[i] += fade[i];      // next setting

    index = col[i]>>6;      // range 0-0x1FF

    switch (c) {          // update LED colours
      case 0:   // red
      red = 255;
      green = 0;
      blue = 0;
      if(highByte(index)==0) {    // 0-0xFF
        blue = (0xFF-lowByte(index))>>1;   //max 127
      }
      else {                    // 0x100-0x1FF
        green = lowByte(index)>>2;  //max 63
      }
      break;
      
      case 1:   // orange
      red = 255;
      green = 32;
      blue = 0;
      if(highByte(index)==0) {    // 0-0xFF
        green -= (0xFF-lowByte(index))>>3;   //1 - 32
      }
      else {                    // 0x100-0x1FF
        green += lowByte(index)>>1;         //32 - 140
      }
      break;
      
      case 2:     // yellow
      red = 255;
      green = 127;
      blue = 0;
      if(highByte(index)==0) {    // 0-0xFF
        green -= (0xFF-lowByte(index))>>4;   //64 - 127
      }
      else {                    // 0x100-0x1FF
        green += lowByte(index);         //127 - 154
        if(green>0xFF) {
          red -= green -0xFF;
          green = 0xFF;
        }
      }
      break;
      
      case 3:     // green
      red = 0;
      green = 255;
      blue = 0;
      if(highByte(index)==0) {    // 0-0xFF
        red = 0xFF-lowByte(index);   // 255 - 0
      }
      else {                    // 0x100-0x1FF
        blue += lowByte(index)*2;         //127 - 154
        if(blue>0xFF) {
          green -= blue - 0xFF;
          blue = 0xFF;
        }
      }
      break;
      
      case 4:     // blue
      red = 0;
      green = 0;
      blue = 255;
      if(highByte(index)==0) {    // 0-0xFF
        green = (0xFF-lowByte(index))*2;   // 255 - 0
        if(green>0xFF) {
          blue -= green - 0xFF;
          green = 0xFF;
        }
      }
      else {                    // 0x100-0x1FF
        red = lowByte(index)>>2;         // red 0-64
      }
      break;
      
      case 5:     // indigo
      red = 40;
      green = 0;
      blue = 255;
      if(highByte(index)==0) {    // 0-0xFF
        red -= (0xFF-lowByte(index))>>2;   // 255 - 0
        if(red<0) {
          green = -red;
          red = 0;
        }
      }
      else {                    // 0x100-0x1FF
        red += lowByte(index)>>1;         // red 40-255
        if(red>0xFF) {
          blue -= red - 0xFF;
          red = 0xFF;
        }
      }        
      break;
      case 6:   // violet
      red = 255;
      green = 0;
      blue = 80;
      if(highByte(index)==0) {    // 0-0xFF
        blue += (0xFF-lowByte(index))>>1;   // 255 - 0
        if(blue>0xFF) {
          red -= blue - 0xFF;
          blue = 0xFF;
        }
      }
      else {                    // 0x100-0x1FF
        blue -= lowByte(index)>>1;         // red 40-255
        if(blue<0) {
          green -= blue - 0xFF;
          blue = 0;
        }
      }     
      break;
      
      default:                  // should not get here
      red = 0;
      green = 0;
      blue = 0;
    }
      
    pixels.setPixelColor(i, pixels.Color(red,green,blue)); // set colour
    
  }  //  for(int i=0;i<NUMPIXELS;i++){

  cnt += 1;
  if(cnt>=NUMPIXELS) cnt=0;

  #ifdef twinkle
      // twinkle routine
  
    tw = cno-0xDF;
  
    if(tw <NUMPIXELS && tw>= 0 && digitalRead(twi)==HIGH) {
      pixels.setPixelColor(tw, white);
    }
  #endif
  
  pixels.show();    // This sends the updated pixel colors to the string.

  delay(delayval);  // Delay (milliseconds).

  // Serial.println(millis());        // use for checking loop speed (delay plus code)
}

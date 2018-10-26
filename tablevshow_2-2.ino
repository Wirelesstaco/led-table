#include "FastLED.h"
#include <math.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
   #warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    165
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120


class Player{
  

   int buttonState;             // the current reading from the input pin
   int lastButtonState = HIGH;   // the previous reading from the input pin
   
   unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
   unsigned long debounceDelay = 0;    // the debounce time; increase if the output flickers
   
   CRGB color;
    bool wasDown = false;
   bool _isAlive;
   bool doubleTime = false;
   public:
   byte pinNum;
   int posOffset;
   int currentPos;
   int clickCount;
   int modPos;
   int gameSpeed;
   

   Player(byte p, int startPosition,CRGB myColor){
      _isAlive =true;
      Serial.println("START");
      pinNum = p;
      posOffset = startPosition;
      color = myColor;
      
      //Button
      pinMode(pinNum, INPUT_PULLUP);
      currentPos =  posOffset;
      clickCount =0;
      
   }

  //Public Return if Alive or not
  bool isAlive(){
    return _isAlive;
  }
  CRGB myColor(){
    return color;
  }

  void setDoubleTime(bool doubleTimeSet){
     doubleTime = doubleTimeSet;
  }
   
   void Update(int theGameSpeed){
    gameSpeed = theGameSpeed;
      if(isAlive()){
         // read the state of the switch into a local variable:
         int reading = digitalRead(pinNum);
         
         // If the switch changed, due to noise or pressing:
         if (reading != lastButtonState) {
            // reset the debouncing timer
            lastDebounceTime = millis();
         }
         
         if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != buttonState) {
               buttonState = reading;
               
               // only toggle the Count if the new button state is LOW
               if (buttonState == LOW) {
                  currentPos = currentPos + gameSpeed;
                  clickCount = clickCount + gameSpeed;
                  
                  // Serial.print(currentPos);
                  wasDown= true;
               }
              if(doubleTime){
               if (buttonState ==HIGH && wasDown){
                  currentPos = currentPos + gameSpeed;
                  clickCount = clickCount + gameSpeed;
                  wasDown =false;
               }
              }
               
            }
         }
         
         // save the reading. Next time through the loop, it'll be the lastButtonState:
         lastButtonState = reading;
         
         //Light LED
         ledPosition(currentPos,color);
         
      }
   }
   
   void ledPosition(int ledPos,CRGB ledColor){
      //Mod sets the lap loop.
      modPos = ledPos % NUM_LEDS;
   
      leds[modPos] = ledColor;

      if(gameSpeed >=2){
          leds[modPos+1] = ledColor;
      }
      if(gameSpeed >=3){
          leds[modPos+2] = ledColor;
      }
      if(gameSpeed >=4){
          leds[modPos+3] = ledColor;
      }
      if(gameSpeed >=5){
          leds[modPos+4] = ledColor;
      }
       if(gameSpeed >=6){
          leds[modPos+5] = ledColor;
      }
      
      // clear this led for the next time around the loop
      if(modPos == 0){
         leds[NUM_LEDS-1] = CRGB::Black;
           if(gameSpeed >=2){
          leds[NUM_LEDS-2] = CRGB::Black;
         }
          if(gameSpeed >=3){
            leds[NUM_LEDS-3] = CRGB::Black;
         }
          if(gameSpeed >=4){
            leds[NUM_LEDS-4] = CRGB::Black;
         }
          if(gameSpeed >=5){
            leds[NUM_LEDS-5] = CRGB::Black;
         }
           if(gameSpeed >=6){
            leds[NUM_LEDS-6] = CRGB::Black;
         }
          
      }else{
         leds[modPos-1] = CRGB::Black;
         if(gameSpeed >=2){
          leds[modPos-2] = CRGB::Black;
         }
          if(gameSpeed >=3){
          leds[modPos-3] = CRGB::Black;
         }
           if(gameSpeed >=4){
          leds[modPos-4] = CRGB::Black;
         }
           if(gameSpeed >=5){
          leds[modPos-5] = CRGB::Black;
         }

         if(gameSpeed >=6){
          leds[modPos-6] = CRGB::Black;
         }

      }
   }
   void Die(){
      leds[modPos] = CRGB::Black;
      _isAlive = false;
      Serial.println("die");
      
   }
   
   
};




int gameSpeed = 3;
String gameState ="idle";
bool jankyIsPAlive[] ={true,true,true,true,true,true};
//int gameSpeed = 1;

//bool p1Alive = true;
int deathCount = 0;

void setup() {
   
   
   
   delay(3000); // 3 second delay for recovery
   
   // tell FastLED about the LED strip configuration
   FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
   //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
   
   // set master brightness control
   FastLED.setBrightness(BRIGHTNESS);
   
   Serial.begin(9600);
   
}


Player p1(2,3,CRGB::Yellow);
Player p2(3,30,CRGB::Red);
Player p3(4,60,CRGB::White);
Player p4(5,90,CRGB::Blue);
Player p5(6,120,CRGB::Purple);
Player p6(7,150,CRGB::Green);



// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow ,confetti,rainbowWithGlitter, juggle,sinelon, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns



//Button state Reset

int buttonState;   
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50; 
const int buttonPin = 2;

void resetCheck(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
          resetGame();
      }
    }
  }
  lastButtonState = reading;
      
}

void resetGame(){

    if(gameState != "playing"){
 
    Serial.println("lights");

      for(int dot = 0; dot < NUM_LEDS; dot++) { 
                 leds[dot] = CRGB::Black;
                  FastLED.show();
            if(dot == NUM_LEDS -1){
              gameState = "playing";     
            }
      }
    
     deathCount =0;
      p1.currentPos =  p1.posOffset;
      p1.clickCount = 0;


      p2.currentPos =  p2.posOffset;
      p2.clickCount = 0;

      p3.currentPos =  p3.posOffset;
      p3.clickCount = 0;

      p4.currentPos =  p4.posOffset;
      p4.clickCount = 0;


      p5.currentPos =  p5.posOffset;
      p5.clickCount = 0;

      p6.currentPos =  p6.posOffset;
      p6.clickCount = 0;
      
      
      jankyIsPAlive[0] =true;
      jankyIsPAlive[1] =true;
      jankyIsPAlive[2] =true;
      jankyIsPAlive[3] =true;
      jankyIsPAlive[4] =true;
      jankyIsPAlive[5] =true;
      gCurrentPatternNumber = 0;
      p1.setDoubleTime(false);
      p2.setDoubleTime(false);
      p3.setDoubleTime(false);
      p4.setDoubleTime(false);
      p5.setDoubleTime(false);
      p6.setDoubleTime(false);
    }
 
}
void loop()
{  
  
  if(gameState != "playing"){
    gPatterns[gCurrentPatternNumber]();
    resetCheck();
  
  }else{
     gameloop();
  }

   /*player(currentPos1,CRGB::Yellow);
   player(currentPos2,CRGB::Blue);
   buttonCode();*/
   
   // send the 'leds' array out to the actual LED strip
   FastLED.show();
   // insert a delay to keep the framerate modest
   FastLED.delay(1000/FRAMES_PER_SECOND);
   if(gameState == "idle"){
   // do some periodic updates
   EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
   EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically

   }
   eliminationCheck();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//Game

void gameloop(){
  CRGB endColor = CRGB::Black;
    //rainbowWithGlitter();
   // juggle();
   if(jankyIsPAlive[0]){
   p1.Update(gameSpeed);
   endColor = p1.myColor();
     }
   if(jankyIsPAlive[1]){
      p2.Update(gameSpeed);
      endColor = p2.myColor();
   }
    if(jankyIsPAlive[2]){
   p3.Update(gameSpeed);
   endColor = p3.myColor();
    }
     if(jankyIsPAlive[3]){
   p4.Update(gameSpeed);
   endColor = p4.myColor();
     }
    if(jankyIsPAlive[4]){
   p5.Update(gameSpeed);
   endColor = p5.myColor();
    }
    if(jankyIsPAlive[5]){
   p6.Update(gameSpeed);
   endColor = p6.myColor();
    }

    if(deathCount >=4){
      p1.setDoubleTime(true);
      p2.setDoubleTime(true);
      p3.setDoubleTime(true);
      p4.setDoubleTime(true);
      p5.setDoubleTime(true);
      p6.setDoubleTime(true);
    }
 
   if(deathCount >=5){
    //gameState = "idle";
    // resetGame();

      //Win Animation -> Reset Game
      for(int dot = 0; dot < NUM_LEDS; dot++) { 
                 leds[dot] = endColor;
                  FastLED.show();
            
                   //leds[dot] = CRGB::Black;
            if(dot == NUM_LEDS -1){
               gameState = "idle";
             //  resetGame();   
            }
      }
   }else {
   
   }
}



//Elimiated check.

void eliminationCheck(){
   
Player playerArray[] = {p1,p2,p3,p4,p5,p6};

   //If Mod > other Mod
   for( int i = 0; i < 6; i++){
      for( int j =0; j <6; j++){
         if(j !=i){
            if(playerArray[i].modPos  == playerArray[j].modPos){
              
              //Serial.println(playerArray[i].modPos);
             
               if(playerArray[i].clickCount < playerArray[j].clickCount){
                   if(jankyIsPAlive[i]){
                    deathCount++;
                   //gameSpeed++;
                   }
               //  p1Alive =false;
                playerArray[i].Die();
               jankyIsPAlive[i] =false;
                 
                 
               }
            }

             /*  else if(gameSpeed >=2 && playerArray[i].modPos  == playerArray[j].modPos -1){
              
              //Serial.println(playerArray[i].modPos);
             
               if(playerArray[i].clickCount < playerArray[j].clickCount){
                   if(jankyIsPAlive[i]){
                   gameSpeed++;
                   }
               //  p1Alive =false;
                playerArray[i].Die();
               jankyIsPAlive[i] =false;
                 
                 
               }
            }


             else if(gameSpeed >=3 && playerArray[i].modPos  == playerArray[j].modPos -2){
              
              //Serial.println(playerArray[i].modPos);
             
               if(playerArray[i].clickCount < playerArray[j].clickCount){
                   if(jankyIsPAlive[i]){
                   gameSpeed++;
                   }
               //  p1Alive =false;
                playerArray[i].Die();
               jankyIsPAlive[i] =false;
                 
                 
               }
            }

               else if(gameSpeed >=4 && playerArray[i].modPos  == playerArray[j].modPos -3){
              
              //Serial.println(playerArray[i].modPos);
             
               if(playerArray[i].clickCount < playerArray[j].clickCount){
                   if(jankyIsPAlive[i]){
                   gameSpeed++;
                   }
               //  p1Alive =false;
                playerArray[i].Die();
               jankyIsPAlive[i] =false;
                 
                 
               }
            }

                else if(gameSpeed >=5 && playerArray[i].modPos  == playerArray[j].modPos -4){
              
              //Serial.println(playerArray[i].modPos);
             
               if(playerArray[i].clickCount < playerArray[j].clickCount){
                   if(jankyIsPAlive[i]){
                   gameSpeed++;
                   }
               //  p1Alive =false;
                playerArray[i].Die();
               jankyIsPAlive[i] =false;
                 
                 
               }
            }*/

            
            

         }
      }
   }
}



///// If Mod Postion is higer than yours and I start behind you

//If lap is higher and Mod is higher





void nextPattern()
{
   // add one to the current pattern number, and wrap around at the end
   gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
   // FastLED's built-in rainbow generator
   fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
   // built-in FastLED rainbow, plus some random sparkly glitter
   rainbow();
   addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
   if( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
   }
}

void confetti()
{
   // random colored speckles that blink in and fade smoothly
   fadeToBlackBy( leds, NUM_LEDS, 10);
   int pos = random16(NUM_LEDS);
   leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
   // a colored dot sweeping back and forth, with fading trails
   fadeToBlackBy( leds, NUM_LEDS, 20);
   int pos = beatsin16( 13, 0, NUM_LEDS-1 );
   leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
   // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
   uint8_t BeatsPerMinute = 62;
   CRGBPalette16 palette = PartyColors_p;
   uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
   for( int i = 0; i < NUM_LEDS; i++) { //9948
      leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
   }
}

void juggle() {
   // eight colored dots, weaving in and out of sync with each other
   fadeToBlackBy( leds, NUM_LEDS, 20);
   byte dothue = 0;
   for( int i = 0; i < 8; i++) {
      leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
      dothue += 32;
   }
}


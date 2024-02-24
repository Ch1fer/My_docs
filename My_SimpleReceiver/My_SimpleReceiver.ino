#include <Arduino.h>
#include "Adafruit_NeoPixel.h"


// This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

#define DECODE_NEC // Includes Apple and Onkyo


#define LED_COUNT 150 - 12
#define LED_PIN A5



int mode = 0;
int new_mode = 0;
bool power = true;
int brightness = 255;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 200;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  
  strip.begin();

  Serial.begin(115200);

  startMillis = millis();

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}



void loop() {
  currentMillis = millis();
  strip.setBrightness(brightness);
  // Serial.println(brightness);
  /*
    * Check if received data is available and if yes, try to decode it.
    * Decoded result is in the IrReceiver.decodedIRData structure.
    *
    * E.g. command is in IrReceiver.decodedIRData.command
    * address is in command is in IrReceiver.decodedIRData.address
    * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
    */
  if (IrReceiver.decode()) {

    /*
      * Print a short summary of received data
      */
    // IrReceiver.printIRResultShort(&Serial);
    // IrReceiver.printIRSendUsage(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
        // We have an unknown protocol here, print more info
        IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();

    /*
      * !!!Important!!! Enable receiving of the next value,
      * since receiving has stopped after the end of the current received data packet.
      */
    IrReceiver.resume(); // Enable receiving of the next value

    /*
      * Finally, check the received data and perform actions according to the received command
      */
    
    if (IrReceiver.decodedIRData.command == 0x47 && (currentMillis - startMillis >= period)){
      power = !power;
      if(!power){
        Serial.println("OFF");
      }
      else{
        Serial.print("Mode_");
        Serial.println(mode);
      }
      startMillis = millis();
    }
    else if(power){

      switch (IrReceiver.decodedIRData.command){
        
        case 0x7:

          if(brightness >= 95 && (currentMillis - startMillis >= period)){
            brightness = brightness - 32;
            startMillis = millis();
          }
          break;

        case 0x15:

          if(brightness <= 223 && (currentMillis - startMillis >= period)){
            brightness = brightness + 32;
            startMillis = millis();
          }
          break;

        case 0x16:
          new_mode = 0;
          break;

        case 0xC:
          new_mode = 1;
          break;

        case 0x18:
          new_mode = 2;
          break;

        case 0x5E:
          new_mode = 3;
          break;

        case 0x8:
          new_mode = 4;
          break;

        case 0x1C:
          new_mode = 5;
          break;

        case 0x5A:
          new_mode = 6;
          break;

        case 0x42:
          new_mode = 7;
          break;

        case 0x52:
          new_mode = 8;
          break;

        case 0x4A:
          new_mode = 9;
          break;
      }
    }
    if(power && new_mode != mode){
      mode = new_mode;
      switch (mode){

        case 0:
          Serial.println("Mode_0");
          setColour(255, 0, 0); //red
          break;

        case 1:
          Serial.println("Mode_1");
          setColour(0, 255, 0); //green
          break;

        case 2:
          Serial.println("Mode_2");
          setColour(0, 0, 255); //blue
          break;

        case 3:
          Serial.println("Mode_3");
          setColour(255, 255, 0); //yellow
          break;
        
        case 4:
          Serial.println("Mode_4");
          setColour(255, 120, 0); //orange
          break;

        case 5:
          Serial.println("Mode_5");
          setColour(200, 0, 255); //purple
          break;

        case 6:
          Serial.println("Mode_6");
          break;

        case 7:
          Serial.println("Mode_7");
          break;

        case 8:
          Serial.println("Mode_8");
          break;

        case 9:
          Serial.println("Mode_9");
          break;
      }
    }

    // Serial.print(currentMillis);
    // Serial.print(" - ");
    // Serial.print(startMillis);
    // Serial.print(" = ");
    // Serial.println(currentMillis - startMillis);
  }
}

void clearStrip(){

  for(int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }

  strip.show();

  return;
}

void setColour(int r, int g, int b){

  clearStrip();

  for(int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  strip.show();
  
  return;
}





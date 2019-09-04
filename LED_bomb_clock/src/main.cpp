#include <Arduino.h>
#include <IRremote.h>

#define RIGHT_LEDS    7
#define LEFT_LEDS     8
#define IR_RECIVE     5
#define PIN_CLOCK     2//2 3
#define PIN_LATCH     3//3 2
#define PIN_DATA      4//4 4
#define PIN_LED_1     9
#define PIN_LED_2     10

const byte ledCharSet[10] =
{
  B01111110, //0
  B00011000, //1
  B11010110, //2
  B11011010, //3
  B10111000, //4
  B11101010, //5
  B11101110, //6
  B01011000, //7
  B11111110, //8
  B11111010  //9
};

IRrecv irrecv(IR_RECIVE);

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(RIGHT_LEDS, OUTPUT);
  pinMode(LEFT_LEDS, OUTPUT);
  digitalWrite(LEFT_LEDS, 0);
  digitalWrite(RIGHT_LEDS, 1);
  digitalWrite(PIN_LED_1, 0);
  digitalWrite(PIN_LED_2, 0);
  
}

void  ircode (decode_results *results)
{

  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }
  Serial.print(results->value, HEX);
}



void  dumpInfo (decode_results *results)
{
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
    return;
  }

  Serial.print("Code      : ");
  ircode(results);

  Serial.println("");
}


void DisplayNumber(int num) {
  int tens;
  int ones;
  if (num > 100 || num < 0) {
    num = 0;
  }

  ones = num % 10;
  if (num < 10) {
    tens = 0;
  }
  else {
    tens = num -(num/10)*10;
  }

  digitalWrite(PIN_LATCH, 0);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[ones]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[tens]);
  digitalWrite(PIN_LATCH, 1);
  
  Serial.print(ones);
  Serial.print("  ");
  Serial.print(tens);
  Serial.print("\n"); 
}

void  loop ( )
{
  decode_results  results;        
  // if (irrecv.decode(&results)) {  
  //   dumpInfo(&results);           
  //   Serial.println("");           
  //   irrecv.resume();              
  // }

  for (int i = 0; i < 10 ; i++) {
    // DisplayNumber(i);
    
    digitalWrite(RIGHT_LEDS,!digitalRead(RIGHT_LEDS));
    digitalWrite(LEFT_LEDS,!digitalRead(LEFT_LEDS));
    analogWrite(PIN_LED_1,30);
    analogWrite(PIN_LED_2,30);
    digitalWrite(PIN_LATCH, 0);
    shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[i]);
    shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[i]);
    shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[i]);
    digitalWrite(PIN_LATCH, 1);
    delay(600);
  }
  delay(600);
}
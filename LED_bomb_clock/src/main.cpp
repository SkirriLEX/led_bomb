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


const byte ledCharSet[11] ={
  B01111110, //0
  B00011000, //1
  B11010110, //2
  B11011010, //3
  B10111000, //4
  B11101010, //5
  B11101110, //6
  B01011000, //7
  B11111110, //8
  B11111010, //9
  B00000000  //F
};

int led_num1=72, led_num2=0, led_num3=0;
bool flag_clock=0, flag_play =1;
long tick_last_time =0;
int flag_general_state = 1;
int time_tick = 1000;
int time_half_tick = 500;
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

void  dumpInfo (decode_results *results){
  long code = results->value;
  Serial.println(code, HEX);
  switch (code) {
    case 0xFF38C7: //OK
      Serial.println("OK");
      if  (flag_general_state !=1){
        flag_general_state =1;
      } else {
        flag_general_state =2;
      }
      break;
    case 0xFF18E7: //UP
      if (led_num1<99){
        led_num1++;
      }
      break;
    case 0xFF4AB5: //DOWN
      if (led_num1>0){
        led_num1--;
      }
      break;
    case 0xFF5AA5: //RIGHT
      if (led_num2<59){
        led_num2++;
      }
      break;
    case 0xFF10EF: //LEFT
      if (led_num2>0){
        led_num2--;
      }
      break;
    case 0xFFA25D: //1
      flag_general_state =1;
      break;
    case 0xFF629D: //2
      flag_general_state =4;
      break;
    case 0xFFE21D: //3
    case 0xFF22DD: //4
    case 0xFF02FD: //5
      led_num1=72, led_num2=0, led_num3=0;
      break;
    case 0xFFC23D: //6
      break;
    case 0xFFE01F: //7
      time_tick = 1000;
      time_half_tick = 500;
      break;
    case 0xFFA857: //8      
      time_tick = 500;
      time_half_tick = 250;
      break;
    case 0xFF906F: //9
      time_tick = 250;
      time_half_tick = 125;
      break;
    case 0xFF9867: //0
      flag_general_state =3;
      break;
    case 0xFF6897: //*
      time_tick = 1000;
      time_half_tick = 500;
      break;
    case 0xFFB04F: //#
      time_tick = 10;
      time_half_tick = 500;
      break;
    case 0xFFFFFF:
      break;
    default:
      break;
  }
}

int getNums(int num){
  if  (num>99){
    return 10;
  }
  if (flag_clock){
    return num/10;
  } else {
    return num%10;
  } 
}

void drawDisplayNumbers() {
  if (flag_clock){ 
    digitalWrite(RIGHT_LEDS,!flag_clock);
  } else {
    digitalWrite(LEFT_LEDS,flag_clock);
  }
  digitalWrite(PIN_LATCH, 0);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[getNums(led_num3)]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[getNums(led_num2)]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, ledCharSet[getNums(led_num1)]);
  digitalWrite(PIN_LATCH, 1);    
  if (!flag_clock){ 
    digitalWrite(RIGHT_LEDS,!flag_clock);
  } else {
    digitalWrite(LEFT_LEDS,flag_clock);
  }
  delayMicroseconds(100);
  flag_clock = !flag_clock;
}

void  loop ( )
{
  decode_results  results;        
  if (irrecv.decode(&results)) {  
    dumpInfo(&results);           
    irrecv.resume();              
  }

  switch (flag_general_state){
    case 1://time go
      if (millis()-tick_last_time>time_half_tick){
        analogWrite(PIN_LED_1,20);
        analogWrite(PIN_LED_2,20);
      }else{
        digitalWrite(PIN_LED_1,0);
        digitalWrite(PIN_LED_2,0);
      }
      if (millis()-tick_last_time>time_tick){
        tick_last_time = millis();
        led_num3--;
        if (led_num3<0){
          led_num2--;
          if (led_num2<0){
            led_num1--;
            if (led_num1<0)
            {
              led_num1 = 99;
              flag_general_state =3;
            }
            led_num2=59;
          }
          led_num3=59;
        }
      }
      break;
    case 2://time stop
        analogWrite(PIN_LED_1,20);
        analogWrite(PIN_LED_2,20);
      break;
    case 3://time out
        led_num1=0, led_num2=0, led_num3=0;
        flag_general_state =2;
      break;
    case 4:
      if (millis()-tick_last_time>time_half_tick){
        analogWrite(PIN_LED_1,20);
        analogWrite(PIN_LED_2,20);
      }else{
        digitalWrite(PIN_LED_1,0);
        digitalWrite(PIN_LED_2,0);
      }
      if (millis()-tick_last_time>time_tick){
        tick_last_time = millis();
        led_num3++;
        if (led_num3>59){
          led_num2++;
          if (led_num2>59){
            led_num1++;
            if (led_num1>99)
            {
              led_num1 = 00;
              flag_general_state =3;
            }
            led_num2=00;
          }
          led_num3=00;
        }
      }
      break;
    default:

      break;
    
  }
  drawDisplayNumbers();
}
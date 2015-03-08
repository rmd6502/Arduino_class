#include <Servo.h> 
#include "flappy.h"
#include "pitches.h"

#define BOX_OPEN 90
#define BOX_CLOSE 0

// Constant multiplier
#define DEG_PER_MM (180.0/(21.0*3.14159))

// Now in mm
#define CHARACTER_BOTTOM 0
#define CHARACTER_TOP (60.0*DEG_PER_MM)
#define JUMP_DISTANCE (10.0*DEG_PER_MM)

// 
static const uint32_t gracePeriod = 1000;

const uint8_t magnetPin = hall_input;     // the number of the reed switch
const uint8_t buttonPin = start_button;  // the number of the pushbutton pin for bird
const uint8_t speakerPin = speaker;
const uint8_t rollServoPin = background_servo;
const uint8_t characterServoPin = character_servo;
const uint8_t lidServoPin = box_servo;
const uint8_t ledPin =  led;      // the number of the LED pin

uint16_t magnetState = 0;         // variable
uint8_t buttonState = 0;         // variable 
uint8_t birdup = CHARACTER_BOTTOM; //start 
boolean released = true;  
boolean in_game =false;
Servo myservoRoll;
Servo myservoBird;
Servo myservoGame;

int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};// notes in the melody:
uint8_t noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 }; // note durations: 4 = quarter note, 8 = eighth note, etc.:

int jumpmusic[] = {
  NOTE_A3};// notes in the melody:
uint8_t jumpDurations[] = {
  4}; // note durations: 4 = quarter note, 8 = eighth note, etc.:

void setup() 
{ 
  myservoGame.attach(lidServoPin);
  myservoGame.write(BOX_CLOSE); 
  delay(100);
  myservoGame.detach();
  
  pinMode(magnetPin, INPUT_PULLUP);  //reed
  pinMode(buttonPin, INPUT_PULLUP); //button 
  // Enable the pullup resistor for Port A0
//  bitSet(PUEA, _BV(magnetPin) | _BV(buttonPin));
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  noTone(speakerPin);
  pinMode(speakerPin, OUTPUT);
  digitalWrite(speakerPin, LOW);
  
  Serial.begin(115200);
  Serial.println("Starting");
} 

void loop() {
  static uint32_t gameStartTime = 0;
  magnetState = digitalRead(magnetPin);
  buttonState = digitalRead(buttonPin);

  //start game 
  if (buttonState == LOW && in_game==false){
    Serial.println("Starting game");
    myservoGame.write(BOX_OPEN); //open box
    birdup=(CHARACTER_BOTTOM+CHARACTER_TOP)/2; //bird position
    delay (700);
    myservoRoll.attach(rollServoPin);
    myservoRoll.write(10); //roll background
    myservoBird.attach(characterServoPin);
    in_game=true;
    released = false;
    digitalWrite(ledPin, HIGH);
    gameStartTime = millis();
  }
  if(in_game==true)
  {
    if ((buttonState == LOW) && released ==true)
    {
      Serial.println("up");
      released = false;    
      if(birdup > (CHARACTER_BOTTOM)){
        birdup-=(JUMP_DISTANCE); //going up
      } else {
        birdup = CHARACTER_BOTTOM;
      }
      for (int thisNote = 0; thisNote < (sizeof(jumpmusic)/sizeof(jumpmusic[0])); thisNote++) {
        int jumpDuration = 1000/jumpDurations[thisNote];
        tone(speakerPin, jumpmusic[thisNote],jumpDuration);
        int pauseBetweenNotes = jumpDuration * 13 / 10;
        delay(pauseBetweenNotes);
      }
    }
    else
    {
      if(birdup < CHARACTER_TOP){
        Serial.println("down");
        birdup+=1; //going down
        delay(10);
      }
    }
    if(buttonState == HIGH)
    {
      released = true;
    }

    myservoBird.write(birdup);
    delay (50);

    //game over; when bird hits ground
    if (birdup >= CHARACTER_TOP)
    {
          Serial.println("Game over 1");
      game_over();
    }
    //game over: when bird hit pipes
    if (magnetState == LOW && millis() - gameStartTime >= gracePeriod) 
    {   
         Serial.println("Game over 2"); 
      game_over();
    }
  }

}

void game_over(){
  //reset all the variables
  in_game =false;
  birdup = (CHARACTER_BOTTOM + CHARACTER_TOP)/2;
  released = true;
  myservoRoll.write(90); //stop roll background
  myservoRoll.detach();
  myservoGame.write(BOX_CLOSE); //close game box
  myservoGame.detach();
  myservoBird.write(birdup);//bird go back to position 40
  myservoBird.detach();
  digitalWrite(ledPin, LOW);
//  digitalWrite(ledPin, HIGH);
  for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(melody[0])); thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 13 / 10;
    delay(pauseBetweenNotes);
  }
}







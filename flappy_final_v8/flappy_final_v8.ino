#include <Servo.h> 
#include "flappy.h"
#include "pitches.h"

#define BOX_OPEN 50
#define BOX_CLOSE 150

#define CHARACTER_BOTTOM 0
#define CHARACTER_TOP 160
#define JUMP_DISTANCE 25

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
  myservoBird.attach(characterServoPin);
  myservoBird.write(CHARACTER_BOTTOM);
  myservoGame.attach(lidServoPin);
  myservoGame.write(BOX_CLOSE); 
  delay(700);
  myservoGame.detach();
  myservoBird.detach();
  
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
    myservoBird.attach(characterServoPin);
    myservoGame.attach(lidServoPin);
    myservoGame.write(BOX_OPEN); //open box
    birdup=(CHARACTER_BOTTOM+CHARACTER_TOP)/2; //bird position
    delay (700);
    myservoRoll.attach(rollServoPin);
    myservoRoll.write(10); //roll background
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

    Serial.println(birdup);
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
  myservoRoll.write(90); //stop roll background
  myservoRoll.detach();
  myservoGame.attach(lidServoPin);
  myservoGame.write(BOX_CLOSE); //close game box
  delay(100);
  birdup = CHARACTER_BOTTOM;
  released = true;
  delay(100);
  myservoBird.write(birdup);//bird go back to position 40
  digitalWrite(ledPin, LOW);
//  digitalWrite(ledPin, HIGH);
  for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(melody[0])); thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 13 / 10;
    delay(pauseBetweenNotes);
  }
  delay(550);
  myservoGame.detach();
  myservoBird.detach();
}







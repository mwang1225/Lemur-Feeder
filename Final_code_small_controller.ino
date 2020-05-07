#include <LiquidCrystal_I2C.h> //LCD display
#include <LowPower.h> //Low Power mode
#include <Wire.h>
#define led_pin 12

const int upButton = 7; //pin for button that increases time
const int downButton = 8; //pin for button that decreases time
const int confirmButton = 9; //pin for button that confirms feeding time
const int enable = 3; //pin for output to motor drive
const int logic_pow = 2; //pin for output voltage for logic power
int minutes = 0; //minutes until next feeding
int hr = 0; //hours until next feeding
int upButtonState = 0;
int downButtonState = 0;
int confirmState = 0;
int lastUpButtonState = 0;
int lastDownButtonState = 0;
int lastConfirmState = 0;
int outputSec; //total seconds to next feeding
int timeOne; //time until first feeding
int timeTwo; //time between first and second feeding
int timeThree; //time between second and third feeding
int randInt1; //random time for first feeding if needed
int randInt2;
int randInt3;
int voltage;
int analogPin = A0;

LiquidCrystal_I2C lcd (0x27, 16, 2); //defines 16x2 LCD screen at location 0x27)

void setup() {
  // put your setup code here, to run once:
  randomSeed(1); //seed to synchronize feeders in same enclosure
  pinMode(logic_pow, OUTPUT); //output voltages for motor controller logic
  pinMode(enable, OUTPUT); //output voltages for motor controller
  lcd.begin(); //initialize LCD
  lcd.backlight();
  pinMode(upButton, INPUT); //set button pins to digital inputs
  pinMode(downButton, INPUT);
  pinMode(confirmButton, INPUT);
  randInt1 = random(15000, 45000); //set random time intervals if needed
  randInt2 = random(20000, 60000);
  randInt3 = random(20000, 60000);
}

int Schedule(int dispensal) {
  upButtonState = 0; //set all initial button states to 0 (off)
  downButtonState = 0;
  lastUpButtonState = 0;
  lastDownButtonState = 0;
  confirmState = 0;
  lastConfirmState = 0;
  upButtonState = digitalRead(upButton); //read all button states to detect changes
  downButtonState = digitalRead(downButton);
  confirmState = digitalRead(confirmButton);
  outputSec = 0;
  hr = 2; //default time
  minutes = 0;
  String output = "";
  if (dispensal == 1) {
    output = "1st feeding in: ";
  }
  if (dispensal == 2) {
    output = "2nd feeding in: ";
  }
  if (dispensal == 3) {
    output = "3rd feeding in: ";
  }
  lcd.setCursor(0, 0);
  lcd.print(output);
  confirmState = 0;
  //if confirm state button is untouched, keep allowing time to be changed
  while (confirmState == 0) {
    upButtonState = 0;
    downButtonState = 0;
    lastUpButtonState = 0;
    lastDownButtonState = 0;
    upButtonState = digitalRead(upButton);
    downButtonState = digitalRead(downButton);
    lcd.setCursor(0, 1);
    lcd.print(String(hr) + " hr " + String(minutes) + " min ");
    //if change in down button, decrease time
    if (downButtonState != lastDownButtonState) {
      minutes -= 15;
      lastDownButtonState = downButtonState;
      if (minutes == -15) {
        minutes = 45;
        hr--;
     }
    }
    //if change in up button, increase time
    if (upButtonState != lastUpButtonState) {
      minutes += 15;
      lastUpButtonState = upButtonState;
      if (minutes == 60) {
        minutes = 0;
        hr++;
     }
    }
    lcd.setCursor(0, 1);
    if (hr < 0 || minutes < 0) {
      lcd.print("Random          ");
    }
    else {
      lcd.print(String(hr) + " hr " + String(minutes) + " min  ");
    }
    delay(200);
    confirmState = digitalRead(confirmButton);
  }
  if (hr >= 0 && minutes >= 0) {
    outputSec = hr * 3600 + minutes * 60;
  }
  return outputSec;
}

void TurnMotor(int delayTime, int numTurn){
//while motor is not running, Arduino is Low Power to decrease energy consumption
  for(int x; x < int(delayTime/1000); x++){
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  delay(delayTime);
  digitalWrite(logic_pow, HIGH); //turn motor controller on
  digitalWrite(enable, HIGH); //turn motor on
  delay(484); //time needed to turn 90 degrees
  digitalWrite(enable, LOW); //turn motor off
  if(numTurn == 3) {
    digitalWrite(enable, HIGH);
    delay(1451.6);
    digitalWrite(enable, LOW);
  }
  digitalWrite(logic_pow, LOW);
}
void loop() {
  int sensorValue = analogRead(analogPin);
  float voltage = sensorValue * (5.00 / 1023.00);
  if (voltage < 4.5) {
    digitalWrite(led_pin, HIGH);//if the voltage is below 4.5 V, light turns on
  }
  timeOne = Schedule(1); //convert seconds to milliseconds
  timeTwo = Schedule(2);
  timeThree = Schedule(3);
  lcd.noBacklight(); //turn LCD off after times are confirmed
  lcd.noDisplay();
  if (timeOne < 0) {
    timeOne = randInt1;
  }
  if (timeTwo < 0) {
    timeTwo = randInt2;
  }
  if (timeThree < 0) {
    timeThree = randInt3;
  }
  TurnMotor(timeOne, 1);
  TurnMotor(timeTwo, 2);
  TurnMotor(timeThree, 3);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); //Arduino is in Low Power until turned off
}

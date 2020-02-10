#include <LiquidCrystal_I2C.h>
#include <LowPower.h>
#include <Wire.h>

const int upButton = 7;
const int downButton = 8;
const int confirmButton = 9;
const int in1 = 3;
const int in2 = 2;
int minutes = 0;
int hr = 0;
int upButtonState = 0;
int downButtonState = 0;
int confirmState = 0;
int lastUpButtonState = 0;
int lastDownButtonState = 0;
int lastConfirmState = 0;
int outputSec;
int randInt1;
int randInt2;
int randInt3;

LiquidCrystal_I2C lcd (0x27, 16, 2); //defines 16x2 LCD screen at location 0x27)

void setup() {
  // put your setup code here, to run once:
  randomSeed(1);
  pinMode(in1, OUTPUT); //output voltages for motor
  pinMode(in2, OUTPUT);
  lcd.begin(); //initialize LCD
  lcd.backlight();
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(confirmButton, INPUT);
  randInt1 = random(15000, 45000); //set random time intervals if needed
  randInt2 = random(20000, 60000);
  randInt3 = random(20000, 60000);
}

int schedule(int dispensal) {
  upButtonState = 0;
  downButtonState = 0;
  lastUpButtonState = 0;
  lastDownButtonState = 0;
  confirmState = 0;
  lastConfirmState = 0;
  upButtonState = digitalRead(upButton);
  downButtonState = digitalRead(downButton);
  confirmState = digitalRead(confirmButton);
  outputSec = 0;
  hr = 2;
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
  while (confirmState == 0) {
    upButtonState = 0;
    downButtonState = 0;
    lastUpButtonState = 0;
    lastDownButtonState = 0;
    upButtonState = digitalRead(upButton);
    downButtonState = digitalRead(downButton);
    lcd.setCursor(0, 1);
    if (hr >= 0 && minutes >=0){
      lcd.print(String(hr) + " hr " + String(minutes) + " min ");
    }
    else {
      lcd.print("Random");
    }
    if (downButtonState != lastDownButtonState) {
      minutes -= 15;
      lastDownButtonState = downButtonState;
      if (minutes == -15) {
        minutes = 45;
        hr--;
     }
    }
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
  return outputSec; // output in seconds
}

void TurnMotor(int delayTime, int numTurn){
  for(int x; x < int(delayTime/1000); x++){
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  delay(delayTime);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(14584);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  if(numTurn == 3) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(58336);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}
void loop() {
  int timeOne = schedule(1);
  int timeTwo = schedule(2);
  int timeThree = schedule(3);
  lcd.noBacklight();
  lcd.noDisplay();
  if (timeOne == 0) {
    timeOne = randInt1;
  }
  if (timeTwo == 0) {
    timeTwo = randInt2;
  }
  if (timeThree == 0) {
    timeThree = randInt3;
  }
  TurnMotor(timeOne, 1);
  TurnMotor(timeTwo, 2);
  TurnMotor(timeThree, 3);
  //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

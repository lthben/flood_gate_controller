/*
   Detect water, servo will open for 5s, then close
   Buzzer and light will on when servo is open
   After closed, stop buzzer and light, send SMS "gate closed"
   Manual two buttons for open/close servo
*/

#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

//user settings
String phoneNum = "+6597974063"; //need to include country code
String myMessage = "gate closed";

#define servoPin 6 //pwm pin
#define switchOnPin 4
#define switchOffPin 5
#define ledPin 7
#define buzzerPin 8
#define rainSensorPin 9

Servo servo;
boolean isServoOn, isRaining;

const long TIMEOPEN = 5000;
long servoOnTime;

void setup() {

  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(switchOnPin, INPUT_PULLUP);
  pinMode(switchOffPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(rainSensorPin, INPUT);

  servo.attach(servoPin);
  servo.write(0); //servo angle val: 0-180 degrees
}

void loop() {

  if (digitalRead(switchOnPin) == LOW && isServoOn == false) { //manual servo on
    isServoOn = true;
    servo.write(90);
    //    Serial.println("switched on");
  }

  if (digitalRead(switchOffPin) == LOW && isServoOn == true) { //manual servo off
    isServoOn = false;
    servo.write(0);
    //    Serial.println("switched off");
  }

  if (digitalRead(rainSensorPin) == LOW) {

    //rain detected, servo on
    isServoOn = true;
    isRaining = true;
    servo.write(90);
    servoOnTime = millis();
  }

  if (millis() - servoOnTime > TIMEOPEN && isServoOn && isRaining) {

    //turn servo off after a few sec and send sms message
    isServoOn = false;
    isRaining = false;

    servo.write(0);

    update_indicators(); //added here for immediate update cos send_message will take a few sec

    send_message(); //this will cause the servo to jitter three times. I don't know why. 
  }

  if (mySerial.available() > 0) {
    Serial.write(mySerial.read());
  }

  update_indicators(); //the led and buzzer
}

void update_indicators() {

  //the buzzer and led indicators
  if (isServoOn) { //indicate servo status via led and buzzer
        digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }
}

void send_message() {

  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  mySerial.print("AT+CMGS=\""); mySerial.print(phoneNum); mySerial.println("\"\r");
  delay(1000);
  mySerial.println(myMessage);
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


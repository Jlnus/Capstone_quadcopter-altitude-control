/*
  throttle 구간별 모터 속도 조정
  - 같은 신호를 부여해도 ESC 또는 모터 차이로 다른 추력을 내기 때문에 
    동일한 추력을 내도록 throttle 구간별로 속도를 조정
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);  // RX, TX

const int motorA_pin = 3;
const int motorB_pin = 9;
const int motorC_pin = 10;
const int motorD_pin = 11;

int motorA_speed = 100;
int motorB_speed = 100;
int motorC_speed = 100;
int motorD_speed = 100;

String input = "";
int offset = 0;
int speed = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);  // bluetooth
  mySerial.setTimeout(20);
  analogWrite(motorA_pin, motorA_speed);
  analogWrite(motorB_pin, motorB_speed);
  analogWrite(motorC_pin, motorC_speed);
  analogWrite(motorD_pin, motorD_speed);
}

void loop() {
  if (mySerial.available()) {
    input = mySerial.readString();
    speed = input.toInt();
    motorA_speed = 100 + speed;
    motorB_speed = 100 + speed;
    motorC_speed = 100 + speed;
    motorD_speed = 100 + speed;

    if (motorA_speed < 105) {
      motorA_speed = 100;
    }
    if (motorA_speed > 255) {
      motorA_speed = 255;
    }
    ////////////////////////////////
    if (motorB_speed < 105) {
      motorB_speed = 100;
    }
    if (motorB_speed > 255) {
      motorB_speed = 255;
    }
    ////////////////////////////////////
    if (motorC_speed < 105) {
      motorC_speed = 100;
    }
    if (motorC_speed >= 105 && motorC_speed < 175) {
      motorC_speed -= 2;
    }
    if (motorC_speed >= 175 && motorC_speed < 185) {
      motorC_speed -= 1;
    }
    if (motorC_speed >= 185 && motorC_speed < 195) {
      motorC_speed -= 1;
    }
    if (motorC_speed >= 195 && motorC_speed < 205) {
      motorC_speed -= 1;
    }
    if (motorC_speed >= 205 && motorC_speed < 215) {
      motorC_speed -= 1;
    }
    if (motorC_speed >= 215 && motorC_speed < 255) {
      motorC_speed -= 1;
    }
    if (motorC_speed >= 255) {
      motorC_speed = 255;
    }
    ////////////////////////////////////
    if (motorD_speed < 105) {
      motorD_speed = 100;
    }
    if (motorD_speed >= 105 && motorD_speed < 155) {
      motorD_speed -= 4;
    }
    if (motorD_speed >= 155 && motorD_speed < 165) {
      motorD_speed -= 3;
    }
    if (motorD_speed >= 165 && motorD_speed < 175) {
      motorD_speed -= 3;
    }
    if (motorD_speed >= 175 && motorD_speed < 185) {
      motorD_speed -= 3;
    }
    if (motorD_speed >= 185 && motorD_speed < 195) {
      motorD_speed -= 2;
    }
    if (motorD_speed >= 195 && motorD_speed < 205) {
      motorD_speed -= 2;
    }
    if (motorD_speed >= 205 && motorD_speed < 215) {
      motorD_speed -= 1;
    }
    if (motorD_speed >= 215 && motorD_speed < 255) {
      motorD_speed -= 1;
    }
    if (motorD_speed >= 255) {
      motorD_speed = 255;
    }

    analogWrite(motorA_pin, motorA_speed);
    analogWrite(motorB_pin, motorB_speed);
    analogWrite(motorC_pin, motorC_speed);
    analogWrite(motorD_pin, motorD_speed);

    mySerial.println("motor speed");
    mySerial.print("A : ");
    mySerial.println(motorA_speed);
    mySerial.print("B : ");
    mySerial.println(motorB_speed);
    mySerial.print("C : ");
    mySerial.println(motorC_speed);
    mySerial.print("D : ");
    mySerial.println(motorD_speed);
  }
}

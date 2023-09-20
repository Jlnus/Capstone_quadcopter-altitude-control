/* 
  모터 균형을 맞추기 위한 두 개의 모터 속도 비교
  드론 암 축 고정 후 한쪽 모터에 offset 부여
*/
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);  // RX, TX

const int motorA_pin = 3;
const int motorB_pin = 9;
const int motorC_pin = 10;
const int motorD_pin = 11;

int motorA_speed = 100; // ESC arming signal
int motorB_speed = 100;
int motorC_speed = 100;
int motorD_speed = 100;

String input = "";
int offset = 0;
int speed = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);  // bluetooth
  mySerial.setTimeout(20); // readString() 딜레이 시간 설정
  analogWrite(motorA_pin, motorA_speed);
  analogWrite(motorB_pin, motorB_speed);
  analogWrite(motorC_pin, motorC_speed);
  analogWrite(motorD_pin, motorD_speed);
}

void loop() {
  // a 입력시 offset +1, b 입력시 offset-1, r 입력시 offset 초기화, 나머지 입력 throttle
  if (mySerial.available()) {
    input = mySerial.readString();
    if (input == "a") {
      offset += 1;
    } else if (input == "b") {
      offset -= 1;
    } else if (input == "r") {
      offset = 0;
    } else {
      speed = input.toInt();
    }

    if (speed >= 0 && speed < 140) { // 한 쌍의 모터 비교를 위해 나머지 주석처리
      motorA_speed = 100 + speed;
      // motorB_speed = 100 + speed;
      motorC_speed = 100 + speed + offset;
      // motorD_speed = 100 + speed + offset;

      analogWrite(motorA_pin, motorA_speed);
      analogWrite(motorB_pin, motorB_speed);
      analogWrite(motorC_pin, motorC_speed);
      analogWrite(motorD_pin, motorD_speed);
    }


    mySerial.println("motor speed");
    mySerial.print("A : ");
    mySerial.println(motorA_speed);
    mySerial.print("B : ");
    mySerial.println(motorB_speed);
    mySerial.print("C : ");
    mySerial.println(motorC_speed);
    mySerial.print("D : ");
    mySerial.println(motorD_speed);
    mySerial.print("offset : ");
    mySerial.println(offset);
  }
}

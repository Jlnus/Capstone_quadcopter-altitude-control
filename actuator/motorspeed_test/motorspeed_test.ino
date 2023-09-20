// 모터 속도 테스트(블루투스)
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5);  // RX, TX

const int motorA_pin = 3;
const int motorB_pin = 9;
const int motorC_pin = 10;
const int motorD_pin = 11;

int MIN_motor_speed = 100; // ESC가 준비(arm)되기 위한 신호
int motor_speed = 0;
int input = 0;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(20);  // parseInt의 딜레이 변경
  mySerial.begin(115200); // Bluetooth
  mySerial.setTimeout(20);

  analogWrite(motorA_pin, MIN_motor_speed);
  analogWrite(motorB_pin, MIN_motor_speed);
  analogWrite(motorC_pin, MIN_motor_speed);
  analogWrite(motorD_pin, MIN_motor_speed);
  delay(2000);
}

void loop() {
  if (mySerial.available()) {
    input = mySerial.parseInt();
    if (input >= 0 && input < 255) {
      motor_speed = input;
    }
  }
  analogWrite(motorA_pin, motor_speed);
  analogWrite(motorB_pin, motor_speed);
  analogWrite(motorC_pin, motor_speed);
  analogWrite(motorD_pin, motor_speed);

  Serial.print("motor speed : ");
  Serial.println(motor_speed);
  Serial.println();
}

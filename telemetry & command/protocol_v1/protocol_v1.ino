/***********************************************************************
쓰로틀 입력 시 : 정수형 숫자
PID 게인 입력 시 : 맨 앞 'a', 각 게인값 마다 '/' (a/KP/KI/KD/RP/RI)
ex) a/3/0.1/0.05/0.1/1
길이 제한 : 18
***********************************************************************/
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5);  // RX, TX

float pitch_stabilize_kp;
float pitch_stabilize_ki;
float pitch_stabilize_kd;
float pitch_rate_kp;
float pitch_rate_ki;
int throttle = 0;

String input = "";
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  mySerial.setTimeout(0);
}
void loop() {
  bluetooth_input();
}
void bluetooth_input() {
  int first, second, third, fourth, length;

  if (mySerial.available()) {
    input = mySerial.readString();
    first = input.indexOf("/", 2);
    second = input.indexOf("/", first + 1);
    third = input.indexOf("/", second + 1);
    fourth = input.indexOf("/", third + 1);
    length = input.length();
    if (input[0] == 'a') {
      String kp = input.substring(2, first);
      String ki = input.substring(first + 1, second);
      String kd = input.substring(second + 1, third);
      String rkp = input.substring(third + 1, fourth);
      String rki = input.substring(fourth + 1, length);
      pitch_stabilize_kp = kp.toFloat();
      pitch_stabilize_ki = ki.toFloat();
      pitch_stabilize_kd = kd.toFloat();
      pitch_rate_kp = rkp.toFloat();
      pitch_rate_ki = rki.toFloat();

    } else {
      throttle = input.toInt();
    }
  }
}

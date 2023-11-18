/***********************************************************************
쓰로틀 입력 시 : 'T' + 정수형 숫자 + '@'
PID 게인 입력 시 : 'R or P' + 각 게인 값, 사이 '/' (R/KP/KI/KD/RP/RI) + '@'
    ex) roll 게인 입력 시 - R/3/0.1/0.05/0.1/1 (길이 제한 : 18)
센서 및 적분 누적값 초기화 : 'I' + 1 + '@'
방향 조종 : 'D' + 'F or B or L or R' + '@'
***********************************************************************/
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

float roll_stabilize_kp;
float roll_stabilize_ki;
float roll_stabilize_kd;
float roll_rate_kp;
float roll_rate_ki;
float pitch_stabilize_kp;
float pitch_stabilize_ki;
float pitch_stabilize_kd;
float pitch_rate_kp;
float pitch_rate_ki;
int throttle = 0;
int offset_roll, offset_pitch;

String input = "";
void setup() {
    Serial.begin(115200);
    mySerial.begin(115200);
    mySerial.setTimeout(0);
}
void loop() { bluetooth_input(); }

void bluetooth_input() {
    // 쓰로틀 : T 100 @ // roll 게인 : R3/0.3/0.05/0.1/0 // pitch 게인 : P3/0.3/0.05/0.1/0 // 센서
    // 리셋 : R1@
    int first, second, third, fourth, length;
    static uint32_t cnt;

    if (mySerial.available()) {
        while (mySerial.available()) {
            input = mySerial.readStringUntil('@');
            if (input[0] == 'T') {
                throttle = input.substring(1).toInt();
            } else if (input[0] == 'P') {
                first = input.indexOf("/");
                second = input.indexOf("/", first + 1);
                third = input.indexOf("/", second + 1);
                fourth = input.indexOf("/", third + 1);
                length = input.length();
                pitch_stabilize_kp = input.substring(1, first).toFloat();
                pitch_stabilize_ki = input.substring(first + 1, second).toFloat();
                pitch_stabilize_kd = input.substring(second + 1, third).toFloat();
                pitch_rate_kp = input.substring(third + 1, fourth).toFloat();
                pitch_rate_ki = input.substring(fourth + 1, length).toFloat();
            } else if (input[0] == 'R') {
                first = input.indexOf("/");
                second = input.indexOf("/", first + 1);
                third = input.indexOf("/", second + 1);
                fourth = input.indexOf("/", third + 1);
                length = input.length();
                roll_stabilize_kp = input.substring(1, first).toFloat();
                roll_stabilize_ki = input.substring(first + 1, second).toFloat();
                roll_stabilize_kd = input.substring(second + 1, third).toFloat();
                roll_rate_kp = input.substring(third + 1, fourth).toFloat();
                roll_rate_ki = input.substring(fourth + 1, length).toFloat();
            } else if (input[0] == 'O') {
                if (input[1] == '1') {
                    filtered_angle_x = 0; // 각 변수와 함수는 메인 코드에 포함
                    filtered_angle_y = 0;
                    filtered_angle_z = 0;
                    calibAccelGyro();
                    initYPR();
                    roll_stabilize_iterm = 0;
                    roll_rate_iterm = 0;
                    pitch_stabilize_iterm = 0;
                    pitch_rate_iterm = 0;
                    offset_roll = 0;
                    offset_pitch = 0;
                }
            } else if (input[0] == 'D') { // Direction
                if (input[1] == 'F') {    // front
                    offset_pitch -= 1;
                } else if (input[1] == 'B') { // back
                    offset_pitch += 1;
                } else if (input[1] == 'L') { // left
                    offset_roll -= 1;
                } else if (input[1] == 'R') { // right
                    offset_roll += 1;
                }
            }
        }
    }
    input = "";
}

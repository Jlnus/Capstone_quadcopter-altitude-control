#include <SoftwareSerial.h>
#include <Wire.h>
SoftwareSerial mySerial(4, 5); // RX, TX

#define THROTTLE_MAX 255
#define THROTTLE_MIN 100
// ESC 칼리브레이션 : Servo.writemicroseconds 이용, Min(800) - Max(2000)

const int MPU_addr = 0x68;                 // MPU Address
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 가속도 센서값, 자이로 센서값

// 세가지 형태의 Roll, Pitch, Yaw 각도를 저장하기 위한 변수
float accel_angle_x, accel_angle_y, accel_angle_z;
// float gyro_angle_x, gyro_angle_y, gyro_angle_z;
float filtered_angle_x, filtered_angle_y, filtered_angle_z;

// 보정형 변수
float baseAcX, baseAcY, baseAcZ;
float baseGyX, baseGyY, baseGyZ;

// dt 변수
float dt;
unsigned long t_now;
unsigned long t_prev;

// 자이로 센서로 계산한 각도 변수
float gyro_x, gyro_y, gyro_z;

// 이중루프PID 변수들
float roll_target_angle = 0.0;
float roll_angle_in;
float roll_angle_error_sum;
float roll_rate_in;
float roll_rate_error_sum;
float roll_stabilize_kp = 2.1;
float roll_stabilize_ki = 0.16;
float roll_stabilize_kd = 0;
float roll_last_angle_in;
float roll_rate_kp = 0.3;
float roll_rate_ki = 0.2;
float roll_stabilize_iterm;
float roll_rate_iterm;
float roll_output;

float pitch_target_angle = 0.0;
float pitch_angle_in;
float pitch_angle_error_sum;
float pitch_rate_in;
float pitch_rate_error_sum;
float pitch_stabilize_kp = 2.3;
float pitch_stabilize_ki = 0.2;
float pitch_stabilize_kd = 0;
float pitch_last_angle_in;
float pitch_rate_kp = 0.3;
float pitch_rate_ki = 0.2;
float pitch_stabilize_iterm;
float pitch_rate_iterm;
float pitch_output;

float yaw_target_angle = 0.0;
float yaw_angle_in;
float yaw_angle_error_sum;
float yaw_rate_in;
float yaw_rate_error_sum;
float yaw_stabilize_kp = 3;
float yaw_stabilize_ki = 0;
float yaw_stabilize_kd = 0;
float yaw_last_angle_in;
float yaw_rate_kp = 0.2;
float yaw_rate_ki = 0;
float yaw_stabilize_iterm;
float yaw_rate_iterm;
float yaw_output;

float base_roll_target_angle;
float base_pitch_target_angle;
float base_yaw_target_angle;

// 모터의 속도제어 변수
uint32_t throttle = 0;
uint32_t motorA_speed, motorB_speed, motorC_speed, motorD_speed;

// 드론 각 모터 핀 설정(시계방향으로 ABCD)
const int motorA_pin = 3;
const int motorB_pin = 9;
const int motorC_pin = 10;
const int motorD_pin = 11;

// 블루투스 입력 변수
String input = "";

void setup() {
    Serial.begin(115200);
    mySerial.begin(115200); // HM-10(블루투스)로 부터 데이터를 받기 위해서 선언
    mySerial.setTimeout(5);

    initMPU6050();    // MPU6050 초기화
    calibAccelGyro(); // 가속도, 자이로 센서의 초기 평균값 계산
    initDT();         // 시간 간격 초기화
    initYPR();
    initMotorSpeed(); // 모터 초기값 부여 (arm)
}

void initMPU6050() {
    // MPU6050 초기화
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.setClock(400000);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true); // I2C의 제어권을 반환
}

void readAccelGyro() {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false); // I2C의 제어권을 이어감
    Wire.requestFrom(MPU_addr, 14, true);

    // 가속도, 자이로 센서의 값을 읽어온다.
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    Tmp = Wire.read() << 8 | Wire.read();
    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();
}

void calibAccelGyro() {
    float sumAcX = 0;
    float sumAcY = 0;
    float sumAcZ = 0;
    float sumGyX = 0;
    float sumGyY = 0;
    float sumGyZ = 0;

    readAccelGyro();

    // 초기 보정값은 10번의 가속도 자이로 센서의 값을 받아 평균값 계산
    for (int i = 0; i < 10; i++) {
        readAccelGyro();

        sumAcX += AcX, sumAcY += AcY, sumAcZ += AcZ;
        sumGyX += GyX, sumGyY += GyY, sumGyZ += GyZ;

        delay(100);
    }

    baseAcX = sumAcX / 10;
    baseAcY = sumAcY / 10;
    baseAcZ = sumAcZ / 10;

    baseGyX = sumGyX / 10;
    baseGyY = sumGyY / 10;
    baseGyZ = sumGyZ / 10;
}

void initDT() {
    t_prev = micros(); // 초기 t_prev값은 근사값
}

void initYPR() {
    // 초기 호버링의 각도를 잡아주기 위해서 Roll, Pitch, Yaw 상보필터 구하는 과정을 10번 반복
    for (int i = 0; i < 10; i++) {
        readAccelGyro();
        calcDT();
        calcAccelYPR();
        calcGyroYPR();
        calcFilteredYPR();

        base_roll_target_angle += filtered_angle_y;
        base_pitch_target_angle += filtered_angle_x;
        base_yaw_target_angle += filtered_angle_z;

        delay(100);
    }

    base_roll_target_angle /= 10;
    base_pitch_target_angle /= 10;
    base_yaw_target_angle /= 10;

    // 초기 타겟 각도 지정
    roll_target_angle = base_roll_target_angle;
    pitch_target_angle = base_pitch_target_angle;
    yaw_target_angle = base_yaw_target_angle;
}

void initMotorSpeed() {
    // esc에 arm 신호 부여
    analogWrite(motorA_pin, THROTTLE_MIN);
    delay(1000);
    analogWrite(motorB_pin, THROTTLE_MIN);
    delay(1000);
    analogWrite(motorC_pin, THROTTLE_MIN);
    delay(1000);
    analogWrite(motorD_pin, THROTTLE_MIN);
    delay(1000);
}

static uint32_t tmp = 0;
void loop() {
    readAccelGyro();
    calcDT();
    calcAccelYPR();    // 가속도 센서 Roll, Pitch, Yaw의 각도 계산
    calcGyroYPR();     // 자이로 센서 Roll, Pitch, Yaw의 각도 계산
    calcFilteredYPR(); // 상보필터를 적용해 Roll, Pitch, Yaw의 각도 계산

    calcYPRtoDualPID(); // 이중루프PID
    calcMotorSpeed();   // PID출력값을 기준으로 모터의 속도를 계산
    bluetooth_input();  // 블루투스 입력값 수신
    updateMotorSpeed(); // 제어값과 입력값으로 모터 속도 업데이트

    tmp++;
    if (tmp == 500) {
        mySerial.println("");
        mySerial.print("DT:");
        mySerial.print(dt * 1000);
        mySerial.print(" #T:");
        mySerial.print(throttle);
        mySerial.print(" #R_to:");
        mySerial.print(roll_target_angle);
        mySerial.print(" #P_to:");
        mySerial.println(pitch_target_angle);
        mySerial.print("#r:");
        mySerial.print(roll_stabilize_kp, 1);
        mySerial.print("/");
        mySerial.print(roll_stabilize_ki);
        mySerial.print("/");
        mySerial.print(roll_stabilize_kd, 3);
        mySerial.print("/");
        mySerial.print(roll_rate_kp);
        mySerial.print("/");
        mySerial.println(roll_rate_ki);
        mySerial.print("#p:");
        mySerial.print(pitch_stabilize_kp, 1);
        mySerial.print("/");
        mySerial.print(pitch_stabilize_ki);
        mySerial.print("/");
        mySerial.print(pitch_stabilize_kd, 3);
        mySerial.print("/");
        mySerial.print(pitch_rate_kp);
        mySerial.print("/");
        mySerial.println(pitch_rate_ki);
        mySerial.print("#R:");
        mySerial.print(roll_angle_in);
        mySerial.print(" #P:");
        mySerial.print(pitch_angle_in);
        mySerial.print(" #Y:");
        mySerial.println(yaw_angle_in);
        mySerial.print(" #A:");
        mySerial.print(motorA_speed);
        mySerial.print(" #B:");
        mySerial.print(motorB_speed);
        mySerial.print(" #C:");
        mySerial.print(motorC_speed);
        mySerial.print(" #D:");
        mySerial.println(motorD_speed);
        tmp = 0;
    }
}

void calcDT() {
    t_now = micros();
    dt = (t_now - t_prev) / 1000000.0;
    t_prev = t_now;
}

void calcAccelYPR() {
    float accel_x, accel_y, accel_z;
    float accel_xz, accel_yz;
    const float RADIANS_TO_DEGREES = 180 / 3.14159;

    accel_x = AcX - baseAcX;
    accel_y = AcY - baseAcY;
    accel_z = AcZ + (16384 - baseAcZ);

    // accel_angle_y는 가속도 Roll 각
    accel_yz = sqrt(pow(accel_y, 2) + pow(accel_z, 2));
    accel_angle_y = atan(-accel_x / accel_yz) * RADIANS_TO_DEGREES;

    // accel_angle_x는 가속도 Pitch 각
    accel_xz = sqrt(pow(accel_x, 2) + pow(accel_z, 2));
    accel_angle_x = atan(accel_y / accel_xz) * RADIANS_TO_DEGREES;

    accel_angle_z = 0;
}

void calcGyroYPR() {
    const float GYROXYZ_TO_DEGREES_PER_SEC = 131;

    gyro_x = (GyX - baseGyX) / GYROXYZ_TO_DEGREES_PER_SEC;
    gyro_y = (GyY - baseGyY) / GYROXYZ_TO_DEGREES_PER_SEC;
    gyro_z = (GyZ - baseGyZ) / GYROXYZ_TO_DEGREES_PER_SEC;
}

void calcFilteredYPR() {
    const float ALPHA = 0.96;
    float tmp_angle_x, tmp_angle_y, tmp_angle_z;

    tmp_angle_x = filtered_angle_x + gyro_x * dt;
    tmp_angle_y = filtered_angle_y + gyro_y * dt;
    tmp_angle_z = filtered_angle_z + gyro_z * dt;

    // 상보필터 통과
    filtered_angle_x = ALPHA * tmp_angle_x + (1.0 - ALPHA) * accel_angle_x;
    filtered_angle_y = ALPHA * tmp_angle_y + (1.0 - ALPHA) * accel_angle_y;
    filtered_angle_z = tmp_angle_z; // 자이로값만 이용
}

void dualPID(float target_angle,
             float angle_in,
             float rate_in,
             float stabilize_kp,
             float stabilize_ki,
             float rate_kp,
             float rate_ki,
             float &stabilize_iterm,
             float &rate_iterm,
             float &output) {
    float angle_error;
    float desired_rate;
    float rate_error;
    float stabilize_pterm, rate_pterm;


    // 이중루프PID알고리즘
    angle_error = target_angle - angle_in;
    stabilize_pterm = stabilize_kp * angle_error;
    stabilize_iterm += stabilize_ki * angle_error * dt; // 안정화 적분항

    desired_rate = stabilize_pterm;
    rate_error = desired_rate - rate_in;
    rate_pterm = rate_kp * rate_error;       // 각속도 비례항
    rate_iterm += rate_ki * rate_error * dt; // 각속도 적분항

    // 적분항 누적 제한
    stabilize_iterm = constrain(stabilize_iterm, -10, 10);
    rate_iterm = constrain(rate_iterm, -10, 10);

    // 초기 이륙 시 진동 누적값 방지
    if (throttle <= 50) {
        rate_iterm = 0;
        stabilize_iterm = 0;
    }

    // 최종 출력 : 각속도 비례항 + 각속도 적분항 + 안정화 적분항
    output = rate_pterm + rate_iterm + stabilize_iterm;
    output = constrain(output, -20, 20); // 출력값 제한
}

void calcYPRtoDualPID() {

    roll_angle_in = filtered_angle_y;
    roll_rate_in = gyro_y;

    dualPID(roll_target_angle,
            roll_angle_in,
            roll_rate_in,
            roll_stabilize_kp,
            roll_stabilize_ki,
            roll_rate_kp,
            roll_rate_ki,
            roll_stabilize_iterm,
            roll_rate_iterm,
            roll_output);

    pitch_angle_in = filtered_angle_x;
    pitch_rate_in = gyro_x;

    dualPID(pitch_target_angle,
            pitch_angle_in,
            pitch_rate_in,
            pitch_stabilize_kp,
            pitch_stabilize_ki,
            pitch_rate_kp,
            pitch_rate_ki,
            pitch_stabilize_iterm,
            pitch_rate_iterm,
            pitch_output);

    yaw_angle_in = filtered_angle_z;
    yaw_rate_in = gyro_z;

    dualPID(yaw_target_angle,
            yaw_angle_in,
            yaw_rate_in,
            yaw_stabilize_kp,
            yaw_stabilize_ki,
            yaw_rate_kp,
            yaw_rate_ki,
            yaw_stabilize_iterm,
            yaw_rate_iterm,
            yaw_output);
}

void calcMotorSpeed() {
    //    yaw_output = 0;
    //    pitch_output = 0;
    //    roll_output = 0;
    motorA_speed = (throttle == 0) ? 100
                                   : throttle + 100 - yaw_output + roll_output + pitch_output ;
    motorB_speed = (throttle == 0) ? 100
                                   : throttle + 100 + yaw_output - roll_output + pitch_output;
    motorC_speed = (throttle == 0) ? 100
                                   : throttle + 100 - yaw_output - roll_output - pitch_output;
    motorD_speed = (throttle == 0) ? 100
                                   : throttle + 100 + yaw_output + roll_output - pitch_output;

    //   모터 속도 밸런스 - 각 경계값마다의 보정
    if (105 <= motorA_speed && motorA_speed < 255) {
        motorA_speed -= 3; // 배터리 완충 : 3 -> 50% 이하 : 11
    }
    if (105 <= motorD_speed && motorD_speed < 255) {
        motorD_speed -= 5; // 배터리 완충 : 5 -> 50% 이하 : 7
    }

    //   if (105 <= motorB_speed && motorB_speed < 140) {  // yaw 비교
    //     motorB_speed += 3;
    //   } else if (140 <= motorB_speed && motorB_speed < 150) {
    //     motorB_speed += 5;
    //   } else if (150 <= motorB_speed && motorB_speed < 160) {
    //     motorB_speed += 7;
    //   } else if (160 <= motorB_speed && motorB_speed < 255) {
    //     motorB_speed += 8;
    //   }

    //   if (105 <= motorC_speed && motorC_speed < 173) {  // A, C 비교
    //     motorC_speed += 2;
    //   } else if (173 <= motorC_speed && motorC_speed < 255) {
    //     motorC_speed += 3;
    //   }

    //   if (motorD_speed >= 105 && motorD_speed < 140) {  // (BD 비교값 - Yaw축 비교값)
    //     motorD_speed -= (3 - 3);
    //   } else if (motorD_speed >= 140 && motorD_speed < 150) {
    //     motorD_speed -= (3 - 5);
    //   } else if (motorD_speed >= 150 && motorD_speed < 160) {
    //     motorD_speed -= (3 - 7);
    //   } else if (motorD_speed >= 160 && motorD_speed < 168) {
    //     motorD_speed -= (3 - 8);
    //   } else if (motorD_speed >= 168 && motorD_speed < 183) {
    //     motorD_speed -= (4 - 8);
    //   } else if (motorD_speed >= 183 && motorD_speed < 255) {
    //     motorD_speed -= (3 - 8);
    //   }
}

void bluetooth_input() {
    // 쓰로틀 : T100@
    // roll 게인 : R3/0.3/0.05/0.1/0 // pitch 게인 : P3/0.3/0.05/0.1/0
    // 센서 리셋 : R1@
    // 방향 조정 : DF@ DB@ DL@ DR@
    int first, second, third, fourth, length;
    static uint32_t cnt;

    if (mySerial.available()) {
        while (mySerial.available()) {
            input = mySerial.readStringUntil('@');
            if (input[0] == 'T') {
                throttle = input.substring(1).toInt();
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
            } else if (input[0] == 'I') {
                if (input[1] == '1') {
                    filtered_angle_x = 0;
                    filtered_angle_y = 0;
                    filtered_angle_z = 0;
                    calibAccelGyro();
                    initYPR();
                    roll_stabilize_iterm = 0;
                    roll_rate_iterm = 0;
                    pitch_stabilize_iterm = 0;
                    pitch_rate_iterm = 0;
                    roll_target_angle = 0;
                    pitch_target_angle = 0;
                }
            } else if (input[0] == 'D') { // Direction
                if (input[1] == 'F') {    // front
                    pitch_target_angle -= 1;
                } else if (input[1] == 'B') { // back
                    pitch_target_angle += 1;
                } else if (input[1] == 'L') { // left
                    roll_target_angle -= 1;
                } else if (input[1] == 'R') { // right
                    roll_target_angle += 1;
                }
            }
        }
    }
    input = "";
}

void updateMotorSpeed() {
    // 모터속도 제한
    analogWrite(motorA_pin, constrain(motorA_speed, 100, 255));
    analogWrite(motorB_pin, constrain(motorB_speed, 100, 255));
    analogWrite(motorC_pin, constrain(motorC_speed, 100, 255));
    analogWrite(motorD_pin, constrain(motorD_speed, 100, 255));
}
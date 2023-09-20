/*
      Servo 라이브러리를 이용한 ESC calibration 
      ESC에 최소 신호와 최고 신호 등록
*/
#include <Servo.h>

#define MAX_SIGNAL 2000
#define MIN_SIGNAL 800

int motorA_pin = 3;
int motorB_pin = 9;
int motorC_pin = 10;
int motorD_pin = 11;
int motor_speed = 0;
int input = 0;

Servo motorA, motorB, motorC, motorD;

void setup() {
    Serial.begin(9600); // open the serial port
    Serial.println("Program begin...");
    Serial.println("This program will calibrate the ESC.");
    
    // 핀 할당
    motorA.attach(motorA_pin);
    motorB.attach(motorB_pin);
    motorC.attach(motorC_pin);
    motorD.attach(motorD_pin);

    Serial.println("Now writing maximum output.");
    Serial.println("Turn on power source, then wait 2 seconds and press any key.");
    motorA.writeMicroseconds(MAX_SIGNAL);
    motorB.writeMicroseconds(MAX_SIGNAL);
    motorC.writeMicroseconds(MAX_SIGNAL);
    motorD.writeMicroseconds(MAX_SIGNAL);

    // Wait for input
    while (!Serial.available()); 
    // using while "wait" for input, using if at loop stop "when" serial input
    Serial.read();
    delay(3000);
    // Send min output
    Serial.println("Sending minimum output");
    motorA.writeMicroseconds(MIN_SIGNAL);
    motorB.writeMicroseconds(MIN_SIGNAL);
    motorC.writeMicroseconds(MIN_SIGNAL);
    motorD.writeMicroseconds(MIN_SIGNAL);
    delay(3000);
}

void loop() {}

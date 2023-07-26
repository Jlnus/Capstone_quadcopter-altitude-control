const int motorA_pin = 3;
const int motorB_pin = 9;
const int motorC_pin = 10;
const int motorD_pin = 11;

int motor_speed = 0;
int input = 0;
void setup() {
  Serial.begin(9600);
  delay(2000);
}

void loop() {

  if (Serial.available()) {
    input = Serial.parseInt();
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



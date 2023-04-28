const int motor_pin = 5;

void setup() {
    // put your setup code here, to run once:
    // Serial.begin(115200);
    pinMode(motor_pin, OUTPUT);
    // Serial.println("start");
    delay(2000);
}

void loop() {
    // put your main code here, to run repeatedly:
    analogmotor();
}

void analogmotor() {
    analogWrite(motor_pin, 25);
    // (25 / 255)*100 = 9.8%의 듀티사이클
    delay(5000);
    analogWrite(motor_pin, 0);
    delay(3000);
}

void digitalmotor() {
    digitalWrite(motor_pin, HIGH);
    delay(2); // ms, 2ms = 0.002s
    digitalWrite(motor_pin, LOW);
    delay(8);
}

#include <Wire.h>

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float accel_angle_x, accel_angle_y, accel_angle_z;
float gyro_angle_y;
float baseAcX, baseAcY, baseAcZ;
float baseGyX, baseGyY, baseGyZ;
float dt;
unsigned long t_now;
unsigned long t_prev;
float gyro_y;

void setup() {
    Serial.begin(115200);

    Wire.begin();
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
    calibAccelGyro();  // 센서값 평균 read
    t_prev = micros(); // 초기 시간 부여
}

void readAccelGyro() {
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false); // I2C의 제어권을 이어감
    Wire.requestFrom(0x68, 14, true);

    // 가속도, 자이로 센서의 값을 읽어옴
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

    // 초기 보정값은 100번의 가속도 자이로 센서의 값의 평균
    for (int i = 0; i < 100; i++) {
        readAccelGyro();
        sumAcX += AcX, sumAcY += AcY, sumAcZ += AcZ;
        sumGyX += GyX, sumGyY += GyY, sumGyZ += GyZ;
        delay(10);
    }
    baseAcX = sumAcX / 100;
    baseAcY = sumAcY / 100;
    baseAcZ = sumAcZ / 100;
    baseGyX = sumGyX / 100;
    baseGyY = sumGyY / 100;
    baseGyZ = sumGyZ / 100;
}

void loop() {
    readAccelGyro();

    calcDT();
    calcAccelangle();
    calcGyroangle();
    //   Serial.print("Accel roll angle : ");

    Serial.print(accel_angle_y);
    Serial.print(" // ");
    //   Serial.print("Gyro roll angle : ");
    Serial.println(gyro_angle_y);
    delay(200);
}

void calcDT() {
    t_now = micros();
    dt = (t_now - t_prev) / 1000000.0;
    t_prev = t_now;
}
////////////////////////
void calcAccelangle() {
    float accel_x, accel_y, accel_z;
    float accel_xz, accel_yz;
    const float RADIANS_TO_DEGREES = 180 / 3.14159;

    accel_x = AcX - baseAcX;
    accel_y = AcY - baseAcY;
    accel_z = AcZ + (16384 - baseAcZ); // 중력가속도 제외

    // Roll각
    accel_yz = sqrt(pow(accel_y, 2) + pow(accel_z, 2));
    accel_angle_y = atan(-accel_x / accel_yz) * RADIANS_TO_DEGREES;
}
/////////////////////////
void calcGyroangle() {
    const float GYROXYZ_TO_DEGREES_PER_SEC = 131; // 131도/s

    gyro_y = (GyY - baseGyY) / GYROXYZ_TO_DEGREES_PER_SEC;
    gyro_angle_y += gyro_y * dt;
}

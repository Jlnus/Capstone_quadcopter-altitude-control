/*************************
    GY-86 고도 계산
*************************/
#include "MS5611.h"
#include <Wire.h>

float altitude, pressure, temperature;
const float sea_pressure = 1009.9;  // 해면기압

MS5611 MS5611(0x77);  // MS5611 class 생성

void setup() {
  Serial.begin(115200);
  Wire.setClock(400000);
  if (MS5611.begin() == true) {
    Serial.print("MS5611 begin");
  }
  MS5611.setOversampling(OSR_ULTRA_HIGH);  // 모드 설정
}

void loop() {
  MS5611.read();  // 센서값 read
  pressure = MS5611.getPressure();
  temperature = MS5611.getTemperature();
  altitude = getAltitude(pressure, temperature);

    Serial.print("Temperture : ");
    Serial.print(temperature);
    Serial.print("  Pressure : ");
    Serial.print(pressure);
    Serial.print("  Altitude : ");
//   Serial.println(altitude);
  delay(100);
}

float getAltitude(float pressure, float temperature) {
  return (1.0f - powf((pressure / sea_pressure), 0.190284f)) * (temperature + 273.15f) / 0.0065f;
  //   return (1.0f - powf((pressure / sea_pressure), 0.190284f)) * 44307.694f;
}

/******************************
  GY-86 고도 계산 & LPF 필터링
*******************************/
#include "MS5611.h"
#include <Wire.h>

float altitude1, altitude2, pressure, temperature, filtered_pressure;
const float sea_pressure = 1009.9; // 해면기압
// https://www.kma.go.kr/weather/observation/aws_table_popup.jsp
unsigned long now;

// LPF 변수
uint8_t LPF_fc = 5;                          // cut-off frequency(차단 주파수(기준))
float LPF_dt = 0.02f;                        // time interval(시간 간격)
float LPF_lambda = 2 * PI * LPF_fc * LPF_dt; // lambda
float LPF_x = 0.0f;                          // 입력 데이터
float LPF_filtered_x = 0.0f;                 // LPF를 거친 데이터
float LPF_prev_filtered_x = 0.0f;            // 이전에 LPF를 거친 데이터

// filter2
float pressure_2;
float altitude3;
float prev_filtered2;
float filtered2_pressure;

// filter3
float pressure_3;
float altitude4;
float pressurelist[20];

MS5611 MS5611(0x77); // MS5611 class 생성

void setup() {
    Serial.begin(115200);
    Wire.setClock(400000);
    if (MS5611.begin() == true) {
        Serial.print("MS5611 begin");
    }
    MS5611.setOversampling(OSR_ULTRA_HIGH); // 모드 설정
}

void loop() {
    MS5611.read(); // 센서값 read
    pressure = MS5611.getPressure();
    temperature = MS5611.getTemperature();
    filtered_pressure = low_pass_filter(pressure);           // 필터링 된 압력 값
    altitude1 = getAltitude(pressure, temperature);          // 고도 계산
    altitude2 = getAltitude(filtered_pressure, temperature); // 필터링 후 고도 계산
    pressure_2 = filter2(pressure);
    altitude3 = getAltitude(pressure_2, temperature);
    pressure_3 = filter3(pressure);
    altitude4 = getAltitude(pressure_3, temperature);
    Serial.print(altitude1);
    Serial.print("/");
    //   Serial.print(",");
    //   Serial.print(altitude2);
    //   Serial.print(",");
    //   Serial.print(altitude3);
    //   Serial.print(",");
    Serial.println(altitude4);
}

float getAltitude(float pressure, float temperature) {
    return (1.0f - powf((pressure / sea_pressure), 0.190284f)) * (temperature + 273.15f) / 0.0065f;
}

float low_pass_filter(float pressure) {
    LPF_x = pressure; // 입력 데이터

    // Low Pass Filter 방정식
    LPF_filtered_x =
        ((LPF_lambda / (1 + LPF_lambda)) * LPF_x) + ((1 / (1 + LPF_lambda)) * LPF_prev_filtered_x);
    LPF_prev_filtered_x = LPF_filtered_x; // 현재 데이터 저장

    return LPF_filtered_x;
}

float filter2(float pressure) {
    filtered2_pressure =
        filtered2_pressure + (((pressure + prev_filtered2) / 2.0) - filtered2_pressure) * 0.5;
    prev_filtered2 = filtered2_pressure;
    return filtered2_pressure;
}

float filter3(float pressure) {
    for (int i = 0; i < 19; i++) {
        pressurelist[i] = pressurelist[i + 1];
    }
    pressurelist[19] = pressure;
    float tmppressure = 0;
    for (int i = 0; i < 20; i++) {
        tmppressure += pressurelist[i];
    }

    return tmppressure / 20.0;
}
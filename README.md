# 기압 센서를 이용한 고도 유지 호버링 
Quadcopter altitude control with barometric pressure sensor
캡스톤디자인(2023.03.02 ~ )

Main file : quadcopter.ino

## 부품정보
| 부품 | 이름 |
|---|---|
| 프레임	| TAROT Whirlwind F450 드론 프레임 키트 V2 |
| 랜딩기어	| 랜딩 기어 높이 확장 다리 익스텐더 지원 프로텍터 F450/F550 쿼드 콥터 용 |
|프로펠러	|[GF] 10x4.5 멀티콥터용 프로펠러 오렌지 색상 (DJI Type) - 정방향, 역방향 1조|
|ESC|	[GT-Drone] EC-X3 ESC for Multicopters (30A/OPTO/COB/6S)|
|모터	| T2212-920KV 브러쉬리스 모터 CCW (V2) |
|  | 	T2212-920KV 브러쉬리스 모터 CW (V2)|
|UBEC	|[Hobbywing] 3A U-BEC (Noise Free)|
|배터리	|PT-B1800N-SP65 (11.1V, 3S1P, 65C /JST-XT) |
|배터리 케이블|	XT60 전원케이블 (12AWG, 10cm)|
|MCU|	아두이노 나노 V3.0 호환 보드 CH340|
|송수신기	| HM-10(Bluetooth LE), ESP8266(WiFi) |
|브레드보드	|브레드보드(400핀)|
|IMU 센서	| GY-86 |


## 제어 알고리즘
### 전체 루프
<img width="519" alt="image" src="https://github.com/Jlnus/Capstone_quadcopter-altitude-control/assets/69241185/a952cf35-439f-43bc-b66f-43e829aed129">

### 이중 PID
<img width="725" alt="image" src="https://github.com/Jlnus/Capstone_quadcopter-altitude-control/assets/69241185/c8737bf3-a1e2-4831-b95a-423009fbb96f">



## Command & Telemetry
### Bluetooth
1. 스마트폰(안드로이드) 어플 - Custom Bluetooth Controller 사용
<img src="https://github.com/Jlnus/Capstone_quadcopter-altitude-control/assets/69241185/d4e4e050-a4d4-4d9e-b390-1fc2f0696739"  width="200">

- 구성(각 버튼과 슬라이더 command 수정, protocol.ino 참조)

<img src="https://github.com/Jlnus/Capstone_quadcopter-altitude-control/assets/69241185/bb6e92cd-f0b1-4fd7-a404-460fd7545f78" width="150">
 

2. PC 연결
ble-serial - https://github.com/Jakeler/ble-serial

### WiFi
ESP8266 활용법 - https://velog.io/@jinus/ESP8266-WiFi-모듈-활용하기


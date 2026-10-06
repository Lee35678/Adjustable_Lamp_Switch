# Adjustable_Lamp_Switch

LilyGO T-Display-S3에서 BH1750 조도 값과 로터리 엔코더 설정값을 비교해 화면에 램프 상태를 표시하는 PlatformIO 예제

## 개요

BH1750 조도 센서로 주변 밝기(lux)를 읽고, 로터리 엔코더로 기준값(0~255)을 조절합니다. 측정한 밝기가 기준값보다 낮으면 화면에 주황색 원(램프 켜짐)을, 아니면 검은 원(꺼짐)을 그립니다. 사용자가 켜짐 기준을 직접 조절할 수 있는 자동 조명 스위치를 화면으로 시뮬레이션한 구조이며, 실제 램프나 릴레이 출력 코드는 없습니다. 작성 시기는 2024년 10월입니다(커밋 기록 기준).

## 하드웨어

- 보드: LilyGO T-Display-S3 (`board = lilygo-t-display-s3`, ESP32-S3, 화면 내장)
- 입력: BH1750 조도 센서(I2C), 로터리 엔코더(A/B 2상)
- 출력: 보드 내장 TFT 화면

| 신호 | GPIO | 설정 |
|------|------|------|
| BH1750 SDA (`SDA_PIN`) | 17 | `Wire.begin(17, 18)` |
| BH1750 SCL (`SCL_PIN`) | 18 | |
| 엔코더 A상 (`pulseA`) | 43 | `INPUT_PULLUP`, 인터럽트 `CHANGE` |
| 엔코더 B상 (`pulseB`) | 44 | `INPUT_PULLUP`, 인터럽트 `CHANGE` |

## 동작 방식

1. `setup()`
   - 시리얼을 115200 bps로 엽니다(`-DARDUINO_USB_CDC_ON_BOOT=1`로 USB 시리얼 사용).
   - 엔코더 A/B 핀 모두에 `handleRotary()`를 `CHANGE` 인터럽트로 등록합니다.
   - I2C를 SDA 17 / SCL 18로 시작하고 BH1750을 초기화합니다.
   - 화면에 `Lamp Control`, `Lux : `, `Set : ` 라벨을 그립니다.
2. 엔코더 인터럽트 `handleRotary()` (`IRAM_ATTR`)
   - 이전 상태 2비트와 현재 A/B 상태 2비트를 합친 4비트 값으로 회전 방향을 판별합니다.
   - `0b1101, 0b0100, 0b0010, 0b1011`이면 `encoderValue++`, `0b1110, 0b0111, 0b0001, 0b1000`이면 `encoderValue--`
   - 값은 0~255 범위로 제한합니다.
3. `loop()`: 500 ms마다 다음을 반복합니다.
   - `lightMeter.readLightLevel()`로 lux를 읽어 `Lux :` 옆에 소수 둘째 자리까지 표시합니다.
   - 현재 엔코더 값을 `Set :` 옆에 표시합니다.
   - `lux < encoderValue`이면 화면 (240, 110)에 반지름 50의 주황색 원, 아니면 검은 원을 그립니다.

## 개발 환경

| 항목 | 값 |
|------|----|
| 도구 | PlatformIO |
| 플랫폼 | `espressif32` |
| 프레임워크 | `arduino` |
| 빌드 플래그 | `-DARDUINO_USB_CDC_ON_BOOT=1` |
| 라이브러리 | `bodmer/TFT_eSPI @ ^2.5.22`, `adafruit/Adafruit Unified Sensor @ ^1.1.7`, `claws/BH1750 @ ^1.3.0` |
| 모니터 속도 | `monitor_speed = 115200` |

## 빌드 및 업로드

```bash
pio run -t upload
pio device monitor
```

## 폴더 구조

```
Adjustable_Lamp_Switch/
├── platformio.ini
└── src/
    └── main.cpp
```

## 참고

- 버튼용 핸들러 `buttonClicked()`가 정의되어 있지만 어느 핀에도 `attachInterrupt`로 연결되지 않아 실제로는 호출되지 않습니다.
- `Adafruit Unified Sensor`는 `lib_deps`에만 있고 코드에서 사용하지 않습니다.
- 설정값은 엔코더 단계 수(0~255)를 그대로 lux와 비교하므로, 기준은 최대 255 lux까지만 설정할 수 있습니다.
- TFT_eSPI의 T-Display-S3용 화면 설정은 이 저장소에 포함되어 있지 않습니다.

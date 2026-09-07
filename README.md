# Arduino DC Motor Speed Controller

PWM-based DC motor speed controller built with an Arduino Nano, a potentiometer and an IRFZ44N MOSFET.

The project uses a potentiometer as a throttle-like input. The Arduino Nano reads the potentiometer position and generates a PWM signal to control the MOSFET, regulating the speed of a small DC motor.

The motor is powered by an external 5 V / 1 A USB power supply, while the Arduino is responsible only for the control signal.

## Features

- Arduino Nano based control
- PWM speed control
- Potentiometer-based throttle
- IRFZ44N N-channel MOSFET
- 1N4007 flyback diode for motor protection
- External 5 V / 1 A motor power supply
- Common ground between Arduino and motor supply
- PlatformIO project
- Simple and low-cost design

## Hardware

| Component | Description |
|-----------|-------------|
| Arduino Nano | Main controller |
| IRFZ44N | N-channel power MOSFET |
| BK10 potentiometer | Speed/throttle control |
| 1N4007 | Flyback protection diode |
| DC Motor | 6 V, 600 RPM motor |
| 220 Ω resistor | MOSFET gate resistor |
| 5 V / 1 A USB power supply | External motor power source |

## Motor

The motor used in this project is rated at:

- Nominal voltage: **6 V**
- Nominal speed: **600 RPM**
- Operating voltage: **5 V**

The motor is intentionally operated at 5 V instead of its nominal 6 V.

As a result, the maximum speed is expected to be lower than the nominal 600 RPM. The actual speed depends on the motor, mechanical load, friction, supply voltage and PWM duty cycle.

The project currently controls the motor speed using PWM but does **not** measure the actual RPM.

## Circuit

The motor is controlled using the IRFZ44N in a low-side switching configuration.

```text
                    +5 V
                     │
                     │
                   Motor
                     │
                     │
                  Drain
                ┌─────────┐
Arduino D9 ─220Ω─┤  Gate   │
                │ IRFZ44N │
GND ────────────┤ Source  │
                └─────────┘
                     │
                    GND
```

### MOSFET connections

With the IRFZ44N viewed from the front, with the text facing you:

```text
    ┌─────────────┐
    │   IRFZ44N   │
    └─────────────┘
       │   │   │
       1   2   3
       │   │   │
     Gate Drain Source
```

Connections:

- **Gate → 220 Ω resistor → Arduino D9**
- **Drain → Motor negative**
- **Source → GND**
- **Motor positive → +5 V external supply**

## Flyback diode

The 1N4007 is connected in parallel with the motor to protect the MOSFET from voltage spikes generated when the motor is switched off.

```text
             +5 V
              │
            Motor
              │
              ├───────────────┐
              │               │
              │             ──|<── 1N4007
              │               │
              └─── Drain ─────┘
```

The **stripe on the 1N4007 must be connected to the positive side of the motor (+5 V)**.

In other words:

```text
Motor + ───────|<|────── Motor -
               ↑
          stripe/cathode
```

The diode must be connected **in parallel**, not in series.

## Potentiometer

The BK10 potentiometer is used as a throttle-like control.

```text
              +5 V
               │
               │
          ┌────┴────┐
          │   POT   │
          └────┬────┘
               │
               └──────── A0
               │
              GND
```

Connections:

- One outer pin → **5 V**
- Center pin → **A0**
- Other outer pin → **GND**

Rotating the potentiometer changes the voltage read by the Arduino.

## Power supply

The motor is powered from an external **5 V / 1 A USB power supply**.

The Arduino and motor supply share a common ground:

```text
External USB GND ───── Arduino GND
                          │
                          └──── MOSFET Source
```

The external +5 V supply is connected to the motor.

```text
External USB +5 V ───── Motor +
External USB GND ────── Arduino GND
```

The external motor supply is used so that the motor current does not have to be supplied through the Arduino's 5 V circuitry.

**Do not connect the external +5 V directly to the Arduino 5 V pin when the Arduino is already powered through USB.**

## Arduino Pinout

| Arduino Nano | Component |
|--------------|-----------|
| A0 | Potentiometer center pin |
| D9 | MOSFET Gate through 220 Ω |
| GND | MOSFET Source |
| GND | Potentiometer |
| 5 V | Potentiometer |

The motor itself is powered from the external 5 V supply.

## How it works

The potentiometer produces an analog voltage between 0 V and 5 V.

The Arduino Nano's ADC converts this voltage into a value between:

```text
0 ───────────────────── 1023
```

This value is then mapped to the PWM range:

```text
0 ───────────────────── 255
```

The resulting PWM signal is sent to the MOSFET through pin D9.

```text
Potentiometer
      │
      ▼
     A0
      │
      ▼
  ADC 0–1023
      │
      ▼
  Mapping
      │
      ▼
   PWM 0–255
      │
      ▼
    D9
      │
      ▼
  IRFZ44N
      │
      ▼
    Motor
```

The MOSFET rapidly switches the motor's current on and off. By changing the PWM duty cycle, the average power delivered to the motor is controlled.

The potentiometer therefore behaves similarly to a simple throttle:

```text
Potentiometer position
        │
        ▼
    PWM duty cycle
        │
        ▼
    Motor speed
```

## Software

The project is developed using **PlatformIO** with the Arduino framework.

### Project structure

```text
arduino-dc-motor-controller/
│
├── src/
│   └── main.cpp
│
├── platformio.ini
└── README.md
```

### `platformio.ini`

```ini
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328
framework = arduino
upload_speed = 57600
```

The `upload_speed = 57600` setting is used for Arduino Nano boards with the **ATmega328P Old Bootloader**.

### `src/main.cpp`

```cpp
#include <Arduino.h>

const int potPin = A0;
const int motorPin = 9;

void setup() {
    pinMode(motorPin, OUTPUT);
}

void loop() {
    int potValue = analogRead(potPin);

    int pwm = map(potValue, 0, 1023, 0, 255);

    analogWrite(motorPin, pwm);

    delay(10);
}
```

## PWM control

The Arduino Nano uses an 8-bit PWM value:

```text
0   → 0% duty cycle
64  → 25%
128 → 50%
192 → 75%
255 → 100%
```

Increasing the PWM duty cycle generally increases the motor speed.

However, PWM value and motor RPM are **not directly proportional in a precise way**. The actual speed depends on the motor's characteristics and mechanical load.

## Speed estimation

The motor is rated at approximately **600 RPM at 6 V**.

Since the motor is operated at 5 V, its theoretical no-load speed is expected to be lower than 600 RPM.

A rough approximation based purely on voltage would be:

```text
600 RPM × (5 V / 6 V) ≈ 500 RPM
```

This is only an estimate.

The actual RPM cannot be determined accurately from the PWM value alone.

For example, the following relationship should **not** be considered an accurate RPM measurement:

```text
PWM 255 → 500 RPM
PWM 128 → 250 RPM
PWM 64  → 125 RPM
```

These values are only theoretical approximations.

## Future RPM and speed measurement

A future version of the project could include an encoder or Hall-effect sensor to measure the actual motor speed.

For example:

```text
             ┌──────────────┐
             │   Arduino    │
             │     Nano     │
             └──────┬───────┘
                    │
             PWM    │    RPM feedback
                    │         ▲
                    ▼         │
                ┌───────┐     │
                │ MOSFET│     │
                └───┬───┘     │
                    │         │
                    ▼         │
                 ┌─────┐      │
                 │Motor│──────┘
                 └─────┘
                  Encoder
```

With an encoder, the system could display the actual RPM and calculate the equivalent linear speed in km/h if the motor is connected to a wheel or roller.

## Estimated speed in km/h

If the motor drives a wheel or roller, rotational speed can be converted into linear speed.

The basic formula is:

```text
Speed (km/h) = RPM × circumference (m) × 60 / 1000
```

where:

```text
circumference = π × diameter
```

For example, with a 6 cm diameter wheel:

```text
circumference = π × 0.06
              ≈ 0.188 m
```

At approximately 500 RPM:

```text
Speed ≈ 500 × 0.188 × 60 / 1000
      ≈ 5.6 km/h
```

This represents an ideal theoretical value and does not account for slip, load or losses.

## Limitations

### IRFZ44N

The IRFZ44N is a power MOSFET but is **not an ideal logic-level MOSFET** for direct control from a 5 V Arduino output.

It can work with small motors and relatively low currents, but it may not be fully enhanced at a 5 V gate voltage.

For a more efficient design, a logic-level MOSFET such as an **IRLZ44N** would be preferable.

### Motor speed

The current implementation does not measure actual RPM.

The potentiometer controls PWM duty cycle, not a specific motor speed.

### Power supply

The external power supply must be capable of supplying the motor's startup and operating current.

A motor can briefly draw considerably more current when starting or when mechanically loaded.

## Possible improvements

- [ ] Replace IRFZ44N with a logic-level MOSFET
- [ ] Add RPM measurement
- [ ] Add optical encoder
- [ ] Add Hall-effect sensor
- [ ] Display RPM on an OLED
- [ ] Display estimated speed in km/h
- [ ] Implement closed-loop speed control
- [ ] Add soft-start
- [ ] Add configurable minimum PWM
- [ ] Improve PWM frequency
- [ ] Add multiple motor control
- [ ] Add emergency stop

## Author

**Rafael Ramirez Salas**

Computer Engineering

## License

This project is licensed under the MIT License.

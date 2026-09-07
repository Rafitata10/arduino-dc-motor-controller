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

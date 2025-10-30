#include <Arduino.h>

const int ENA = 9;
const int ENB = 10;
const int IN1 = 5;
const int IN2 = 6;
const int IN3 = 7;
const int IN4 = 8;

#define TRIG_LEFT  4
#define ECHO_LEFT  1
#define TRIG_FRONT 3
#define ECHO_FRONT 0
#define TRIG_RIGHT 2
#define ECHO_RIGHT 20


const int frontThreshold = 25;
const int sideTooClose = 12;
const int maxSnelheid = 220;
const int turnDelay = 400; 
const int reverseDelay = 350;

const int freq = 5000;
const int resolution = 8;
const int pwmChannelA = 0;
const int pwmChannelB = 1;

long measureDistanceCM(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);
    if (duration == 0) return 999;
    return duration / 58;
}

void driveForward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(pwmChannelA, maxSnelheid);
    ledcWrite(pwmChannelB, maxSnelheid);
}

void driveReverse() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    ledcWrite(pwmChannelA, maxSnelheid);
    ledcWrite(pwmChannelB, maxSnelheid);
}

void rotateLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(pwmChannelA, maxSnelheid);
    ledcWrite(pwmChannelB, maxSnelheid);
}

void rotateRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    ledcWrite(pwmChannelA, maxSnelheid);
    ledcWrite(pwmChannelB, maxSnelheid);
}

void stopMotors() {
    ledcWrite(pwmChannelA, 0);
    ledcWrite(pwmChannelB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void setup() {
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);
    pinMode(TRIG_FRONT, OUTPUT);
    pinMode(ECHO_FRONT, INPUT);
    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);

    ledcSetup(pwmChannelA, freq, resolution);
    ledcAttachPin(ENA, pwmChannelA);

    ledcSetup(pwmChannelB, freq, resolution);
    ledcAttachPin(ENB, pwmChannelB);
    stopMotors();
}

void loop() {
    long dLeft  = measureDistanceCM(TRIG_LEFT, ECHO_LEFT);
    long dFront = measureDistanceCM(TRIG_FRONT, ECHO_FRONT);
    long dRight = measureDistanceCM(TRIG_RIGHT, ECHO_RIGHT);

    Serial.print("L="); Serial.print(dLeft);
    Serial.print(" F="); Serial.print(dFront);
    Serial.print(" R="); Serial.println(dRight);

   
    if (dFront < frontThreshold) {
        stopMotors();
        delay(100);

        if (dLeft < 15 && dRight < 15) {
            driveReverse();
            delay(reverseDelay);
            stopMotors();
            delay(100);
            return;
        }

        if (dLeft > dRight) {
            rotateLeft();
        } else {
            rotateRight();
        }
        delay(turnDelay);
        stopMotors();
        delay(100);
    }

    else if (dLeft < sideTooClose) {
        rotateRight();
        delay(200);
        stopMotors();
    }

    else if (dRight < sideTooClose) {
        rotateLeft();
        delay(200);
        stopMotors();
    }
    else {
        driveForward();
    }

    delay(40);
}

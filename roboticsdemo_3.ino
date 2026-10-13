/*
  Robot Demo 3 — Arduino Mega + PCA9685 controller

  Commands (newline-terminated):
    PING
    MOVE q1,q2,q3,q4,q5,q6
    GRIP OPEN
    GRIP CLOSE
    HOME

  IMPORTANT:
  - Not calibrated for arm yet, measure link lengths.
  - Servo angles sent by the browser are mathematical joint angles in degrees.
  - Joint channels 0-5; gripper channel 6 by default.
  - Test with servo power disconnected first, then one joint at a time.
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm(0x40);
const float SERVO_HZ = 50.0f;
const uint8_t N = 6;
const uint8_t GRIPPER_CHANNEL = 6;
const uint8_t CHANNEL[N] = {0, 1, 2, 3, 4, 5};
const int8_t DIRECTION[N] = {1, -1, 1, 1, -1, 1};
const float SERVO_ZERO[N] = {90, 90, 90, 90, 90, 90};
const float SERVO_MIN_DEG[N] = {10, 10, 10, 10, 10, 10};
const float SERVO_MAX_DEG[N] = {170, 170, 170, 170, 170, 170};
const uint16_t PULSE_MIN_US = 600;
const uint16_t PULSE_MAX_US = 2400;
const uint16_t GRIP_OPEN_US = 1500;
const uint16_t GRIP_CLOSED_US = 1900;
float currentQ[N] = {0, 0, 0, 0, 0, 0};
String inputLine;

uint16_t usToTicks(uint16_t us) {
  return (uint32_t)us * 4096UL * (uint32_t)SERVO_HZ / 1000000UL;
}
void servoUs(uint8_t ch, uint16_t us) {
  us = constrain(us, 500, 2500);
  pwm.setPWM(ch, 0, usToTicks(us));
}
void setJoint(uint8_t j, float q) {
  float s = SERVO_ZERO[j] + DIRECTION[j] * q;
  s = constrain(s, SERVO_MIN_DEG[j], SERVO_MAX_DEG[j]);
  uint16_t pulse = (uint16_t)(PULSE_MIN_US + (s / 180.0f) * (PULSE_MAX_US - PULSE_MIN_US));
  servoUs(CHANNEL[j], pulse);
  currentQ[j] = q;
}
void moveJoints(float target[]) {
  const int STEPS = 40;
  float start[N];
  for (uint8_t i=0;i<N;i++) start[i] = currentQ[i];
  for (int s=1;s<=STEPS;s++) {
    float u = (float)s / STEPS;
    for (uint8_t i=0;i<N;i++) setJoint(i, start[i] + u*(target[i]-start[i]));
    delay(20);
  }
  Serial.println(F("OK MOVE"));
}
void grip(bool closeIt) {
  servoUs(GRIPPER_CHANNEL, closeIt ? GRIP_CLOSED_US : GRIP_OPEN_US);
  Serial.println(F("OK GRIP"));
}
void handle(String line) {
  line.trim();
  if (line == "PING") {
    Serial.println(F("PONG"));
  } else if (line.startsWith("MOVE ")) {
    String rest = line.substring(5);
    float q[N];
    for (uint8_t i=0;i<N;i++) {
      int comma = rest.indexOf(',');
      String token;
      if (i < N-1) {
        if (comma < 0) { Serial.println(F("ERR NEED_6_ANGLES")); return; }
        token = rest.substring(0, comma);
        rest = rest.substring(comma+1);
      } else token = rest;
      if (token.length() == 0) { Serial.println(F("ERR BAD_ANGLE")); return; }
      q[i] = token.toFloat();
      if (q[i] < -180 || q[i] > 180) { Serial.println(F("ERR ANGLE_RANGE")); return; }
    }
    moveJoints(q);
  } else if (line == "GRIP OPEN") {
    grip(false);
  } else if (line == "GRIP CLOSE") {
    grip(true);
  } else if (line == "HOME") {
    float q[N] = {0,0,0,0,0,0};
    moveJoints(q);
  } else if (line.length()) {
    Serial.println(F("ERR UNKNOWN_COMMAND"));
  }
}
void setup() {
  Serial.begin(115200);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(SERVO_HZ);
  delay(10);
  Serial.println(F("Robot Demo 3 ready"));
}
void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') { handle(inputLine); inputLine = ""; }
    else if (c != '\r') {
      if (inputLine.length() < 180) inputLine += c;
      else { inputLine = ""; Serial.println(F("ERR LINE_TOO_LONG")); }
    }
  }
}

#include<main.h>

long leftMotorSpeed = 0;
long rightMotorSpeed = 0;

unsigned long lastStepTimeLeft = 0;
unsigned long lastStepTimeRight = 0;

bool leftMotorStatus = false;
bool rightMotorStatus = false;

bool received_confirmation = false;

void stepMotor(int stepPin, unsigned long speed, unsigned long &lastStepTime, bool &status) {
  if (speed) {
    unsigned long currentTime = micros();

    if (currentTime - lastStepTime >= speed) {
      status = !status;

      digitalWrite(stepPin, status);
      lastStepTime = currentTime;
    }
  } else {
    digitalWrite(stepPin, LOW);
  }
}

void on_receive(const int size) {
  if (size >= 4) {
    byte highByte = Wire.read();
    byte lowByte = Wire.read();

    leftMotorSpeed = (highByte << 8) | lowByte;
    digitalWrite(LEFT_DIR_PIN, leftMotorSpeed >= 0);
    leftMotorSpeed = abs(leftMotorSpeed);

    highByte = Wire.read();
    lowByte = Wire.read();

    rightMotorSpeed = (highByte << 8) | lowByte;
    digitalWrite(RIGHT_DIR_PIN, rightMotorSpeed >= 0);
    rightMotorSpeed = abs(rightMotorSpeed);
  }
}

void no_request() {}

void send_up() {
    if (received_confirmation) {
        Wire.write(UP_MESSAGE);

        Wire.onRequest(no_request);
        Wire.onReceive(on_receive);
    }
}

void await_confirmation(const int size) {
    while (Wire.available()) {        
        if (Wire.read() == UP_CHAR) {
            received_confirmation = true;
        }
    }
}

void setup() {
  pinMode(LEFT_STEP_PIN, OUTPUT);
  pinMode(LEFT_DIR_PIN, OUTPUT);
  pinMode(LEFT_ENABLE_PIN, OUTPUT);
  pinMode(RIGHT_STEP_PIN, OUTPUT);
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  pinMode(RIGHT_ENABLE_PIN, OUTPUT);

  digitalWrite(LEFT_ENABLE_PIN, LOW);
  digitalWrite(RIGHT_ENABLE_PIN, LOW);

  digitalWrite(LEFT_DIR_PIN, HIGH);
  digitalWrite(RIGHT_DIR_PIN, HIGH);

  Wire.begin(I2C_ADDRESS);

  Wire.onRequest(send_up);
  Wire.onReceive(await_confirmation);

  while (!received_confirmation) {
      delay(10);
  }
}

void loop() {
  if (Serial.available() >= 2 * sizeof(int)) {
    leftMotorSpeed = Serial.parseInt();
    rightMotorSpeed = Serial.parseInt();

    digitalWrite(LEFT_DIR_PIN, leftMotorSpeed > 0);
    digitalWrite(RIGHT_DIR_PIN, rightMotorSpeed > 0);
  }

  stepMotor(LEFT_STEP_PIN, leftMotorSpeed, lastStepTimeLeft, leftMotorStatus);
  stepMotor(RIGHT_STEP_PIN, rightMotorSpeed, lastStepTimeRight, rightMotorStatus);
}
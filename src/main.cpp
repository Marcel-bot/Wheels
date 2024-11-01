#include<main.h>

long leftMotorSpeed = 0;
long rightMotorSpeed = 0;

unsigned long lastStepTimeLeft = 0;
unsigned long lastStepTimeRight = 0;

bool leftMotorStatus = false;
bool rightMotorStatus = false;

bool received_confirmation = false;

String buffer;

void stepMotor(const int stepPin, const unsigned long speed, unsigned long &lastStepTime, bool &status) {
    if (speed) {
        const unsigned long currentTime = micros();

        if (currentTime - lastStepTime >= speed) {
            status = !status;

            digitalWrite(stepPin, status);
            lastStepTime = currentTime;
        }
    } else {
        digitalWrite(stepPin, LOW);
    }
}

void setup() {
    Serial.begin(115200);

    delay(500);

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

    Serial.println("Booted");
}

void loop() {
    if (Serial.available()) {
        char c = Serial.read();

        if (c == '\n') {
            const int spaceIndex = buffer.indexOf(' ');

            leftMotorSpeed = buffer.substring(0, spaceIndex).toInt();
            rightMotorSpeed = buffer.substring(spaceIndex + 1).toInt();

            digitalWrite(LEFT_DIR_PIN, leftMotorSpeed > 0);
            digitalWrite(RIGHT_DIR_PIN, rightMotorSpeed > 0);

            leftMotorSpeed = abs(leftMotorSpeed);
            rightMotorSpeed = abs(rightMotorSpeed);

            buffer = "";
        } else {
            buffer += c;
        }

        while (Serial.available()) {
            Serial.read();
        }
    }

    stepMotor(LEFT_STEP_PIN, leftMotorSpeed, lastStepTimeLeft, leftMotorStatus);
    stepMotor(RIGHT_STEP_PIN, rightMotorSpeed, lastStepTimeRight, rightMotorStatus);
}

#ifndef Damper_h
#define Damper_h

class Damper {
    SmoothServo servo;
    int minAngle;
    int maxAngle;
    float currentPosition;

    
  public:
    Damper(SmoothServo s, int minAng, int maxAng) :
      servo(s), minAngle(minAng), maxAngle(maxAng) {
    }

    void setPosition(float position) {
      currentPosition = position;
      int angle = position * (abs(minAngle) + abs(maxAngle)) + minAngle;
    
      servo.setAngle(angle);
    }

    float getPosition() {
      return currentPosition;
    }

    void printState() {
        Serial.print("[DAMPER POSITION] -> ");
        Serial.print(getPosition());
        Serial.println();
    }

};

#endif

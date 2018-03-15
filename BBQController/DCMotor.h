#ifndef DCMotor_h
#define DCMotor_h

class DCMotor {
    const byte pin;

    unsigned long lastTime = 0;
    boolean isHigh = false;
    
  public:
    float motorPower = 0.0;

    DCMotor(byte attachToPin) :
      pin(attachToPin) {
    }

    void setPower(float power) {
        motorPower = power;
        if (motorPower < 0.0)
          motorPower = 0.0;
        if (motorPower > 1.0)
          motorPower = 1.0;
    }

    float getPower() {
      return motorPower;
    }

    void printState() {
        Serial.print("[MOTOR POWER] -> ");
        Serial.print(getPower());
        Serial.println();
    }

    void setup() {
      pinMode(pin, OUTPUT);
    }

    void loop() {

      if (motorPower == 0.0) {
          digitalWrite(pin, LOW);
      } 
      else if (motorPower == 1.0) {
          digitalWrite(pin, HIGH);
      }
      else {    
        int t1 = 100 * motorPower;
        int t2 = 100 - t1;
  
        if (isHigh && (millis() - lastTime >= t1)) {
            digitalWrite(pin, LOW);
            isHigh = false;
            lastTime = millis();
        }
        else if (!isHigh && (millis() - lastTime >= t2)) {
            digitalWrite(pin, HIGH);
            isHigh = true;
            lastTime = millis();
        }
      }
  }
};

#endif


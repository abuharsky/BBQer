#ifndef SmoothServo_h
#define SmoothServo_h

class SmoothServo {
    Servo &servo;
    const int pin;

    
  public:
    SmoothServo(Servo &s, int pin) :
      servo(s), pin(pin) {
    }


    void setAngle(int angle) {
      servo.attach(pin);

      int previousAngle = servo.read();

      if (angle != previousAngle) {
        if(angle>previousAngle)
        {
          for(int i=previousAngle; i<=angle; i++)
          {
            servo.write(i); //turn servo by 1 degrees
            delay(15);        //delay for smoothness
          }
        }
      
        if(angle<previousAngle)
        {
          for(int i=previousAngle; i>=angle; i--)
          {
            servo.write(i); //turn servo by 1 degrees
            delay(15);        //delay for smoothness
          }
        }
  
        delay(500);
      }
      
      servo.detach();
    }

    int getAngle() {
      return servo.read();
    }

    void printState() {
        Serial.print("[SERVO ANGLE] -> ");
        Serial.print(getAngle());
        Serial.println();
    
    }
};

#endif

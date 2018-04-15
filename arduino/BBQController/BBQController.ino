#include <SerialCommand.h>
#include "max6675.h"
#include <Servo.h>
#include "DCMotor.h"
#include "SmoothServo.h"
#include "Damper.h"
#include "ThermoProbe.h"
#include "TemperatureController.h"

#define SERVO_PIN A0
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 90

#define MOTOR_PIN 11

#define PROBE0_PIN A4
#define PROBE1_PIN A7

#define THERMO_S0  2
#define THERMO_CS  3
#define THERMO_CLK 4


Servo servo;
SmoothServo smoothServo(servo, SERVO_PIN);
DCMotor motor(MOTOR_PIN);
Damper damper(smoothServo, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_S0);
ThermoProbe probe0(PROBE0_PIN);
ThermoProbe probe1(PROBE1_PIN);

TemperatureController controller(damper, motor, thermocouple, probe0, probe1);

SerialCommand sCmd;

void setup()
{
  Serial.begin(115200);

  motor.setup();
  controller.setup();
  probe0.setup();

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("servo", servoControl);
  sCmd.addCommand("damper", damperControl);
  sCmd.addCommand("motor", motorControl);
  sCmd.addCommand("temp", getTemperature);
  sCmd.addCommand("target", setTargetTemperature);
  sCmd.addCommand("controller", setControllerParameters);
  sCmd.setDefaultHandler(unrecognized);

  Serial.println("Ready");
}

void loop()
{
  motor.loop();
  controller.loop();
  probe0.loop();
  probe1.loop();
  sCmd.readSerial();
}

//---------------------------------------------

void servoControl() {
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    smoothServo.setAngle(atoi(arg)); 
    smoothServo.printState();
  }


}

void damperControl() {
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    damper.setPosition(atof(arg)); 
    damper.printState();
 }


}

void motorControl() {
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    motor.setPower(atof(arg));
    motor.printState();
  }

}

void setTargetTemperature() {
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    controller.setTargetTemperature(atof(arg));
  }

  arg = sCmd.next();
  if (arg != NULL) {
    controller.setPrintStateInterval(atof(arg)*1000);
  }
}

void setControllerParameters() {
  char *printInterval   = sCmd.next();
  char *airFlowMin      = sCmd.next();
  char *airFlowMax      = sCmd.next();
  char *airFlowStepUp   = sCmd.next();
  char *airFlowStepDown = sCmd.next();
  
  if (printInterval != NULL && 
      airFlowMin != NULL && airFlowMax != NULL && 
      airFlowStepUp != NULL && airFlowStepDown != NULL) 
  {
    controller.setPrintStateInterval(atof(printInterval)*1000);
    controller.setAirflowLimitsAfterTempReached(atof(airFlowMin), atof(airFlowMax));
    controller.setAirflowChangeSteps(atof(airFlowStepUp), atof(airFlowStepDown));

    controller.printParameters();
  }
  else {
    Serial.println("Wrong 'controller' command arguments, must be:");
    Serial.println("> controller printInterval(seconds) airFlowMinWhenTempReached(float) airFlowMaxWhenTempReached(float) airFlowStepUp(float) airFlowStepDown(float)");
    Serial.println("Example:");
    Serial.println("> controller 10 0.3 0.7 0.05 0.15");
  }

}

void getTemperature() {
  Serial.print("[TEMP] PIT=");
  Serial.print(thermocouple.readCelsius());
  Serial.print("C, PROBE1=");
  Serial.print(probe0.readCelsius());
  Serial.print("C, PROBE2=");
  Serial.print(probe1.readCelsius());
  Serial.print("C");
  Serial.println();
}

void unrecognized(const char *command) {
  Serial.println("Unrecognized command");
}


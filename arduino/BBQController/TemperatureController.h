#ifndef TemperatureController_h
#define TemperatureController_h

#include <EEPROM.h>
#include "DCMotor.h"
#include "SmoothServo.h"
#include "Damper.h"
#include "ThermoProbe.h"

class TemperatureController {
    Damper &damper;
    ThermoProbe &probe0;
    ThermoProbe &probe1;
    DCMotor &motor;
    MAX6675 &thermocouple;

    float targetTemperature = 0.00f;

    unsigned long lastPrintStateMs = 0;
    unsigned long printStateInterval = 10000;

    unsigned long lastMs = 0;

    unsigned long stepMs = 1 * 1000; // 1 sec
    
    float airFlowLevel = 0.0;

    float minAirFlowLevelAfterTempReached = 0.0; // decrease airflow limits when 
    float maxAirFlowLevelAfterTempReached = 1.0; // temperature has reached target first time
    boolean isTemperatureReachedFirstTime = false;

    float airFlowUpStep   = 0.05; // percent
    float airFlowDownStep = 0.05; // percent

  public:
    TemperatureController(Damper &damper, DCMotor &motor, MAX6675 &thermocouple, ThermoProbe &probe0, ThermoProbe &probe1) :
      damper(damper), motor(motor), thermocouple(thermocouple), probe0(probe0), probe1(probe1) {
    }

    void setup() {
      
      EEPROM.get(0, targetTemperature); 
      if (targetTemperature < 0 || targetTemperature > 300) {
        setTargetTemperature(0);
      }

      EEPROM.get(sizeof(float), printStateInterval); 
      if (printStateInterval < 3000) { // min 3 sec
        setPrintStateInterval(3000);
      }

      EEPROM.get(sizeof(float)*2, minAirFlowLevelAfterTempReached); 
      if (minAirFlowLevelAfterTempReached < 0.0 || minAirFlowLevelAfterTempReached > 1.0) {
        minAirFlowLevelAfterTempReached = 0.0;
      }

      EEPROM.get(sizeof(float)*3, maxAirFlowLevelAfterTempReached); 
      if (maxAirFlowLevelAfterTempReached > 1.0 || maxAirFlowLevelAfterTempReached < 0.0) {
        maxAirFlowLevelAfterTempReached = 1.0;
      }

      EEPROM.get(sizeof(float)*4, airFlowUpStep); 
      if (airFlowUpStep <= 0.0 || airFlowUpStep > 1.0) {
        airFlowUpStep = 0.05;
      }

      EEPROM.get(sizeof(float)*5, airFlowDownStep); 
      if (airFlowDownStep <= 0 || airFlowDownStep > 1.0) {
        airFlowDownStep = 0.05;
      }
    }

    void loop() {

      if (targetTemperature != 0.0) {
          unsigned long ms = millis();
          if (ms - lastMs > stepMs) {
            lastMs = ms;
    
            float temp = thermocouple.readCelsius();
    
            if (temp < targetTemperature) {
              setAirFlowLevel(airFlowLevel + airFlowUpStep);
            }
            else {
              isTemperatureReachedFirstTime = true;
              setAirFlowLevel(airFlowLevel - airFlowDownStep);
            }
          }

          printCurrentStateIfNeeded();
      }
    }

    
    void setAirFlowLevel(float level) {

      float minAirFlowLevel = 0.0;
      float maxAirFlowLevel = 1.0;

      if (isTemperatureReachedFirstTime) {
        minAirFlowLevel = minAirFlowLevelAfterTempReached;
        maxAirFlowLevel = maxAirFlowLevelAfterTempReached;
      }

      
      if (level > maxAirFlowLevel) {
        level = maxAirFlowLevel;
      }
      
      if (level < minAirFlowLevel) {
        level = minAirFlowLevel;
      }

      airFlowLevel = level;
            
      float maxDamperPercent = 0.2;
      if (level <= maxDamperPercent) {
        damper.setPosition(level/maxDamperPercent);
        motor.setPower(0);
      }
      else {
        damper.setPosition(1);
        float power = (level - maxDamperPercent) / (1.0 - maxDamperPercent);
        motor.setPower(power);
      }
    }
    
    void setTargetTemperature(float targetTemp) {
      isTemperatureReachedFirstTime = false;
      
      targetTemperature = targetTemp;
      EEPROM.put(0, targetTemperature); 

      if (targetTemperature == 0) {

        setAirFlowLevel(0);
        
        Serial.println("[CONTROLLER] off");
        printCurrentState();

        return;
      }

    }

    void setAirflowLimitsAfterTempReached(float minAirFlow, float maxAirFlow) {
      minAirFlowLevelAfterTempReached = minAirFlow;
      EEPROM.put(sizeof(float)*2, minAirFlowLevelAfterTempReached); 

      maxAirFlowLevelAfterTempReached = maxAirFlow;
      EEPROM.put(sizeof(float)*3, maxAirFlowLevelAfterTempReached); 
    }

    void setAirflowChangeSteps(float stepUp, float stepDown) {
      airFlowUpStep = stepUp;
      EEPROM.put(sizeof(float)*4, airFlowUpStep); 

      airFlowDownStep = stepDown;
      EEPROM.put(sizeof(float)*5, airFlowDownStep); 
    }

    void setPrintStateInterval(float interval) {
      printStateInterval = interval;
      EEPROM.put(sizeof(float), printStateInterval); 
    }
  
    void printCurrentStateIfNeeded() {      
      if (millis() - lastPrintStateMs > printStateInterval) {
        lastPrintStateMs = millis();
        printCurrentState();
      }
    }

    void printParameters() {
        Serial.println("[CONTROLLER]");
        Serial.print("> printInterval(seconds) = "); 
          Serial.println(printStateInterval/1000);
        Serial.print("> airflow(when target temp reached): min = "); 
          Serial.print(minAirFlowLevelAfterTempReached);
          Serial.print(", max = "); 
          Serial.println(maxAirFlowLevelAfterTempReached);
        Serial.print("> airflow step: up = "); 
          Serial.print(airFlowUpStep);
          Serial.print(", down = "); 
          Serial.println(airFlowDownStep);
    }

    void printCurrentState() {
      
      float pitTemp = thermocouple.readCelsius();
      if (isnan(pitTemp)) {
        pitTemp = 0;
      }
      
      Serial.print("{");
          Serial.print("\"airflow\": {");
              Serial.print("\"level\": \"");    Serial.print(airFlowLevel);               Serial.print("\",");
              Serial.print("\"damper\": \"");   Serial.print(damper.getPosition());       Serial.print("\",");
              Serial.print("\"motor\": \"");    Serial.print(motor.getPower());           Serial.print("\"");
          Serial.print("},");
          Serial.print("\"temperature\": {");
              Serial.print("\"current\": \"");  Serial.print(pitTemp);                    Serial.print("\",");
              Serial.print("\"probe0\": \"");   Serial.print(probe0.readCelsius());       Serial.print("\",");
              Serial.print("\"probe1\": \"");   Serial.print(probe1.readCelsius());       Serial.print("\",");
              Serial.print("\"target\": \"");   Serial.print(targetTemperature);          Serial.print("\"");
          Serial.print("}");
      Serial.print("}");

      Serial.println();
    }
};

#endif

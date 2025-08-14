#include <dht.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <AccelStepper.h>
#include <PID_v1_bc.h>

#define MODULATION_RATE 9600
#define DHT_PIN 7
#define LED_PIN 8
#define STEPPER_PIN1 4
#define STEPPER_PIN2 5
#define STEPPER_PIN3 6
#define STEPPER_PIN4 9

dht DHT;
SoftwareSerial BT(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);
AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_PIN1, STEPPER_PIN3, STEPPER_PIN2, STEPPER_PIN4);

double tempSetpoint = 24.0;
double humSetpoint = 56.0;
double inputVal, outputFan;
PID fanPID(&inputVal, &outputFan, &tempSetpoint, 2.0, 5.0, 1.0, P_ON_E, DIRECT);

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(MODULATION_RATE);
  BT.begin(MODULATION_RATE);
  pinMode(LED_PIN, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(2000);
  fanPID.SetMode(AUTOMATIC);
  fanPID.SetOutputLimits(500, 4000);
}

void loop() {
  if (BT.available()) {
    char cmd = BT.read();
    digitalWrite(LED_PIN, cmd == '1' ? HIGH : LOW);
    Serial.println(cmd == '1' ? "LED On" : "LED Off");
  }

  if (DHT.read11(DHT_PIN) == DHTLIB_OK) {
    float temp = DHT.temperature;
    float hum = DHT.humidity;
    double tempError = abs(temp - tempSetpoint);
    double humError = abs(hum - humSetpoint);
    inputVal = (humError > tempError) ? hum : temp;
    tempSetpoint = (humError > tempError) ? humSetpoint : 24.0;
    fanPID.Compute();
    stepper.setSpeed(outputFan);
  }

  stepper.runSpeed();

  if (millis() - lastUpdate > 100) {
    lastUpdate = millis();
    BT.println("{\"temperature\":" + String(DHT.temperature, 2) + ",\"humidity\":" + String(DHT.humidity, 2) + ",\"fanSpeed\":" + String(outputFan, 0) + "}");
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(DHT.temperature, 1) + "C H:" + String(DHT.humidity, 0) + "%   ");
    lcd.setCursor(0, 1);
    lcd.print("FAN:" + String(outputFan, 0) + " SPD   ");
  }
}

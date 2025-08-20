#include <dht.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PID_v1_bc.h>

#define MODULATION_RATE 9600
#define DHT_PIN 7
#define LED_PIN 8
#define MOTOR_PIN_IN1 A1
#define MOTOR_PIN_IN2 A2
#define MOTOR_PIN_EN A0

dht DHT;
SoftwareSerial BT(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

double tempSetpoint = 24.0;
double inputVal, outputMotor;

PID motorPID(&inputVal, &outputMotor, &tempSetpoint, 2.0, 5.0, 1.0, DIRECT);

unsigned long lastUpdate = 0;
unsigned long lastDHT = 0;

void setup() {
  Serial.begin(MODULATION_RATE);
  BT.begin(MODULATION_RATE);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_PIN_IN1, OUTPUT);
  pinMode(MOTOR_PIN_IN2, OUTPUT);
  pinMode(MOTOR_PIN_EN, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
  motorPID.SetMode(AUTOMATIC);
  motorPID.SetOutputLimits(0, 255);
}

void loop() {
  if (BT.available()) {
    char cmd = BT.read();
    digitalWrite(LED_PIN, cmd == '1' ? HIGH : LOW);
    Serial.println(cmd == '1' ? "LED On" : "LED Off");
  }
  if (millis() - lastDHT > 1000) {
    lastDHT = millis();
    if (DHT.read11(DHT_PIN) == DHTLIB_OK) {
      inputVal = DHT.temperature;
      motorPID.Compute();
      digitalWrite(MOTOR_PIN_IN1, HIGH);
      digitalWrite(MOTOR_PIN_IN2, LOW);
      analogWrite(MOTOR_PIN_EN, (int)outputMotor);
    }
  }
  if (millis() - lastUpdate > 100) {
    lastUpdate = millis();
    BT.println("{\"temperature\":" + String(DHT.temperature, 2) + ",\"humidity\":" + String(DHT.humidity, 2) + ",\"motorSpeed\":" + String(outputMotor, 0) + "}");
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(DHT.temperature, 1) + "C H:" + String(DHT.humidity, 0) + "%   ");
    lcd.setCursor(0, 1);
    lcd.print("Motor:" + String(outputMotor, 0) + " SPD   ");
  }
}

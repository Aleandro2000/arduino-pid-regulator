#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PID_v1_bc.h>
#include <SoftwareSerial.h>

#define MODULATION_RATE 9600
#define DHT_PIN 7
#define LED_PIN 8
#define MOTOR_PIN_IN1 A1
#define MOTOR_PIN_IN2 A2
#define MOTOR_PIN_EN A0

DHT dht(DHT_PIN, DHT11);

SoftwareSerial bluetooth(2, 3);

LiquidCrystal_I2C lcd(0x27, 16, 2);

double tempSetpoint = 24.0;
double humSetpoint = 50.0;

double tempInput, tempOutput;
double humInput, humOutput;
double outputMotor;

PID tempPID(&tempInput, &tempOutput, &tempSetpoint, 2.0, 5.0, 1.0, DIRECT);
PID humPID(&humInput, &humOutput, &humSetpoint, 1.5, 4.0, 1.0, DIRECT);

unsigned long lastUpdate = 0;
unsigned long lastDHT = 0;

void setup() {
  Serial.begin(MODULATION_RATE);
  bluetooth.begin(MODULATION_RATE);

  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_PIN_IN1, OUTPUT);
  pinMode(MOTOR_PIN_IN2, OUTPUT);
  pinMode(MOTOR_PIN_EN, OUTPUT);

  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();

  tempPID.SetMode(AUTOMATIC);
  tempPID.SetOutputLimits(0, 255);

  humPID.SetMode(AUTOMATIC);
  humPID.SetOutputLimits(0, 255);
}

void loop() {
  digitalWrite(LED_PIN, Serial.available() ? HIGH : LOW);

  if (millis() - lastDHT > 1000) {
    lastDHT = millis();

    double temp = dht.readTemperature();
    double hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      tempInput = temp;
      humInput = hum;

      tempPID.Compute();
      humPID.Compute();

      outputMotor = max(tempOutput, humOutput);

      digitalWrite(MOTOR_PIN_IN1, HIGH);
      digitalWrite(MOTOR_PIN_IN2, LOW);
      analogWrite(MOTOR_PIN_EN, outputMotor);

      String data = "{\"temperature\":";
      data += String(temp, 2);
      data += ",\"humidity\":";
      data += String(hum, 2);
      data += ",\"motorSpeed\":";
      data += String(outputMotor, 0);
      data += "}";

      Serial.println(data);
      bluetooth.println(data);
    }
  }

  if (millis() - lastUpdate > 100) {
    lastUpdate = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    lcd.setCursor(0, 0);
    lcd.print("T:" + String(temp, 1) + "C H:" + String(hum, 0) + "%   ");
    lcd.setCursor(0, 1);
    lcd.print("Motor:" + String(outputMotor, 0) + " SPD   ");
  }
}

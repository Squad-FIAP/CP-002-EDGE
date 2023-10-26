#include <LiquidCrystal_I2C.h>

const int RED_LED_PIN = 7;
const int YELLOW_LED_PIN = 4;
const int GREEN_LED_PIN = 2;
const int LDR_PIN = A0;
const int HUMIDITY_PIN = A3;
const int TEMPERATURE_PIN = A2;
const int BUZZER_PIN = 8;

double Voltage = 0;
double tempC = 0;
int humiditysensorOutput = 0;


LiquidCrystal_I2C lcd(0x20,16,2);

void setupLCD() {
  lcd.init();
  lcd.backlight();
  lcd.print("Vinheria Agnello");
  lcd.setCursor(0, 1);
  lcd.print("CP02 - EDGE");
}

void readDHT() {
  Voltage = (analogRead(TEMPERATURE_PIN) / 1023.0) * 5000; // 5000 to get millivots.
  tempC = (Voltage-500) * 0.1; // 500 is the offset
  humiditysensorOutput = map(analogRead(HUMIDITY_PIN), 0, 1023, 10, 70);
}

void printTempLCD(int temp) {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (temp < 10){
    lcd.print("TEMPERATURA BAIXA");
  } else if(temp > 15) {
    lcd.print("TEMPERATURA ALTA");
  } else {
    return;
  }
    lcd.setCursor(0, 1);
    lcd.print(String(temp) + String(" Celsius"));
    return;
}

void setup() {
  setupLCD();
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);  
  pinMode(LDR_PIN, INPUT);
  pinMode(A3, INPUT);
  Serial.begin(9600);
}

void turnOffAllLeds() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void buzz() {
  tone(8, 1000, 3000);
  delay(3000);
  noTone(8);
}

void execScenario(int scenario []){
    int luminosity = scenario[0];
    int humidity = scenario[1];
    int temperature = scenario[2];
    
    if (luminosity <= 3){
        turnOffAllLeds();
        digitalWrite(GREEN_LED_PIN, HIGH);
    } else if (luminosity > 3 && luminosity < 7){
        turnOffAllLeds();
        digitalWrite(YELLOW_LED_PIN, HIGH);
    } else if (luminosity >= 7) {
        turnOffAllLeds();
        digitalWrite(RED_LED_PIN, HIGH);
      	printTempLCD(0);
        buzz();
    }
}

void calculateStatusOfWineyard() {
  int count = 0;
  int luminositySum = 0;
  int humiditySum = 0;
  int temperatureSum = 0;

  while (count < 5){
    luminositySum += analogRead(LDR_PIN);
    humiditySum += analogRead(HUMIDITY_PIN);
    temperatureSum += analogRead(TEMPERATURE_PIN);    
    count++;
  }

  int luminosityAverage = luminositySum/count;
  int humidityAverage = humiditySum/count;
  int temperatureAverage = temperatureSum/count;

  int scenario [] = {(map(luminosityAverage, 160, 600, 1, 8)), humidityAverage, temperatureAverage};
  execScenario(scenario);
}

void loop() {
  readDHT();
  calculateStatusOfWineyard();
  delay(1000);
}

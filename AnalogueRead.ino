#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "button.h"
#include "led.h"

#include <math.h>

#define DHTTYPE DHT11
#define DHTPIN D2

#define PRINT_BUFFER_LEN 1024
#define debug Serial

// 12 bits of ADC Resolution
#define ADC_BITS 12

#define VCC 5.0
#define ln(x) log(x)
#define ADC_V_MAX 3.3
#define KELVIN_OFFSET 273.15

typedef struct {
  const char* name;
  const float rDivider;
  const double A;
  const double B;
  const double C;
  const float R25;
  const float Beta;
} coefficients;

// https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
const coefficients coefs[] = {
  coefficients{
    // T=100, R=   993
    // T= 34, R= 7,040
    // T=  3, R=26,350
    .name = "10K",
    .rDivider = 10000.0,
    .A = 5.507359522044491E-4,
    .B = 3.134967793642773E-4,
    .C = -1.0408993927588221E-7,
    .R25 = 9834.34,
    .Beta = 3401.29,
  },
  coefficients{
    // T=100, R=  6,640
    // T= 34, R= 66,200
    // T=  3, R=300,000
    .name = "100K",
    .rDivider = 103000.0,
    .A = 3.41795556518205E-4,
    .B = 2.7101265907443527E-4,
    .C = -6.902798113422121E-8,
    .R25 = 98016.79,
    .Beta = 3993.34,
  },
};


const float ADC_MAX = (float)(std::pow(2.0, (double)ADC_BITS) - 1.0);

int counter = 0;

char printBuffer[PRINT_BUFFER_LEN] = {};

void printRes(const int pin, const int adcVal);
void readDht();

DHT dht(DHTPIN, DHTTYPE);


Button* button1;
Button* button2;
Led* led1;
Led* ledBuiltin;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  analogReadResolution(ADC_BITS);
  dht.begin();
  button1 = new Button("B1", D7, HIGH, 10);
  button2 = new Button("B2", D8, HIGH, 10);
  ledBuiltin = new Led("Builtin", LED_BUILTIN, LOW);
  led1 = new Led("L1", D9, HIGH);
  ledBuiltin->off();
  led1->on();
 }

void loop() {
  memset(printBuffer, 0, PRINT_BUFFER_LEN);
  sprintf(printBuffer, "Sample: %d", ++counter);
  Serial.println(printBuffer);

  ledBuiltin->toggle();
  led1->toggle();

  printRes(0, analogRead(A0));
  printRes(1, analogRead(A1));
  readDht();
  printButtonState(button1);
  printButtonState(button2);

  Serial.println();

  delay(1000);
}

void printButtonState(Button* b){
  Serial.print("Button ");
  Serial.print(b->name());
  Serial.print(": ");
  Serial.println(b->pressed()?"PRESSED":"NOT PRESSED");
}

void printRes(const int pin, const int adcVal) {
  float V = (float)adcVal / ADC_MAX * ADC_V_MAX;
  float i = (VCC - V) / coefs[pin].rDivider;
  float R = V / i;
  float T = 1.0 / (coefs[pin].A + coefs[pin].B * ln(R) + coefs[pin].C * std::pow(ln(R), 3.0)) - KELVIN_OFFSET;

  float BetaTemp = coefs[pin].Beta / (ln(R / (coefs[pin].R25 * std::exp(-1.0 * (coefs[pin].Beta / (25 + KELVIN_OFFSET)))))) - KELVIN_OFFSET;


  memset(printBuffer, 0, sizeof(printBuffer));
  sprintf(printBuffer, "%s: adcVal=%d, ADC_MAX=%.1f, VCC=%.1f, V=%.1f, i=%f, R=%.3f, T=%.2f, BetaTemp=%.2f", coefs[pin].name, adcVal, ADC_MAX, VCC, V, i, R, T, BetaTemp);
  Serial.println(printBuffer);
}

void readDht() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}


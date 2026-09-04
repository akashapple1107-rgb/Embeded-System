#include <Wire.h>
#include <BH1750.h>

// pin definitions
const int PIR_PIN = 2;
const int BUTTON_PIN = 3;

const int LED1_PIN = 6;
const int LED2_PIN = 7;

// BH1750 light sensor
BH1750 lightMeter;

// darkness threshold
const float DARK_THRESHOLD = 50.0;

// LED time
const unsigned long LED1_TIME = 30000;  // 30 seconds
const unsigned long LED2_TIME = 60000;  // 60 seconds

unsigned long led1StartTime = 0;
unsigned long led2StartTime = 0;

// flags
volatile bool pirTriggered = false;
volatile bool buttonTriggered = false;

// LED states
bool led1Active = false;
bool led2Active = false;

// Button debounce
volatile unsigned long lastButtonInterrupt = 0;
const unsigned long DEBOUNCE_TIME = 200;

// PIR interrupt

void pirInterrupt() {
  pirTriggered = true;
}

// Button interrupt
void buttonInterrupt() {

  unsigned long currentTime = millis();

  if (currentTime - lastButtonInterrupt > DEBOUNCE_TIME) {
    buttonTriggered = true;
    lastButtonInterrupt = currentTime;
  }
}

// Setup
void setup() {

  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  
  Wire.begin();

  if (lightMeter.begin()) {
    Serial.println("BH1750 sensor started.");
  }
  else {
    Serial.println("BH1750 sensor not detected.");
  }

  attachInterrupt(
    digitalPinToInterrupt(PIR_PIN),
    pirInterrupt,
    RISING
  );

  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonInterrupt,
    FALLING
  );

  Serial.println("Interrupt lighting system ready.");
}



// Main loop
void loop() {

  //PIR evnet
  if (pirTriggered) {

    noInterrupts();
    pirTriggered = false;
    interrupts();

    // Read actual light level
    float lux = lightMeter.readLightLevel();

    Serial.println();
    Serial.println("Movement detected near the entrance.");

    if (lux < DARK_THRESHOLD) {

      Serial.println("Surrounding light level is low.");
      Serial.println("Automatic lighting activated.");

      // Turn both lights ON
      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, HIGH);

      led1Active = true;
      led2Active = true;

      unsigned long currentTime = millis();

      led1StartTime = currentTime;
      led2StartTime = currentTime;

      Serial.println("Main light switched ON.");
      Serial.println("Secondary light switched ON.");
    }

    else {

      Serial.println("Surrounding light level is sufficient.");
      Serial.println("Automatic lighting not required.");
    }
  }

  // Button event
    if (buttonTriggered) {

    noInterrupts();
    buttonTriggered = false;
    interrupts();

    Serial.println();
    Serial.println("Manual button input received.");
    Serial.println("Lighting controlled manually.");

    // If lights are OFF, turn them ON
    if (!led1Active && !led2Active) {

      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, HIGH);

      led1Active = true;
      led2Active = true;

      unsigned long currentTime = millis();

      led1StartTime = currentTime;
      led2StartTime = currentTime;

      Serial.println("Lights switched ON.");
    }

    // If lights are ON, turn them OFF
    else {

      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);

      led1Active = false;
      led2Active = false;
      Serial.print("Manual button pressed again.")
      Serial.println("Manual lighting cancelled.");
      Serial.println("All lights switched OFF.");
    }
  }
   
  // LED 1 timer
  unsigned long currentTime = millis();

  if (led1Active &&
      currentTime - led1StartTime >= LED1_TIME) {

    digitalWrite(LED1_PIN, LOW);
    led1Active = false;

    Serial.println();
    Serial.println("First light duration completed.");
    Serial.println("First light switched OFF.");
  }

  // LED 2 timer
  if (led2Active &&
      currentTime - led2StartTime >= LED2_TIME) {

    digitalWrite(LED2_PIN, LOW);
    led2Active = false;

    Serial.println();
    Serial.println("Second light duration completed.");
    Serial.println("Second light switched OFF.");
  }

  delay(50);
}
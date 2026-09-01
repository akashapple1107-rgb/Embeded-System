#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

// ---------- Network credentials ----------
char wifiSSID[] = "Redmi Note 13 5G";
char wifiPass[] = "akash@123";

// ---------- MQTT setup ----------
WiFiClient netClient;
MqttClient mqtt(netClient);

const char MQTT_BROKER[] = "broker.emqx.io";
const int  MQTT_PORT     = 1883;

const char TOPIC_WAVE[] = "ES/Wave";
const char TOPIC_PAT[]  = "ES/Pat";

// ---------- Pin mapping ----------
const int PIN_LED_A = 6;
const int PIN_LED_B = 5;

const int PIN_TRIG = 2;
const int PIN_ECHO = 3;

// ---------- Distance thresholds (cm) ----------
const float PAT_MIN  = 0;
const float PAT_MAX  = 10;
const float WAVE_MIN = 20;
const float WAVE_MAX = 50;

const unsigned long ACTION_COOLDOWN_MS = 2000;
const unsigned long LOOP_DELAY_MS      = 200;


// ---------- MQTT message handler ----------
void handleIncomingMessage(int payloadSize) {
  String incomingTopic = mqtt.messageTopic();

  Serial.print("MQTT message received on: ");
  Serial.println(incomingTopic);
  Serial.print("Message: ");

  while (mqtt.available()) {
    Serial.print((char)mqtt.read());
  }
  Serial.println();

  if (incomingTopic == TOPIC_WAVE) {
    setLeds(HIGH);
    Serial.println("Wave topic received -> LEDs turned ON");
  } else if (incomingTopic == TOPIC_PAT) {
    setLeds(LOW);
    Serial.println("Pat topic received -> LEDs turned OFF");
  }
}

void setLeds(uint8_t state) {
  digitalWrite(PIN_LED_A, state);
  digitalWrite(PIN_LED_B, state);
}

// ---------- Ultrasonic distance reading ----------
float readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long echoTime = pulseIn(PIN_ECHO, HIGH, 30000);

  if (echoTime == 0) {
    return -1;
  }

  return (echoTime * 0.0343f) / 2.0f;
}

// ---------- MQTT publish helpers ----------
void sendEvent(const char* topic, const char* label) {
  Serial.print(label);
  Serial.println(" DETECTED");

  mqtt.beginMessage(topic);
  mqtt.print("Aakash");
  mqtt.endMessage();

  Serial.print("Published to ");
  Serial.println(topic);
}

// ---------- Connection setup helpers ----------
void connectWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.begin(wifiSSID, wifiPass) != WL_CONNECTED) {
    Serial.println("Trying again...");
    delay(5000);
  }
  Serial.println("Wi-Fi connected!");
}

void connectMqtt() {
  mqtt.setId("Nano33Iot-Aakash");
  mqtt.onMessage(handleIncomingMessage);

  Serial.println("Connecting to MQTT...");

  if (!mqtt.connect(MQTT_BROKER, MQTT_PORT)) {
    Serial.print("MQTT connection failed. Error = ");
    Serial.println(mqtt.connectError());
    while (true) { /* halt */ }
  }

  Serial.println("MQTT connected!");

  mqtt.subscribe(TOPIC_WAVE);
  mqtt.subscribe(TOPIC_PAT);

  Serial.println("Subscribed to ES/Wave");
  Serial.println("Subscribed to ES/Pat");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  pinMode(PIN_LED_A, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  setLeds(LOW);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  connectWiFi();
  connectMqtt();

  Serial.println("System ready!");
}

// ---------- Main loop ----------
void loop() {
  mqtt.poll();

  float dist = readDistanceCm();

  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.println(" cm");

  if (dist > PAT_MIN && dist <= PAT_MAX) {
    sendEvent(TOPIC_PAT, "PAT");
    delay(ACTION_COOLDOWN_MS);
  }
  else if (dist > WAVE_MIN && dist <= WAVE_MAX) {
    sendEvent(TOPIC_WAVE, "WAVE");
    delay(ACTION_COOLDOWN_MS);
  }

  delay(LOOP_DELAY_MS);
}
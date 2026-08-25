#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Wi-Fi details
const char* ssid = "Redmi Note 13 5G";
const char* pass = "akash@123";

// Your laptop IPv4 address
const char* mqtt_server = "10.18.17.121";

const float LIGHT_THRESHOLD = 50.0;

bool sunlightOn = false;


void setup() {

  Serial.begin(9600);

  while (!Serial) {
    ;
  }

  Serial.println("Starting system...");


  // Start BH1750 sensor
  Wire.begin();

  if (lightMeter.begin()) {
    Serial.println("BH1750 sensor connected!");
  } else {
    Serial.println("BH1750 sensor NOT found!");
  }


  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("Arduino IP address: ");
  Serial.println(WiFi.localIP());


  // MQTT broker
  mqttClient.setServer(mqtt_server, 1883);

  Serial.println("System ready!");
}


void loop() {

  // Connect to MQTT
  if (!mqttClient.connected()) {

    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect("ArduinoTerrariumClient")) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("MQTT connection failed. State: ");
      Serial.println(mqttClient.state());

      delay(5000);
      return;
    }
  }

  mqttClient.loop();


  // Read light level
  float lux = lightMeter.readLightLevel();

  Serial.print("Light level: ");
  Serial.print(lux);
  Serial.println(" lux");


  // Detect sunlight starting
  if (lux > LIGHT_THRESHOLD && !sunlightOn) {

    mqttClient.publish("terrarium/light", "SUNLIGHT_START");

    Serial.println("SUNLIGHT_START sent!");

    sunlightOn = true;
  }


  // Detect sunlight stopping
  else if (lux <= LIGHT_THRESHOLD && sunlightOn) {

    mqttClient.publish("terrarium/light", "SUNLIGHT_STOP");

    Serial.println("SUNLIGHT_STOP sent!");

    sunlightOn = false;
  }


  delay(2000);
}
#include <PubSubClient.h>
#include <WiFi.h>

const char* WIFI_SSID = "Xiaomi 12T"; // WiFI SSID
const char* WIFI_PASSWORD = "xiaomi12t"; // WiFi password
const char* MQTT_SERVER = "34.124.128.203"; // MQTT external IP address
const char* MQTT_TOPIC = "iot"; // MQTT topic
const int MQTT_PORT = 1883; // Communication port
char buffer[128] = ""; // Buffer
WiFiClient espClient; //Object for WiFi client
PubSubClient client(espClient); //Object for Publish/Subscribe client

const int led_red = 19; // For red LED to simulate warning light
const int relay_pin = 32; // For Octocoupler pin that controls the fan (ventilation)
const int smoke_pin = 14; // For green LED to simulate on/off

float smoke = 0; // Placeholder for smoke readings

void setup_wifi() {
  delay(10);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(10000);

  // Set the sensors and acutators pin mode
  pinMode(smoke_pin, INPUT);
  pinMode(led_red, OUTPUT);
  pinMode(relay_pin, OUTPUT);

  // Set the default behaviour of the devices
  digitalWrite(led_red, LOW);
  digitalWrite(relay_pin, false);

  setup_wifi(); // Connect to the WiFi network
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if(!client.connected()) {
    reconnect();
  } 
  client.loop();
  delay(10000);

  smoke = analogRead(smoke_pin);

  if (smoke > 400) {
    sprintf(buffer, "Air quality: %.2f", smoke);
    sprintf(buffer, "Warning light and ventilation ON");

    Serial.print("Air quality: ");
    Serial.println(smoke);
    Serial.println("Air quality at danger level. Ventilation on.\n");
    
    digitalWrite(led_red, HIGH);
    digitalWrite(relay_pin, true);
  }
  else {
    sprintf(buffer, "Air quality: %.2f", smoke);
    sprintf(buffer, "Warning light and ventilation OFF");

    Serial.print("Air quality: ");
    Serial.println(smoke);
    Serial.println("Air quality at normal level. Ventilation off.\n");

    digitalWrite(led_red, LOW);
    digitalWrite(relay_pin, false);
  }

  client.publish(MQTT_TOPIC, buffer);
}

void reconnect() {
  while (!client.connected()) { 
    Serial.println("Attempting MQTT connection...");
    
    if(client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT server");
    } 
    else { 
      Serial.print("Connection failed, reason =");
      Serial.print(client.state());
      Serial.println(" Retrying in 3 seconds...");
      delay(3000);
    } 
  } 
}
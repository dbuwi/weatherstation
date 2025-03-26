#include <Adafruit_GFX.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <WiFi.h> 
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <ArduinoJson.h>
#include "NTP.h"
#include "mqtt.h"

// DEFINED CONTROL PINS FOR TFT DISPLAY
#define TFT_DC    17
#define TFT_CS    5
#define TFT_RST   16
#define SCREEN_WIDTH  240  // For ILI9341
#define SCREEN_HEIGHT 320  // For ILI9341

// DEFINED CONTROL PINS FOR THE DHT22
#define DHTPIN 4
#define DHTTYPE DHT22

// DEFINED CONTROL PINS FOR BMP280
#define BMP280_SDA  21
#define BMP280_SCL  22
#define SoilPin     32  // Soil moisture pin

// MQTT CLIENT CONFIG  
static const char* pubtopic      = "620162321";                    // Add your ID number here
static const char* subtopic[]    = {"620162321_sub","/elet2415"};  // Array of Topics(Strings) to subscribe to
static const char* mqtt_server   = "broker.emqx.io";         // Broker IP address or Domain name as a String 
static uint16_t mqtt_port        = 1883;

// WIFI CREDENTIALS
const char* ssid       = "MonaConnect";     // Add your Wi-Fi ssid
const char* password   = ""; // Add your Wi-Fi password 

// Initialized DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialized BMP280 sensor
Adafruit_BMP280 bmp;

// Initialized TFT display 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    dht.begin();
    Wire.begin(BMP280_SDA, BMP280_SCL);
    
    if (!bmp.begin(0x76)) {
        Serial.println("Could not find a valid BMP280 sensor.");
        while (1);
    }
    
    delay(100);  
    tft.begin();  // ILI9341 initialization
    tft.setRotation(2);  
    tft.fillScreen(ILI9341_BLACK);  //  Background color
    tft.setTextColor(ILI9341_WHITE);  // Text color 
    tft.setTextSize(1);
    tft.setCursor(40, 30);
    tft.print("Sensor Data");
    delay(500); // Ensures that the initial text is displayed


// **Wi-Fi Connection Debugging**
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Wait up to 20 attempts
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi Connection Failed!");
    }

    // **Initialize MQTT**
    connectToMQTT();
}

// **Function to Connect to MQTT**
void connectToMQTT() {
    Serial.print("Connecting to MQTT...");
    
    mqtt.setServer(mqtt_server, mqtt_port);

    if (mqtt.connect("weatherstation")) {
        Serial.println("MQTT Connected!");
        mqtt.subscribe(subtopic[0]);
        mqtt.subscribe(subtopic[1]);
    } else {
        Serial.print("MQTT Connection Failed, State: ");
        Serial.println(mqtt.state());
    }
}

void publishSensorData() {
    StaticJsonDocument<256> doc;
    doc["Data"] = "Sensor";
    doc["ID"] = "620162321";
    doc["temperatureDHT"] = round(dht.readTemperature()*100) /100.0;
    doc["humidity"] = round(dht.readHumidity()*100) /100.0;

    // Calculate heatIndex
    double heatIndex = -8.784695 + (1.61139411 * dht.readTemperature()) + (2.33854884 * dht.readHumidity())
                   - (0.14611605 * dht.readTemperature() * dht.readHumidity())
                   - (0.012308094 * pow(dht.readTemperature(), 2))
                   - (0.0164248277778 * pow(dht.readHumidity(), 2))
                   + (0.002211732 * pow(dht.readTemperature(), 2) * dht.readHumidity())
                   + (0.00072546 * dht.readTemperature() * pow(dht.readHumidity(), 2));

    // Round the heatIndex to two decimal places
    doc["heatIndex"] = round(heatIndex * 100) / 100.0;

    // Read and round the other sensor data to two decimal places
    doc["temperatureBMP"] = round(bmp.readTemperature() * 100) / 100.0;
    doc["pressure"] = round(bmp.readPressure() / 100.0F * 100) / 100.0; // Convert to hPa and round
    doc["altitude"] = round(bmp.readAltitude() * 100) / 100.0;
    doc["soilMoisture"] = round(map(analogRead(SoilPin), 0, 4095, 100, 0) * 100) / 100.0;

    char buffer[256];
    serializeJson(doc, buffer); // Convert JSON to char array

    mqtt.publish(pubtopic, buffer); // Publish JSON data to the MQTT topic

    Serial.println("Data Published to MQTT!");
}

void loop() {
    // Reads humidity and temperature from DHT22
    float humidity = dht.readHumidity();
    float temperatureDHT = dht.readTemperature(); // Celsius
    
    // Checks if DHT22 readings are valid
    if (isnan(humidity) || isnan(temperatureDHT)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Calculation of Heat Index
    float heatIndex = -8.784695 + (1.61139411 * temperatureDHT) + (2.33854884 * humidity)
               - (0.14611605 * temperatureDHT * humidity) - (0.012308094 * temperatureDHT * temperatureDHT)
               - (0.0164248277778 * humidity * humidity) + (0.002211732 * temperatureDHT * temperatureDHT * humidity)
               + (0.00072546 * temperatureDHT * humidity * humidity) - (0.000003582 * temperatureDHT * temperatureDHT * humidity * humidity);

    // Reads temperature and pressure from BMP280
    float temperatureBMP = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
    float altitude = bmp.readAltitude();
    // Reads soil moisture sensor
    int soilValue = analogRead(SoilPin);  
    int soilMoisture = map(soilValue, 0, 4095, 100, 0);  // Prcentage (0% = Dry, 100% = Wet)

    // Print values to Serial Monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temp DHT: ");
    Serial.print(temperatureDHT);
    Serial.print("C  Heat Index: ");
    Serial.print(heatIndex);
    Serial.print("C  Temp BMP: ");
    Serial.print(temperatureBMP);
    Serial.print("C  Pressure: ");
    Serial.print(pressure);
    Serial.print(" hPa  Soil Moisture: ");
    Serial.print(soilMoisture);
    Serial.println("%");
    Serial.print(F("Altitude = "));
    Serial.print(bmp.readAltitude(1013.25));
    Serial.println(" m"); 

   // **Wi-Fi Reconnection Debugging**
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected! Attempting Reconnection...");
        WiFi.begin(ssid, password);
    }

    // **MQTT Reconnection Debugging**
    if (!mqtt.connected()) {
        Serial.println("MQTT Disconnected! Reconnecting...");
        connectToMQTT();
    }

    mqtt.loop(); // Keep MQTT connection alive
    publishSensorData();

    //Display Temperature from DHT
    tft.fillRect(20, 60, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 60);
    tft.print("Temp DHT: ");
    tft.print(temperatureDHT);
    tft.print(" C");
        
    //Display Humidity from DHT
    tft.fillRect(20, 100, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 100);
    tft.print("Humidity: ");
    tft.print(humidity);
    tft.print("%");

  // Display Heat Index which was calculated
    tft.fillRect(20, 140, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 140);
    tft.print("Heat Index: ");
    tft.print(heatIndex);
    tft.print(" C");
    
    //Display Temperature from BMP
    tft.fillRect(20, 180, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 180);
    tft.print("Temp BMP: ");
    tft.print(temperatureBMP);
    tft.print(" C");

   // Display Pressure from BMP
    tft.fillRect(20, 220, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 220);
    tft.print("Pressure: ");
    tft.print(pressure);
    tft.print(" hPa");

   //Display Soil Moisture Percentage
    tft.fillRect(20, 260, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 260);
    tft.print("Soil Moisture: ");
    tft.print(soilMoisture);
    tft.print("%");

   //Display Altitude
    tft.fillRect(20, 300, 200, 30, ILI9341_BLACK);  
    tft.setCursor(20, 300);
    tft.print("Altitude:");
    tft.print(altitude);
    tft.print("m");

   
    delay(2000); // Refresh every 2 seconds
}

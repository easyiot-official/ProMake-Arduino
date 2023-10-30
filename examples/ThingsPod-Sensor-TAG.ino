
//Sensors ==========================================
#include "ProMake_SHT20.h"
#include "ProMake_VEML7700.h"

ProMake_SHT20 sht20;
ProMake_VEML7700 als;

// WiFi Connection =================================
#include <SPI.h>
#include <WiFiNINA.h>

//char ssid[] = "EzIoT";            // your network SSID (name)
//char pass[] = "papalam121";        // your network password
char ssid[] = "AlnoAP";            // your network SSID (name)
char pass[] = "kdlo6835";        // your network password
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// Initialize the Ethernet client object
WiFiClient espClient;

// IOT Platform ===================================
#include "ThingsBoard.h"
// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN               "pm3UWWGVZxdc5ctyPNk4"
#define THINGSBOARD_SERVER  "cloud.gigapardaz.com"

// Initialize ThingsBoard instance
ThingsBoardSized<512> tb(espClient);

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in milliseconds
//const unsigned long postingInterval = 600000L; // delay between updates, in milliseconds


void setup(void)
{
  Serial.begin(9600);

  Wire.begin();  

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.println(status);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  Serial.print("Done");

  als.begin();   // Init
  sht20.initSHT20();                         // Init SHT20 Sensor
  delay(1000);
  sht20.checkSHT20();                        // Check SHT20 Sensor
}

void loop(void)
{
  float humd = sht20.readHumidity();         // Read Humidity
  float temp = sht20.readTemperature();      // Read Temperature
  float lux;
  als.getALSLux(lux);   // Get the measured ambient light value
  
  if (WiFi.status() == WL_IDLE_STATUS) {
    // Still connecting
    delay(1000);
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to AP ...");
    // Connect to WPA/WPA2 network
    WiFi.begin(ssid, pass);
    delay(1000);
    return;
  }

  if (!tb.connected()) {
    Serial.println("Connected");
    // Connect to the ThingsBoard
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      oled.println("Failed to connect, retrying ...");
      return;
    }
  }

  if (millis() - lastConnectionTime > postingInterval) {

    const int data_items = 3;
    Telemetry data[data_items] = {
      { "temperature", temp },
      { "humidity",    humd },
      { "light",    lux },
    };
    tb.sendTelemetry(data, data_items);
    
    lastConnectionTime = millis();
  }

  Serial.println("Waiting for data...");
  tb.loop();

  delay(100);
  
}

#include "ProMake_SHT20.h"
#include "ProMake_VEML7700.h"

ProMake::SHT20 sht20;
ProMake::VEML7700 als;

void setup()
{
#ifdef ESP32
    Wire.setTimeOut(500);
#endif
    // Start the I2C Communication
    Wire.begin();
    
    Serial.begin(9600);

    als.begin(); // Init
    delay(100);
    sht20.checkSHT20(); // Check SHT20 Sensor
}

void loop()
{
    float humd = sht20.readHumidity();     // Read Humidity
    float temp = sht20.readTemperatureC(); // Read Temperature

    Serial.print("humd:");
    Serial.print(humd);
    Serial.println(" %");

    Serial.print("temp:");
    Serial.print(temp);
    Serial.println(" C");

    float lux;

    als.getALSLux(lux); // Get the measured ambient light value
    Serial.print("Lux:");
    Serial.print(lux);
    Serial.println(" lx");

    Serial.println("-------------------------");

    delay(2000); // wait 2 seconds for next scan
}

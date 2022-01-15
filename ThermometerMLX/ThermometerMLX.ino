/*************************************************** 
  This is a library example for the MLX90614 Temp Sensor

  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1747 3V version
  ----> https://www.adafruit.com/products/1748 5V version

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <MLX90614.h>
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define SDA_pin D4
#define SCL_pin D3

MLX90614 mlx = MLX90614();

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA_pin, SCL_pin);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h


void setup() {
  Serial.begin(115200);

  Serial.println("MLX90614 test");  

  mlx.begin(SDA_pin, SCL_pin);  


  // Initialising the UI will init the display too.
  display.init();

  //display.flipScreenVertically();
  //display.setFont(ArialMT_Plain_24);

  
}

void loop() {
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  Serial.println();
  // clear the display
  //display.clear();
  
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.drawString(1, 1, "OBJ:");
  //display.drawString(60, 1, String(mlx.readObjectTempC()));
  
  // write the buffer to the display
  //display.display();

  
  delay(500);
}

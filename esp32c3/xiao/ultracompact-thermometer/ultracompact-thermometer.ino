#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
    mlx.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
    display.clearDisplay();
    display.display();
}

void loop() {
    float obTemperature = mlx.readObjectTempC();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(37, 35);
    display.print(obTemperature);
    display.setTextSize(1);
    display.setCursor(45,55);
    display.print("CELSIUS");
    display.display();
    delay(500);  
}

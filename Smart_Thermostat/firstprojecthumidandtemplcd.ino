#include "ShiftedLCD.h"
#include "DHT.h"

// DHT setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); 

// initialize the library with the number of the sspin 
// (or the latch pin of the 74HC595)
LiquidCrystal lcd(9);

//initilize object dht for class DHT with DHT pin with STM32 and DHT type as DHT11
  

void setup() {

dht.begin();          //Begins to receive Temperature and humidity values.
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  delay(3000);
  lcd.clear();
}

void loop() 
{
  delay(2000);
  float h = dht.readHumidity();       //Gets Humidity value
  float t = dht.readTemperature();    //Gets Temperature value
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");
  lcd.setCursor(0,1);
  lcd.print("Humid: ");
  lcd.print(h);
  lcd.print(" %");
}
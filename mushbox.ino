#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_SHT31.h" 

// LCD Instructions: https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/
// 

// Constants 
// ----------
// I2C pins declaration
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Temperature/Humidity Sensor
Adafruit_SHT31 sht31 = Adafruit_SHT31();


// constants won't change
const int RELAY_PIN = 3;  // the Arduino pin, which connects to the IN pin of relay



int DegreeChar = 0;
byte Degree[8] = {
0b01110,
0b01010,
0b01110,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000
};



// ---------


// Temporary Vars
// ------

bool enableHeater = false;
uint8_t loopCnt = 0;
uint8_t screenCounter = 0;


bool enableHumidifier = false;
// ------


// Not so Constants
// ------

float HUMIDITY_TOLERANCE = 5.0;
float TARGET_HUMIDITY_PERCENTAGE = 90.0;
uint8_t HEATER_INTERVAL = 30;



// the setup function runs once when you press reset or power the board
void setup() {
  
  
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.createChar(0, Degree);

  write_to_screen("Booting...", "");
  
  // initialize digital pin as an output.
  pinMode(RELAY_PIN, OUTPUT);
  
  delay(500);
  
  bool sensor_started = sht31.begin(0x44);
  if (!sensor_started) {
    write_to_screen("SHT31:", "  Unstarted");
//    while (1) delay(1); // stay here forever
  } else {
    write_to_screen("SHT31:", "  Started");
  }
  
  delay(1500);

  if (sht31.isHeaterEnabled()) {
    write_to_screen("SHT Heater:", "  ENABLED");
  } else {
    write_to_screen("SHT Heater:", "  DISABLED");
  }

  delay(1500);




  
  

  write_to_screen("","");
}

// the loop function runs over and over again forever
void loop() {

  float temperature = read_temperature();
  float humidity = read_humidity();
  
  bool heating = set_heater();
  bool humidifying = set_humidity(humidity);

  write_to_screen(temperature, humidity, heating, humidifying);
  
  delay(1000);
}

// TODO: deal with the heater situation....


bool set_heater() {
  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  if (loopCnt >= HEATER_INTERVAL) {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    loopCnt = 0;
  }
  loopCnt++;
  return enableHeater;
  
}

float read_temperature() {
//  return sht31.readTemperature();
  return 23.78123498712;
}

float read_humidity() {
//  return sht31.readHumidity();
  return 86.56;
}


void write_to_screen(String msg, String msg2) {
  lcd.clear(); 
  
  lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
  lcd.print(msg);
  
  lcd.setCursor(0,1);   //Set cursor to character 0 on line 1
  lcd.print(msg2);
}

void write_to_screen(int temperature, int humidity, bool heating, bool humidifying) {
  
  // Print a message on both lines of the LCD.
  lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
  lcd.print(String(temperature));
  lcd.write(DegreeChar);
  lcd.print("C");

  lcd.print("  ");
  lcd.print(String(humidity) + " %");  


  
  
  lcd.setCursor(0,1);   //Move cursor to character 0 on line 1
  lcd.print("Ht: " + String(heating));
  
  lcd.print(" ");
  lcd.print("Hu: " + String(humidifying));

  
}

bool set_humidity(float humidity) {
  if (humidity > TARGET_HUMIDITY_PERCENTAGE + HUMIDITY_TOLERANCE ) {
    stop_humidity();
    enableHumidifier =  false;
  } else if (humidity < TARGET_HUMIDITY_PERCENTAGE - HUMIDITY_TOLERANCE) {
    start_humidify();
    enableHumidifier = true;
  } 
  return enableHumidifier;
}
bool start_humidify() {
  digitalWrite(RELAY_PIN, HIGH);
}

bool stop_humidity() {
  digitalWrite(RELAY_PIN, LOW);
}

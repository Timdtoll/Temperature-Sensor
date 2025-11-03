#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>

//Initialize LCD
int ROWS = 2;
int CHARACTERS = 16;
LiquidCrystal_I2C lcd(0x27, CHARACTERS, ROWS); //Address, # Char per row, # rows

byte celciusChar[] = {7, 5, 7, 0, 0, 0, 0, 0}; //Custom Character for degree symbol. Bit 1 to enable light, 0 to disable light. 

//Character looks like:
//000111
//000101
//000111
//000000
//000000
//000000
//000000
//000000

//Initialize Temperature Sensors
#define ONE_WIRE_BUS 2 //Which GPIO Pin used (Labbeled as GPx on Pico, where x is the number after the GP) for Temperature sensors
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); //create DallasTemperature object called sensor, allow it to use oneWire object

int sensorCount = 2; //Stores number of connected temperature sensors
DeviceAddress tempSensorAddress; //Address of temperature sensors, changes

//Set Relay Pins
int relayHeatPinOUT = 10; //GPx pin relay signal pin is attached to (For the relay that controlls the "heating")
int relayCoolPinOUT = 20; //GPx pin relay signal pin is attached to (For the relay that controlls the "cooling")

//Other Variables
int cycle = 0; //Every X cycles, do a task you dont need to do every cycle
int upperTemperatureBound = 29;
int lowerTemperatureBound = 24;

void setup() {
  //Start up LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, celciusChar);

  //Start up Temperature Sensors and get # currently connected
  sensors.begin();
  sensorCount = sensors.getDeviceCount();
  
  //Relay
  pinMode(10, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(relayHeatPinOUT, LOW);
  digitalWrite(relayCoolPinOUT, LOW);

  //Serial for Debug
  Serial.begin(9600);
}

void loop() {
  delay(2000); //2 second
  if (cycle == 3) { //30 seconds in theory, probably only slightly more but the nature of the project allows large time discrepencies. Compare temperature 
     float outside = sensors.getTempCByIndex(0);
     float inside = sensors.getTempCByIndex(1);
     if (sensorsAllPositive(sensorCount)) { //Check if ALL sensors operational
        //Assume sensor 1 for Inside, 0 for Outside
        if (!((lowerTemperatureBound <= inside) and (inside <= upperTemperatureBound))) { //Check if Inside temperature is not within the set limits for the room
          if ((inside < lowerTemperatureBound) and (inside < outside)) {
            //Inside lower than X degrees, since inside is colder than outside, use outside to heat up inside
            Serial.println("InHeat");
            digitalWrite(relayHeatPinOUT, HIGH);
          }
          
          if ((inside > upperTemperatureBound) and (inside > outside)) {
            //Inside higher than X degrees, since inside is warmer than outside, use outside to cool down inside
            Serial.println("InCool");
            digitalWrite(relayCoolPinOUT, HIGH);
          }
        } else { //If within range, stop heating or cooling (both at the same time easiest method)
          Serial.println("In bounds");
          digitalWrite(relayHeatPinOUT, LOW);
          digitalWrite(relayCoolPinOUT, LOW);
        }
     } else { //Not all sensors operational
        Serial.println("Sensor not operating");
     }
     cycle = 0; //Reset
  }
  sensors.requestTemperatures(); //Get temp from ALL CURRENTLY CONNECTED sensors
  for(int i = 0; i < sensorCount; i++) { //Cycle through each sensor
     if (sensors.getAddress(tempSensorAddress, i)) { //Check if the sensor still exits (If the address can still be found)
        //Print Temperature to each line
        clearSingleLine(i);
        switch (i) {
          case 0:
            lcd.print("Out");
            break;
          case 1:
            lcd.print("Ins");
            break;
        }
        
        lcd.print(String(" TMP") + String(sensors.getTempC(tempSensorAddress)));
        lcd.write(byte(0)); //Degree symbol
        lcd.print("C");
     } else {
        //Print Error to LCD
        clearSingleLine(i);
        lcd.print(String("SNSR") + String(i) + String(" ERROR")); 
     }
  }
  cycle++;
}

//To update a single line instead of clearing the whole LCD with lcd.clear();
void clearSingleLine(int line) {
   for (int i = 0; i < CHARACTERS; i++) { //Replace all characters currently printed with " "
       lcd.setCursor(i, line);
       lcd.print(" ");
   }
   lcd.setCursor(0, line); //Return to start of line
}

//Checks if ALL connected sensors are functional (As opposed to checking one and printing an error on the LCD)
boolean sensorsAllPositive(int sensorCount) { 
  for (int i = 0; i < sensorCount; i++) {
     if (sensors.getAddress(tempSensorAddress, i)) {} //If getAddress returns true, do nothing
     else {return false;} //If getAddress returns false, return false for the function
  }
  return true; //All sensors OK return true
}

//Empty loop to stop
void stop()
{
  while(1);
}

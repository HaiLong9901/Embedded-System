#include <Arduino_FreeRTOS.h>
#include <DHT11.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
// #include <Arduino.h>
#define portCHAR		char
int signalFan = 3;
int signalHeater = 4;
int signalCooler = 12;
int tempUpperLimit = 30;
int tempLowerLimit = 15;
int humidLimit = 75;
int state = 0;
LiquidCrystal lcd(2, A5, A4, A3, A2, A1);

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROWS] = {8, 7, 6, 5};
byte pin_cols[COLS] = {11, 10, 9};

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, ROWS, COLS);
DHT11 dht(A0);

void TaskControl(void *pvParameters);
void TaskDisplay(void *pvParameters);


void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("HVAC system"); 
  pinMode(signalFan, OUTPUT);
  pinMode(signalHeater, OUTPUT);
  pinMode(signalCooler, OUTPUT);
  Serial.begin(9600);

  xTaskCreate(
    TaskControl
    , (const portCHAR *)"Control"
    , 128
    , NULL
    , 2
    , NULL
  );
  xTaskCreate(
    TaskDisplay
    , (const portCHAR *)"Display"
    , 128
    , NULL
    , 3
    , NULL
  );


  vTaskStartScheduler();
}
void loop() {
}

void TaskControl(void *pvParameters) {
  (void) pvParameters;
  int temperature;
  int humidity;
  for(;;) {
    Serial.println("TaskControl");
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    if (isnan(temperature) || isnan(humidity)) { 
    } else {
      if (temperature < tempLowerLimit) {
        digitalWrite(signalHeater, HIGH);
        digitalWrite(signalCooler, LOW);
      } else if (temperature > tempUpperLimit) {
        digitalWrite(signalHeater, LOW);
        digitalWrite(signalCooler, HIGH);
      } else {
        digitalWrite(signalHeater, LOW);
        digitalWrite(signalCooler, LOW);
      }

      if (humidity > humidLimit) {
        digitalWrite(signalFan, HIGH);
      } else {
        digitalWrite(signalFan, LOW);
      }
    }
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

void TaskDisplay(void *pvParameters) {
  (void) pvParameters;
  int temperature = 0;
  int humidity = 0;
  char customKey;
  int digit = -1;
  int contemp = -1;
  for(;;) {
    customKey = keypad.getKey();
    switch(state) {
      case 0: {
        vTaskDelay(20);
        if (customKey && customKey == '*') {
          Serial.println(customKey);
          ++state;
        }
        else {
          humidity = dht.readHumidity();
          temperature = dht.readTemperature();
          if (isnan(temperature) || isnan(humidity)) { 
          } 
          else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Humidity: ");
          lcd.setCursor(12, 0);
          lcd.print(round(humidity));
          lcd.print("%");
          lcd.setCursor(0, 1);
          lcd.print("Temp: ");
          lcd.print(round(temperature));
          lcd.setCursor(9, 1);
          lcd.print( (char)223);
          lcd.print("C");
          }
        }
        break;
      };
      
      case 1: {
        vTaskDelay( 20);
        if (customKey && customKey == '*') {
          contemp = -1;
          digit = -1;
          state = 0;
        } else if (customKey && customKey == '#') {
          if (contemp >= 0) {
            tempUpperLimit = contemp;
          }
          contemp = -1;
          digit = -1;
          state = 2;
        } else if (customKey && customKey != '*' && customKey != "#") {
          if (digit <= 0) {
            digit = (int) customKey - 48;
            contemp = digit;
            continue;
          } else {
            contemp = digit * 10 + ((int) customKey - 48);
          }
        }
        else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("TempUpper: ");
          if (contemp >= 0) {
            lcd.print(contemp);
          } else {
            lcd.print(tempUpperLimit);
          }
          lcd.setCursor(13, 0);
          lcd.print( (char)223);
          lcd.print("C");
          lcd.setCursor(0, 1);
          lcd.print("Pess # to save");
        } 
        break;
      };
     
      case 2: {
        vTaskDelay( 20);
        if (customKey && customKey == '*') {
          contemp = -1;
          digit = -1;
          state = 1;
        } else if (customKey && customKey == '#') {
          if (contemp >= 0) {
            tempLowerLimit = contemp;
          }
          contemp = -1;
          digit = -1;
          state = 3;
        } else if (customKey && customKey != '*' && customKey != "#") {
          if (digit <= 0) {
            digit = (int) customKey - 48;
            contemp = digit;
            continue;
          } else {
            contemp = digit * 10 + ((int) customKey - 48);
        }
        }else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("TempLower: ");
          if (contemp >= 0) {
            lcd.print(contemp);
          } else {
            lcd.print(tempLowerLimit);
          }
          lcd.setCursor(13, 0);
          lcd.print( (char)223);
          lcd.print("C");
          lcd.setCursor(0, 1);
          lcd.print("Pess # to save");
        }
        break;
      };
      
      case 3: {
        vTaskDelay( 20);
        if (customKey && customKey == '*') {
          contemp = -1;
          digit = -1;
          state = 2;
        } else if (customKey && customKey == '#') {
          if (contemp >= 0) {
            humidLimit = contemp;
          }
          contemp = -1;
          digit = -1;
          state = 0;
        } else if (customKey && customKey != '*' && customKey != "#") {
          if (digit <= 0) {
            digit = (int) customKey - 48;
            contemp = digit;
            continue;
          } else {
            contemp = digit * 10 + ((int) customKey - 48);
        }
        }else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("HumidLimit: ");
          lcd.setCursor(12, 0);
          if (contemp >= 0) {
            lcd.print(contemp);
          } else {
            lcd.print(humidLimit);
          }
          lcd.print("%");
          lcd.setCursor(0, 1);
          lcd.print("Pess # to save");
        }
      };
      break;
      default: {

      }
        
    }
  }
}


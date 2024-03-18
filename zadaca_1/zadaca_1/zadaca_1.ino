#include <FSBAVR.h>
#include <LiquidCrystal.h>
#include <DS1821.h>
#include <TimerOne.h>
#include <OneWire.h>

// Ports
#define Temp_sensor PORT_B1
#define Heater PORT_B4
#define Buzz PORT_D1
// Button ports
#define Temp_up PORT_C1
#define Temp_down PORT_C2
#define Kp_up PORT_C3
#define Kp_down PORT_C4
#define Ki_up PORT_C5
#define Ki_down PORT_C6

OneWire oneWire(Temp_sensor);

// Variables
int setpoint = 25;  // Desired temperature
float Kp = 5;       // Proportional gain
float Ki = 0.1;     // Integral gain
int currentTemperature = 0;
float integral;
float output; // Result of the PI regulation
int error; // Regulation error

void setup() {
  Serial.begin(9600);
  lcd.begin(20,4); // Initializing the LCD display
  pinMode(Temp_up, INPUT_PULLUP);
  pinMode(Temp_down, INPUT_PULLUP);
  pinMode(Kp_up, INPUT_PULLUP);
  pinMode(Kp_down, INPUT_PULLUP);
  pinMode(Ki_up, INPUT_PULLUP);
  pinMode(Ki_down, INPUT_PULLUP);
  pinMode(Heater, OUTPUT);

  Timer1.initialize(1000000); // Setting the interrupt interval to 1 second
  Timer1.attachInterrupt(Regulation); // Calling the function 'Regulation' as the interrupt function

}

void loop() {
  Serial_Monitor(); // Calling the 'Serial_Monitor' function
  Buttons(); // Calling the 'Buttons' function
  LCD_print(); // Calling the 'LCD_print' function
}

void Regulation() {
  // Reading the current temperature from the sensor
  currentTemperature = oneWire.read(); 
  // Calculating regulation error
  error = setpoint - currentTemperature; 
  // Proportional term
  float proportional = Kp * error;
  // Integral term
  integral += Ki * error;

  // Limiting integral term to prevent windup
  integral = constrain(integral, -100, 100);

  // Calculating the result of the PI regulation
  float output = proportional + integral;

//  if (output < 0) {
//    output = 0;
//    }
//  if (output > 255) {
//    output = 255;
//    }

  // Mapping the 'output' variable to the PWM values
  output = map(output, 0, 255, 0, 255);

  // Applying the output to the heater (light bulb)
  analogWrite(Heater, output);
}

void Serial_Monitor() {
  Serial.print("Setpoint:");
  Serial.print(setpoint);
  Serial.print("Temperature:");
  Serial.print(currentTemperature);
  Serial.print("Regulator output:");
  Serial.println(output); 
}

void Buttons() {
  if (digitalRead(Temp_up) == LOW) {
    setpoint += 1; // Increasing 'setpoint' by 1
    }

  if (digitalRead(Temp_down) == LOW) {
    setpoint -= 1; // Decreasing 'setpoint' by 1
    // Setting the lower limit of 'setpoint' to 0
    if (setpoint < 0) {
      setpoint = 0;
      }
    }  

  if (digitalRead(Kp_up) == LOW) {
    Kp += 0.1; // Increasing proportional gain by 0.1
    }

  if (digitalRead(Kp_down) == LOW) {
    Kp -= 0.1; // Decreasing proportional gain by 0.1
    // Setting the lower limit of proportional gain to 0
    if (Kp < 0) {
      Kp = 0;
      }
    } 

  if (digitalRead(Ki_up) == LOW) {
    Ki += 0.1; // Increasing integral gain by 0.1
    }

  if (digitalRead(Ki_down) == LOW) {
    Ki -= 0.1; // Decreasing integral gain by 0.1
    // Setting the lower limit of integral gain to 0
    if (Ki < 0) {
      Ki = 0;
      }
    } 
}

void LCD_print() {
  lcd.clear(); // Clearing the LCD display
  // Printing the measured temperature
  lcd.print("Temp.[C]"); 
  lcd.print(currentTemperature);
  // Printing the regulation error
  lcd.print("Reg. error"); 
  lcd.print(error);
  lcd.setCursor(0, 1);
  // Printing the result of the PI regulation
  lcd.print("Reg. output"); 
  lcd.print(output);

  lcd.setCursor(0, 2);
  // Printing the setpoint, i.d. the desired temperature
  lcd.print("Setpoint"); 
  lcd.print(setpoint);
  // Printing the proportional gain
  lcd.print("Kp"); 
  lcd.print(Kp);
  // Printing the integral gain
  lcd.print("Ki");
  lcd.print(Ki);
}

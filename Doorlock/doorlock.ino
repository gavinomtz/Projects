#include <Keypad.h>
#include <Servo.h>

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] =
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo setup
Servo lockServo;
const int servoPin = 10;

// LED setup
const int RLED = 11;
const int GLED = 12;

// Buzzer setup
const int buzzerPin = 13;

// Stock password initialization
String password = "1234";  
String input = "";

// the setup function runs once when you press reset or power the board
void setup() 
{

  Serial.begin(9600);
  lockServo.attach(servoPin);
  lockServo.write(0); // Lock closed

  // initialize digital pin RLED, GLED, and Alarm as an output.
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

// the loop function runs over and over again forever
void loop() 
{
  char key = keypad.getKey();

  if (key)
  {
    Serial.println(key);
    if (key == '#') // Enter key
    {
      if (input == password) 
      {
        lockServo.write(90); // Unlock
        Serial.println("Access Granted");
        digitalWrite(GLED, HIGH);
        delay(5000);         // Keep unlocked for 5s
        digitalWrite(GLED, LOW);
        lockServo.write(0);  // Lock again
      } 
      else 
      {
        Serial.println("Access Denied");
        digitalWrite(RLED, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(3000);
        digitalWrite(buzzerPin, LOW);
        digitalWrite(RLED, LOW);
      }
      input = ""; // Reset input
    } 
    else if (key == '*') 
    {
      input = ""; // Clear input
    } 
    else 
    {
      input += key; // Append key to input string
    }
  }
}

// TimeOne to control Timer1

#include <TimerOne.h>


// Utility constants

#define SECONDS_TO_US 1000000
#define SECONDS_TO_MS 1000  


// Hardware Mapping

#define LDR_PIN A0
#define BUTTON_PIN 2
#define PUMP_PIN 8


// PUMPING DEFINITIONS

#define PUMP_PERIOD 5
#define PUMP_INTERVAL 2


// LDR MAPPING AND THRESHOLD

#define MAX_LDR_VALUE 850
#define MIN_LDR_VALUE 0

#define DAY_NIGHT_THRESHOLD 25


// Dynamic Variables 

unsigned int secondsCount = 0;
unsigned int ldrValue = 0;

bool isForcedPumping      = false;
bool isDay                = false;
bool isPumpingNow         = false;


// ISR's

void ISRIncrementSecondsCount() {
  secondsCount += 1;
}

void ISRSetIsForcedPumping() {
  isForcedPumping = true;
}


// Main Program

void setup() {
  
  pinMode(LDR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PUMP_PIN, OUTPUT);
    
  Timer1.initialize(1 * SECONDS_TO_US);
  Timer1.attachInterrupt(ISRIncrementSecondsCount);

  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    ISRSetIsForcedPumping,
    RISING
  );

  Serial.begin(9600);

}

void pumpIt() {

  Timer1.stop();

  digitalWrite(PUMP_PIN, HIGH);
  delay(PUMP_INTERVAL * SECONDS_TO_MS);
  digitalWrite(PUMP_PIN, LOW);

  secondsCount = 0;
  Timer1.restart();
    
}

void loop() {

  if (isForcedPumping) {
    pumpIt();

    isForcedPumping = false;
  }
  
  Serial.println(String("É dia? ") + (isDay ? "sim" : "não"));
  
  ldrValue = map(
    analogRead(LDR_PIN), 
    MIN_LDR_VALUE, 
    MAX_LDR_VALUE, 
    100, 
    0
  );

  isDay = DAY_NIGHT_THRESHOLD <= ldrValue;

  if (!isDay) {
    secondsCount = 0;
  }
  
  isPumpingNow = secondsCount >= PUMP_PERIOD;

  if (isDay && isPumpingNow) {
    pumpIt();
  }
  
}

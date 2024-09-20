//Author: Elias Fu Aka. Juncheng Fu
//Last modified: 2023-08-23

const unsigned int MSensor_Pin = 3; //Motion Sensor Pin
const unsigned int LED_1_Pin = 8;
const unsigned int LED_2_Pin = 9;

const int Sonic_1_Pin = 5;
const int Sonic_2_Pin = 6;

const float Sonic_1_Range = 52; // in cm from 50 to 60 then it never turns off. 60to55 extream sensitive and unstable. 55-52
const float Sonic_2_Range = 15; // in cm

const unsigned int Standby_Timer = 30; // in Sec
const unsigned int Standard_Timer = 30; // in Sec
const unsigned int Hot_Timer = 60; // in Min

const int ledPin =  LED_BUILTIN;


//Timer converter****
const unsigned long Standby_Timer_ms = Standby_Timer * 1000;
const unsigned long Standard_Timer_ms = Standard_Timer * 1000;
const unsigned long Hot_Timer_ms = 60000;;

//Memories***************
unsigned int Cold_State_Counter = 0;
unsigned long RunTime = 0;
int Duration = 0;

int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

//Memories---------------

void setup() {
  
  pinMode (MSensor_Pin, INPUT); //Motionsensor
  
  pinMode (LED_1_Pin , OUTPUT); //LED 1
  pinMode (LED_2_Pin , OUTPUT); //LED 2

  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  if (digitalRead (MSensor_Pin) == HIGH ) {
    Cold_State_Counter = 0;
    Standby_State (Standby_Timer_ms);
  }
  else{
    ++Cold_State_Counter;
  }
  

  if (5 <= Cold_State_Counter) {
    Cold_State_Counter = 0;
    if (Sonic_1 (200) <= Sonic_1_Range) {
      if (Sonic_1 (200) <= Sonic_1_Range) {
        Standby_State (Standby_Timer_ms);
        Cold_State_Counter = 0;
      }
    }
    if (Sonic_2 (200) <= Sonic_2_Range) {
      if (Sonic_2 (200) <= Sonic_2_Range) {
        Hot_State (Hot_Timer_ms); //after HOT always to Standard
        Standard_State (Standard_Timer_ms); //can only exit by non activity timeout but always followed by STANDBY
        Standby_State (Standby_Timer_ms);
        Cold_State_Counter = 0;
      }
    }
    
  }
/*
  if (ledState == LOW) {
      ledState = HIGH;
  } else {
      ledState = LOW;
  }
*/
  delay(500);
  
  Blink(2000, previousMillis);
  Serial.print("Cold_State");
  Serial.println();
}

int Standby_State (unsigned long Timer) {
  unsigned long Time_Start = millis();
  unsigned long Time_Now = millis();
  RunTime = Time_Now - Time_Start;

  digitalWrite (LED_1_Pin, LOW); //Trun OFF the samll Light
  digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
  
  while (RunTime <= Timer) { 
    if (Sonic_1 (200) <= Sonic_1_Range) {
      if (Sonic_1 (200) <= Sonic_1_Range) {
        
        Standard_State (Standard_Timer_ms);
        //reset timer
        Time_Start = millis();
        Time_Now = Time_Start;
        RunTime = Time_Now - Time_Start;
      }
    }

    delay (50);
    
    if (Sonic_2 (200) <= Sonic_2_Range) {
      delay (100);
      if (Sonic_2 (200) <= Sonic_2_Range) {
        
        Hot_State (Hot_Timer_ms);
        Standard_State (Standard_Timer_ms);
        //reset timer
        Time_Start = millis();
        Time_Now = Time_Start;
        RunTime = Time_Now - Time_Start;
      }
    }

    Time_Now = millis();
    RunTime = Time_Now - Time_Start;
    
    Blink(100, previousMillis);
    Serial.print("Standby_State");
    Serial.println();
  }
  //only way to exit this loop is by over time limit

  //before jum out loop kill all lights
  digitalWrite (LED_1_Pin, LOW); //Trun OFF the samll Light
  digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
  return 0; //indicate overtime
  
}

int Standard_State (unsigned long Timer) {
  unsigned int Sonic_1_Counter = 0;
  unsigned long Time_Start = millis();
  unsigned long Time_Now = millis();
  RunTime = Time_Now - Time_Start;

  digitalWrite (LED_1_Pin, HIGH); //Trun ON the samll Light
  digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
  
  while (RunTime <= Timer) {
    if (Sonic_1 (200) <= Sonic_1_Range) {
      ++Sonic_1_Counter;
      if (3 <= Sonic_1_Counter) {
        Sonic_1_Counter = 0;
        //reset timer
        Time_Start = millis();
        Time_Now = Time_Start;
        RunTime = Time_Now - Time_Start;
      }
    }
    delay (50);
    if (Sonic_2 (200) <= Sonic_2_Range) {
      delay (100);
      if (Sonic_2 (200) <= Sonic_2_Range) {
        Hot_State (Hot_Timer_ms);
        //reset timer
        Time_Start = millis();
        Time_Now = Time_Start;
        RunTime = Time_Now - Time_Start;
      }
    }

    Time_Now = millis();
    RunTime = Time_Now - Time_Start;
    Blink(1000, previousMillis);
    Serial.print("Standard_State");
    Serial.println();
  }

  //if return from standard, the light should all be off
  digitalWrite (LED_1_Pin, LOW); //Trun OFF the samll Light
  digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
  return 0;
}

int Hot_State (unsigned long Timer) {
  unsigned int Sonic_1_Counter = 0;
  unsigned int Montion_Cycle = 0;
  bool Montion_Last_State = false;
  unsigned long Time_Start = millis();
  unsigned long Time_Now = Time_Start;
  RunTime = Time_Now - Time_Start;
  unsigned int TimeOut_Counter_min = 0;

  digitalWrite(ledPin, HIGH);
  digitalWrite (LED_1_Pin, HIGH); //Trun ON the samll Light
  digitalWrite (LED_2_Pin, HIGH); // Turn ON the main Light

  delay (1000);
  
  while (Sonic_2(200) <= Sonic_2_Range) {
    Time_Now = millis();
    RunTime = Time_Now - Time_Start;
    if (9000 <= RunTime) {
      digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
      return 1;
    }
    
  }
  
  while (TimeOut_Counter_min <= Hot_Timer) {
    if (Sonic_1 (200) <= Sonic_1_Range) {
      ++Sonic_1_Counter;
      if (3 <= Sonic_1_Counter) {
        Sonic_1_Counter = 0;
        //reset timer and mins counter
        Time_Start = millis();
        Time_Now = Time_Start;
        RunTime = Time_Now - Time_Start;
        TimeOut_Counter_min = 0;
      }
    }
    delay (50);
    if (Sonic_2 (200) <= Sonic_2_Range) {
      delay (10);
      if (Sonic_2 (200) <= Sonic_2_Range) {
        delay (10);
        if (Sonic_2 (200) <= Sonic_2_Range) {
          //Active back to standard state
          digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
          
          delay(1000);
          Time_Start = millis();
          while (Sonic_2(200) <= Sonic_2_Range) {
            Time_Now = millis();
            RunTime = Time_Now - Time_Start;
            if (9000 <= RunTime) return 1;
          }
          return 1;
        }
      }
    }

    /*if (digitalRead (MSensor_Pin) == HIGH) {
      Time_Start = millis();
      Time_Now = Time_Start;
      RunTime = Time_Now - Time_Start;
    }
    */
    
    if (Montion_Last_State) { // if last time the motion sensor state was true
      if (digitalRead (MSensor_Pin) == LOW) { //now it has a low, one on and off cycle complete
        Montion_Last_State = false;
        ++Montion_Cycle;
        if (3 <= Montion_Cycle) { //2 cycle reached
          Montion_Cycle = 0;
          Time_Start = millis();
          Time_Now = Time_Start;
          RunTime = Time_Now - Time_Start;
          TimeOut_Counter_min = 0;
          
        }
      }
    } else {
      if (digitalRead (MSensor_Pin) == HIGH) {
        Montion_Last_State = true;
      }
    }

    Time_Now = millis();
    RunTime = Time_Now - Time_Start;

    if (Timer <= RunTime) {
      ++TimeOut_Counter_min; // one minutes passed

      //reset one min
      Time_Start = millis();
      Time_Now = Time_Start;
      RunTime = Time_Now - Time_Start;
    }
    
    Serial.print("Hot_State");
    Serial.println();
  }
  //Timeout
  digitalWrite (LED_1_Pin, HIGH); //Trun ON the samll Light
  digitalWrite (LED_2_Pin, LOW); // Turn OFF the main Light
  return 0;
}

void Blink (long interval, unsigned long& previousMillis) {
  currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  return;
}

float Sonic_1 (float Max_Range) { //Pin 6

  
  long duration, cm;

  //activate the sonic speaker
  pinMode(Sonic_1_Pin, OUTPUT);
  digitalWrite(Sonic_1_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Sonic_1_Pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(Sonic_1_Pin, LOW);
  
  //switch mic and listening echo
  pinMode(Sonic_1_Pin, INPUT);
  duration = pulseIn(Sonic_1_Pin, HIGH);

  cm = min (Max_Range, duration / 29 / 2);

  return cm;
}

float Sonic_2 (float Max_Range) { //Pin 7

  
  long duration, cm;

  //activate the sonic speaker
  pinMode(Sonic_2_Pin, OUTPUT);
  digitalWrite(Sonic_2_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Sonic_2_Pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(Sonic_2_Pin, LOW);
  
  //switch mic and listening echo
  pinMode(Sonic_2_Pin, INPUT);
  duration = pulseIn(Sonic_2_Pin, HIGH);

  cm = min (Max_Range, duration / 29 / 2);

  return cm;
}

/* Automation for four Autiotecnica AT-LP60-USB stereo turntables
 * Four presence sensors trigger the activation of four relays:
 * each relay activates a 24V solenoid inside the turntable;
 * the solenoids start the playback on the four turntables.
 * Arduino senses the voltage applied to the turntable engine
 * and if it is not zero (machine stopped) activates the
 * presence sensors, otherwise it ignore them.
**/

/* Alberto Gaetti alberto@gaetti.it www.gaetti.it
 * Firenze, 20150618
 * CC BY-NC-SA 3.0 IT
**/


const int ReadyLED = 8;        // set ReadyLED to pin #8
const int PresenceLED = 9;     // set PresenceLED to pin #9
const int TIME = 500;          // timelapse

int PIR[] = {10, 11, 2, 3};    // PIR to pins 10, 11, 3, 4
int RELAY[] = {7, 6, 5, 4};    // RELAYS to pins 4,5,6,7 (RelayShield default)
float ENGINE[] = {0,0,0,0};    // Engine voltage 0 = engine running; !0 = engine stop

int i = 0;
int j = 0;
boolean engineStop = 1;
int PIRCounter = 0;               // counter for the number of activations of the PIRs
int PIRState[] = {0,0,0,0,0};     // current state of the PIRs; PIRState[4] contains the boolean OR
int lastPIRState = 0;

void setup() {
  pinMode(ReadyLED,OUTPUT);             // initialize ReadyLED pin
  pinMode(PresenceLED,OUTPUT);          // initialize PresenceLED pin
  for (i=0;i<4;i++) {              
    pinMode(RELAY[i],OUTPUT);           // initialize RELAY pins
    pinMode(PIR[i],INPUT);              // initialize PIR pins
  }
  digitalWrite(PresenceLED, LOW);
  digitalWrite(ReadyLED, LOW);
  Serial.begin(9600);                   // serial communication for debugging
}


void loop() {
  PIRState[4] = 0;
  for (i=0;i<4;i++) {              
    PIRState[i] = digitalRead(PIR[i]);              // read PIR state
    PIRState[4] = (PIRState[i] || PIRState[4]);     // if nay of the PIR is High PIRState[4] is HIGH
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(PIRState[i]);
    Serial.print(" | ");
  }
  
  Serial.println("");
  Serial.println(PIRState[4]);
  
  if (PIRState[4]) {
    digitalWrite(PresenceLED, HIGH);
  }
  
  for (i=0;i<4;i++) {
    j = i+1;
    ENGINE[i] = analogRead(j);                       // read tension from turntable engines
    engineStop = (engineStop && ENGINE[i]);          // if any and all engines are stopped engineStop = 1
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(ENGINE[i]*0.00488);
    Serial.print(" | ");
    
  } 
  
  Serial.println("");
  Serial.println(engineStop);
  
  if (engineStop) {                                    //if any of the four engines is running engineStop = 0
    digitalWrite(ReadyLED, HIGH);
    if (PIRState[4] != lastPIRState) {
      if (PIRState[4]) {
        for (i=0;i<4;i++) {
          digitalWrite(RELAY[i], HIGH);    //switch the ralys ON
        }
        delay(TIME*2);
        for (i=0;i<4;i++) {
          digitalWrite(RELAY[i], LOW);    //switch the ralys OFF
        }
      }
    }
  
    lastPIRState = PIRState[4];
    delay(TIME*2);
    
  } else {
    engineStop = !engineStop;
    digitalWrite(ReadyLED, HIGH);
    delay(TIME);
    digitalWrite(ReadyLED, LOW);
    delay(TIME);
  }

  digitalWrite(PresenceLED, LOW);
  digitalWrite(ReadyLED, LOW);
  Serial.println("");
  Serial.println("================");
}


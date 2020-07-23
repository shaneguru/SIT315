
const int motionSensor = 2;              // the pin that the sensor is atteched to
const int buzzer = 7;
const int tiltSensor = 3;
int led = 13;

uint8_t motionState = LOW;
uint8_t tiltState = LOW;


void setup() {  
  
  pinMode(buzzer, OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(tiltSensor, INPUT_PULLUP);
  pinMode(motionSensor, INPUT);    // initialize sensor as an input
  
  Serial.begin(9600);        // initialize serial
  
  attachInterrupt(digitalPinToInterrupt(motionSensor), motion, HIGH);
  attachInterrupt(digitalPinToInterrupt(tiltSensor), tilt, HIGH);

}

void loop(){
  
  delay(200);
  
}

void motion(){
  motionState = !motionState;
  Serial.println("Motion detected!");
  digitalWrite(buzzer, motionState);
  Serial.println("Buzzed!");

}

void tilt(){
  tiltState = !tiltState;
  Serial.println("Tilted!");
  digitalWrite(led, tiltState);
  Serial.println("Blinked!");
  
}
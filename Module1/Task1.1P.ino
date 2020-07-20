int sensor = 2;              // the pin that the sensor is atteched to
const int buzzer = 7;  // the pin that the buzzer is atteched to

void setup() {  
  pinMode(buzzer, OUTPUT);
  pinMode(sensor, INPUT);    // initialize sensor as an input
  Serial.begin(9600);        // initialize serial
  attachInterrupt(digitalPinToInterrupt(sensor), motion, RISING); //Fires motion when sensor rises from 0 to 1
}

void loop(){
  delay(200);
}

void motion(){
  Serial.println("Motion detected!");
  digitalWrite(buzzer, HIGH);
  noTone(buzzer);     // Stop sound...
  Serial.println("Buzzed!");

}

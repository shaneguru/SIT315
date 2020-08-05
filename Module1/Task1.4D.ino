
const int motionSensor = 2;              // the pin that the sensor is atteched to
const int buzzer = 7;
const int tiltSensor = 3;
const int motionSensor2 = 4;

int led = 13;
int ledBlue = 11;
const int ledGreen = PB4;

uint8_t motionState,tiltState,motionState2 = 0;

uint8_t premotion, premotion1, premotion2 = 0;

const uint16_t timer = 0; // counter +
const uint16_t compare = 31250; // compare thresholds

void setup(){  
  
  Serial.begin(9600);        // initialize serial

  pinMode(buzzer, OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(ledBlue,OUTPUT);
  pinMode(tiltSensor, INPUT_PULLUP);
  pinMode(motionSensor, INPUT);    // initialize sensor as an input
  pinMode(motionSensor2, INPUT);    // initialize sensor as an input
  DDRB |= (1 << ledGreen);
  
  PCICR |= 0b00000111;
  PCMSK2 |= 0b10011100;
  
  TCCR1A = 0;
  TCCR1B |= (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);
  
  TCNT1 = timer;
  OCR1A = compare;
  
  TIMSK1 = (1 << OCIE1A);
  // Enable interrupts
  sei();

}

//infinite loop
void loop(){
  if(premotion != motionState ){
    digitalWrite(led,motionState);
    Serial.println("Motion Sensor 1 Interrupt - Turned Red LED");
    premotion =  motionState;
  }
  
  if(premotion1 != motionState2){
    digitalWrite(ledBlue,motionState2);
    Serial.println("Motion Sensor 2 Interrupt - Turned blue LED");
    premotion1 =  motionState2;
  }

  if(premotion2 != tiltState){
    digitalWrite(buzzer,tiltState);
    Serial.println("Tilt sensor Interrupt - Buzzed");
    premotion2 =  tiltState;
  }
  
  delay(200);
}

ISR(PCINT2_vect){
  motionState = PIND & B00010000;
  motionState2 = PIND & B00000100;
  tiltState = PIND & B00001000;
}

ISR(TIMER1_COMPA_vect){
  TCNT1 = timer;
  PORTB ^= (1 << ledGreen);
}
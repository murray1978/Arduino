
long count = 0;
long countPerMinute = 0;

void setup()
{
  Serial.begin(9600);
  //16Mhz cpu
  setPwmFrequency(3,64);
  //pin3
  analogWrite( 3, 180 );
  //Pin2, digital input
  attachInterrupt( 0, countPulse, FALLING );
}

void loop()
{
    delay(10000);
    countPerMinute = 6 * count;
    Serial.println(countPerMinute, DEC);
    count = 0;
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;   //8Mhz
      case 64: mode = 0x03; break;  //16Mhz
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;    //8Mhz
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;  //16Mhz
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
void countPulse(){
  detachInterrupt(0);
  count++; 
  while(digitalRead(2)==0){
  }
  attachInterrupt(0,countPulse,FALLING);
}

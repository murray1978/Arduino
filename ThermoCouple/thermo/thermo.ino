void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

int raw;

void loop() {
  raw = (int)analogRead(1);
  float value = (float)raw * (float)(5.0/1024);
  // put your main code here, to run repeatedly:
  Serial.println((value * 100.0));
  delay(1000);
 
}

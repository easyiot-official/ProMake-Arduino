#define RLY1 26

void setup() {
  pinMode(RLY1,OUTPUT);
}

void loop() {
  digitalWrite(RLY1,HIGH);
  delay(1000);
  digitalWrite(RLY1,LOW);
  delay(1000);
}


void setup()
{
  pinMode(7, OUTPUT);      // sets the digital pin 13 as output
  pinMode(A1, INPUT);      // sets the digital pin 7 as input
}

void loop()
{
  if (digitalRead(A1) == 0) {
    digitalWrite(7, HIGH);
    delay(2000);
    digitalWrite(7,LOW);
    delay(1000);
  } else {
    digitalWrite(7, LOW);
  }
}

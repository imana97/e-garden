
void setup()
{
  pinMode(8, OUTPUT);      // sets the digital pin 13 as output
  pinMode(9,OUTPUT);
}

void loop()
{
    digitalWrite(8,HIGH);
    for (int i=255;i<256;i--){
      analogWrite(9,i);
      delay(20);
      
      }
}

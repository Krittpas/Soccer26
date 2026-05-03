int ledPin = PB0; // Connect ZX-LED at PB0/16 pin
int inPin = PA6;  // Connect ZX-BUTTON at PA6/6 pin
int val = 0;
void setup()
{
  pinMode(ledPin, OUTPUT); // Set PB0 as output
  pinMode(inPin, INPUT);   // Set PA6 as input
}
void loop()
{
  val = digitalRead(inPin); // Read status from PA6 
  digitalWrite(ledPin,val); // Write status to PB0
}

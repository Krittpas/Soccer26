int ledPin = PC13; //  On-board LED at PC13 pin
void setup()
{
  pinMode(ledPin, OUTPUT); // Set PC13 as output
}
void loop()
{
  digitalWrite(ledPin, HIGH); // Turn-on LED
  delay(500); // Delay 0.5 second 
  digitalWrite(ledPin, LOW); // Turn-off LED   
  delay(500); // Delay 0.5 second 
}

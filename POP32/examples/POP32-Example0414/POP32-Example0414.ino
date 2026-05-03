int ledPin = PB0;             // Connect ZX-LED at PB0/16 port
void setup()
{
  pinMode(ledPin, OUTPUT);    // Set PB0/16 as output
}
void loop()
{
  digitalWrite(ledPin, HIGH); // Turn-on LED
  delay(1000);                // Delay 1 second 
  digitalWrite(ledPin, LOW);  // Turn-off LED   
  delay(1000);                // Delay 1 second 
}

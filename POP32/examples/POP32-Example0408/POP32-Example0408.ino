int ledPin = PB0;     // Connect ZX-LED at PB0/16 port
int analogPin = A0;   // Connect ZX-POT at A0/0 port
int val = 0;          // Set variable
int threshold = 512;  // Set threshold point
void setup()
{
  pinMode(ledPin, OUTPUT); // Set PB0/16 port as output
}
void loop()
{
  val = analogRead(analogPin);  // Read ADC from A0/0 port
  if (val >= threshold)         // If ADC value equal or more than the threshold point, 
  {                             // condition is true.
    digitalWrite(ledPin, HIGH); // Turn-on LED 
  }
  else                          // If not, condition is false. 
  {
    digitalWrite(ledPin, LOW);  // Turn-off LED
  }
}

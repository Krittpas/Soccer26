int ledPin = PB0;     // Connect ZX-LED at PB0/16 port
int analogPin = A0;   // Connect ZX-POT at A0/0 port
int val = 0;          // Set variable
void setup()
{
  pinMode(ledPin, OUTPUT); // Set PB0/16 port as output
}
void loop()
{
  val = analogRead(analogPin);  // Read ADC from A0/0 port
  analogWrite(ledPin, val/4);   // Convert ADC to byte data 0-255 
                                // Send PWM to drive LED
}

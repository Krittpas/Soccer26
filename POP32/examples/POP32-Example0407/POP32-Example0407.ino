int analogValue = 0;  // Set variable
void setup()
{
  Serial.begin(9600); // Enable serial port. Set baudrate as 9600bps 
}
void loop()
{
  analogValue = analogRead(0);    // Read analog port 0
                                  // Connect the ZX-POT at the A0 port
  // Show result with more formats at Serial monitor
  Serial.println(analogValue);      // Show as Decimal
  Serial.println(analogValue, DEC); // Show as Decimal
  Serial.println(analogValue, HEX); // Show as Hexadecimal
  Serial.println(analogValue, OCT); // Show as Octal
  Serial.println(analogValue, BIN); // Show as Binary
  Serial.write(analogValue);    // Show ADC value from divided by 4 for byte variable
  Serial.println();               // New line 
  Serial.flush();
  delay(500);                      // Delay 10ms 
}

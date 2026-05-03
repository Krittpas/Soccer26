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
  Serial.print(analogValue);      // Show as Decimal
  Serial.print("\t");             // Separated by tab
  Serial.print(analogValue, DEC); // Show as Decimal
  Serial.print("\t");             // Separated by tab
  Serial.print(analogValue, HEX); // Show as Hexadecimal
  Serial.print("\t");             // Separated by tab
  Serial.print(analogValue, OCT); // Show as Octal
  Serial.print("\t");             // Separated by tab
  Serial.print(analogValue, BIN); // Show as Binary
  Serial.print("\t");             // Separated by tab
  Serial.write(analogValue/4);    // Show ADC value from divided by 4 for byte variable
  Serial.print("\t");             // Separated by tab
  Serial.println();               // New line 
  delay(10);                      // Delay 10ms 
}

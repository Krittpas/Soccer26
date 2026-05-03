int incomingByte = 0;
void setup()
{
  Serial.begin(9600); // Enable serial communication, 9600bps baudrate 
}
void loop() 
{
  if (Serial.available() > 0) // Read serial data 
  {
    incomingByte = Serial.read(); // Store data from serial data reading
    Serial.print("I received: "); // Show message at Serial terminal
    Serial.println(incomingByte, DEC); // Show the received data at Serial Monitor
  }
}

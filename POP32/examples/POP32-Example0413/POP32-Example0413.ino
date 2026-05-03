long time;
void setup()
{
  Serial.begin(9600);
}
void loop()
{
  Serial.print("Time: ");
  time = millis();        // Read time
  Serial.println(time);   // Show time value
  Serial.flush();
  delay(1000);
}

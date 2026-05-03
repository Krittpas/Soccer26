int pin = PB7;
unsigned long duration;
void setup()
{
  pinMode(pin, INPUT);
}
void loop()
{
  duration = pulseIn(pin, HIGH);
}

int outputpin = 36;
//this sets the ground pin to LOW and the input voltage pin to high
void setup() {
  Serial.begin(9600);
}

void loop()       //main loop

{
  int analogValue = analogRead(outputpin);
  float millivolts = (analogValue / 2048.0) * 3100; //3300 is the voltage provided by NodeMCU
  float celsius = millivolts / 10;
  Serial.println(celsius);
  delay(10000);
}

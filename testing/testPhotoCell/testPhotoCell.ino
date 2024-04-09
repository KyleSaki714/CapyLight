const int LEDpin = 9;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LEDpin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int lightVal = analogRead(A0);
  Serial.print(lightVal);
  Serial.print(" ");

  float inverse = 1.0 - (lightVal / 1023.0);
  Serial.print(inverse);
  Serial.print(" ");

  int lightStrength = inverse * 255;
  analogWrite(LEDpin, lightStrength);

  Serial.println(lightStrength);

  delay(500);
}

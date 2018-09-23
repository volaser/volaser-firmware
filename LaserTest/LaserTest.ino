void setup() {
  Serial.begin(115200);
  Serial.print('hello');
}

void loop() {
  delay(1000);
  Serial.print('hello again');
}

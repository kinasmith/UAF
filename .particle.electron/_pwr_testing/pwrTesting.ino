/*SYSTEM_MODE(MANUAL);*/

int led = D7;
int btn_pin = C0;

void setup() {
  Serial.begin(9600);

  pinMode(led, OUTPUT);
  for(int i = 0; i < 10; i++) {
    blink(led, 100);
  }
  Serial.println("Setup is Done");
}

void loop() {
  if(btn.clicks == 1) {
    Serial.println("button has been pressed, connecting");
    /*Particle.connect();*/
  }
  if(Particle.connected()) {
    Serial.println("particle connected");
    /*Particle.process();*/
    blink(led, 1000);
  }
}

void blink(int l, int t) {
  digitalWrite(l, HIGH);
  delay(t);
  digitalWrite(l, LOW);
  delay(t);
}

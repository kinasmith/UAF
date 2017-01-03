int led1 = 5;
int led2 = 7;

void setup() {
	Serial.begin(9600);
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
}

void loop() {
	Serial.println("ON");
	digitalWrite(led1, HIGH);
	digitalWrite(led2, HIGH);

	// We'll leave it on for 1 second...
	delay(1000);
	Serial.println("OFF");
	// Then we'll turn it off...
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);

	// Wait 1 second...
	delay(1000);

	// And repeat!
}

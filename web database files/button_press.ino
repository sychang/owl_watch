int pin1 = 1;
volatile int state = LOW;

long lastDebounceTime = 0;
long debounceDelay = 300;
int lastButton = LOW;
int buttonState;

long lastButtonPress = 0;

void setup() {
    pinMode(pin1, INPUT);
    attachInterrupt(digitalPinToInterrupt(pin1), buttonPressed, RISING);
}

void loop() {
}

void buttonPressed() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
        long pressed = millis();
        // do something
	lastButtonPress = pressed;
        lastDebounceTime = millis();
  }
}

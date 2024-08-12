const int pinA0 = A0;
const int pinA1 = 8;
const int pin13 = 13;
const int pin14 = 14;

void setup() {
  Serial.begin(115200);
  pinMode(pinA0, INPUT);
  pinMode(pinA1, INPUT);
  pinMode(pin13, OUTPUT);
  pinMode(pin14, OUTPUT);
}

void loop() {
  // Read the values of A0 and A1 pins
  int valueA0 = digitalRead(pinA0);
  int valueA1 = digitalRead(pinA1);

  // Check if either A0 or A1 has a value of 1
  if (valueA0 == 1 || valueA1 == 1) {
    // Set the values of pins 13 and 14 to HIGH
    digitalWrite(pin13, HIGH);
    digitalWrite(pin14, HIGH);

    // Trigger ESP32-CAM to run email sending code
    Serial.println("RUN_EMAIL_SENDING");

    // Additional actions can be performed here based on the condition
  } else {
    // Reset pins 13 and 14 to LOW if both A0 and A1 are 0
    digitalWrite(pin13, LOW);
    digitalWrite(pin14, LOW);
  }

  // Add other functionalities as needed

  delay(1000); // Adjust delay as needed
}

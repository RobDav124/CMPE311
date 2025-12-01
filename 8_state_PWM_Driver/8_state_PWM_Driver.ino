const int button = 2;
const int pwm = 6;

int buttonState = LOW;
int lastButtonState = LOW;

int state_count = 0;
const int maxDuty = 255;
bool decend = false;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(pwm, OUTPUT);
  pinMode(button, INPUT);
  analogWrite(pwm, 0); 
}

void loop() {
  int buttonRead = digitalRead(button);
  
  Serial.print("Button Read: ");
  Serial.print(buttonRead);
  Serial.print(" | Button State: ");
  Serial.print(buttonState);
  Serial.print(" | State Count: ");
  Serial.println(state_count);

  if (buttonRead != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonRead != buttonState) {
      buttonState = buttonRead;
      
      // Only increment on button PRESS (transition from LOW to HIGH)
      if (buttonState == HIGH) {
        int duty;
        if (decend == false) {
          state_count = state_count + 2;
          if (state_count >= 8) { 
            state_count = 8;
            decend = true; 
          }
        }
        else {
          state_count = state_count - 2;
          if (state_count <= 0) { 
            state_count = 0;
            decend = false; 
          }
        }
        
        duty = (maxDuty * state_count) / 8;
        analogWrite(pwm, duty);
        
      }
    }
  }

  lastButtonState = buttonRead;
  delay(100);
}
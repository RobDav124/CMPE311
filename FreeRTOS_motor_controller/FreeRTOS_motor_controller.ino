#include <Arduino_FreeRTOS.h>

// Pin definitions
const int button = 2;
const int pwm = 6;

// Button state tracking variables
int buttonState = LOW;
int lastButtonState = LOW;

// Motor speed state variables
int state_count = 0;
const int maxDuty = 255;
bool decend = false;

// Debounce timing variables
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Task function prototypes
void TaskButtonDebounce(void *pvParameters);
void TaskMotorControl(void *pvParameters);

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(pwm, OUTPUT);
  pinMode(button, INPUT);
  analogWrite(pwm, 0);
  
  // Create FreeRTOS tasks
  xTaskCreate(
    TaskButtonDebounce,           // Task function
    "ButtonDebounce",             // Task name (for debugging)
    128,                          // Stack size (words)
    NULL,                         // Task parameters
    1,                            // Task priority
    NULL                          // Task handle
  );
  
  xTaskCreate(
    TaskMotorControl,             // Task function
    "MotorControl",               // Task name (for debugging)
    128,                          // Stack size (words)
    NULL,                         // Task parameters
    1,                            // Task priority
    NULL                          // Task handle
  );
  
  // Start the FreeRTOS scheduler (tasks begin executing)
  vTaskStartScheduler();
}

void loop() {
  // Empty loop - FreeRTOS scheduler takes over
  // This should never be reached
}

// Task: Button debounce and state management
void TaskButtonDebounce(void *pvParameters) {
  (void) pvParameters;  // Unused parameter
  
  // Task loop runs indefinitely
  for (;;) {
    // Read current button state
    int buttonRead = digitalRead(button);
    
    // Debug output to serial monitor
    Serial.print("Button Read: ");
    Serial.print(buttonRead);
    Serial.print(" | Button State: ");
    Serial.print(buttonState);
    Serial.print(" | State Count: ");
    Serial.println(state_count);
    
    // Check if button reading has changed (potential bounce or real press)
    if (buttonRead != lastButtonState) {
      lastDebounceTime = millis();  // Reset debounce timer
    }
    
    // Check if enough time has passed since last state change (debounce complete)
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // Check if the stable reading is different from current button state
      if (buttonRead != buttonState) {
        buttonState = buttonRead;  // Update button state to new stable value
        
        // Only update motor speed on button PRESS (transition from LOW to HIGH)
        if (buttonState == HIGH) {
          // Ascending phase: increase speed from 0 to 8
          if (decend == false) {
            state_count = state_count + 2;  // Increment speed state by 2
            if (state_count >= 8) {         // Check if maximum reached
              state_count = 8;              // Cap at maximum
              decend = true;                // Switch to descending mode
            }
          }
          // Descending phase: decrease speed from 8 to 0
          else {
            state_count = state_count - 2;  // Decrement speed state by 2
            if (state_count <= 0) {         // Check if minimum reached
              state_count = 0;              // Cap at minimum
              decend = false;               // Switch back to ascending mode
            }
          }
        }
      }
    }
    
    // Save current reading for next iteration
    lastButtonState = buttonRead;
    
    // FreeRTOS delay - yields to other tasks
    vTaskDelay(100 / portTICK_PERIOD_MS);  // 100ms delay
  }
}

// Task: Motor PWM control
void TaskMotorControl(void *pvParameters) {
  (void) pvParameters;  // Unused parameter
  
  // Task loop runs indefinitely
  for (;;) {
    // Calculate PWM duty cycle based on state_count
    // Maps state_count (0-8) to duty cycle (0-255)
    int duty = (maxDuty * state_count) / 8;
    
    // Apply the calculated duty cycle to the motor
    analogWrite(pwm, duty);
    
    // FreeRTOS delay - yields to other tasks
    vTaskDelay(10 / portTICK_PERIOD_MS);  // 10ms update rate
  }
}
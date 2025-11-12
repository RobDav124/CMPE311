// Pin configuration
const int led1 = 12;
const int led2 = 10;

// LED structure
struct LED_STATE {
  int led;
  int interval;
  bool on;
  unsigned long prevMillis;
  int currState;
};

// Instances
LED_STATE LED1 = {led1, 0, false, 0, LOW};
LED_STATE LED2 = {led2, 0, false, 0, LOW};

//TASK DEFINITIONS 
void task_blink_LED1() { blink(LED1); }
void task_blink_LED2() { blink(LED2); }
void task_serial_input();

// Task function pointer type
typedef void (*TaskFunction)();

// Function pointer array (Cyclic Executive Task List)
TaskFunction taskList[] = {
  task_blink_LED1,
  task_blink_LED2,
  task_serial_input
};

const int NUM_TASKS = sizeof(taskList) / sizeof(TaskFunction); //3 tasks 

//LED blink function 
void blink(LED_STATE &ledState) {
  if (ledState.on && ledState.interval > 0) {
    unsigned long currMillis = millis();
    if (currMillis - ledState.prevMillis >= (unsigned long)ledState.interval) {
      ledState.prevMillis = currMillis;
      ledState.currState = !ledState.currState;
      digitalWrite(ledState.led, ledState.currState);
    }
  }
}

//Input States
enum InputState { READ_LED, READ_INTERVAL };
InputState inputState = READ_LED;
int led_choice = 0;
int interval_choice = 0;

void task_serial_input() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      switch (inputState) {
        case READ_LED:
          led_choice = input.toInt();
          Serial.println("What is the blink rate (in msec)?");
          inputState = READ_INTERVAL;
          break;

        case READ_INTERVAL:
          interval_choice = input.toInt() / 2;

          if (led_choice == 1) {
            LED1.on = true;
            LED1.interval = interval_choice;
          } else if (led_choice == 2) {
            LED2.on = true;
            LED2.interval = interval_choice;
          }

          Serial.println("What LED? (1 or 2)");
          inputState = READ_LED;
          break;
      }
    }
  }
}

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.begin(9600);
  Serial.println("What LED? (1 or 2)");
}

void loop() {
  for (int i = 0; i < NUM_TASKS; i++) {
    taskList[i](); // Call each task (task manager)
  }

  delay(1); // Minor cycle time
}


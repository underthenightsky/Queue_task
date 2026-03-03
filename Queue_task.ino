#define LED_BUILTIN 2
QueueHandle_t Queue1;
QueueHandle_t Queue2;

void TaskA(void *pvParameters) {
  int aSend;
  int aRecieve;
  while (1) {
    while (Serial.available() > 0) {
      aSend = Serial.parseInt();
      Serial.print("aSend values");
      Serial.println(aSend);

      if (aSend > 0) {
        Serial.print("Task A recieved from User: ");
        Serial.println(aSend);
        xQueueSend(Queue1, &aSend, 10 / portTICK_PERIOD_MS);
        vTaskDelay(1000);
      }
    }
    BaseType_t xRecieveStatus = xQueueReceive(Queue2, &aRecieve, 10 / portTICK_PERIOD_MS);
    if (xRecieveStatus == pdPASS) {
      Serial.print("Task A recieved value from Queue 2:");
      Serial.println(aRecieve);
    }
  }
}
void TaskB(void *pvParameters) {
  char bSend[] = "Blinked";
  int bRecieve = 500;  // default delay
  int num_blinks = 0;
  for (int i = 1;; i++) {
    if (num_blinks % 100 == 0 && num_blinks > 0) {
      Serial.print("Task B sent  value to Queue 2:");
      Serial.println(bSend);
      xQueueSend(Queue2, &num_blinks, 10 / portTICK_PERIOD_MS);
    }

    BaseType_t xRecieveStatus = xQueueReceive(Queue1, &bRecieve, 10 / portTICK_PERIOD_MS);
    if (xRecieveStatus == pdPASS) {
      Serial.print("Task B got  value from Queue 1:");
      Serial.println(bRecieve);
    }
    //Always keep blinking , dont wait for new value in Q2 to blink once
    num_blinks++;
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(bRecieve / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(bRecieve / portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN, OUTPUT);
  Queue1 = xQueueCreate(5, sizeof(int32_t));
  Queue2 = xQueueCreate(5, sizeof(int32_t));

  Serial.begin(300);
  vTaskDelay(1000);
  Serial.println("Enter a value:");
  xTaskCreate(TaskA, "TaskA", 1024, NULL, 1, NULL);
  xTaskCreate(TaskB, "TaskB", 1024, NULL, 1, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
// Mistakes i was making : 
// creating tasks in loop not setup 
// had to set the esp32 to blink outside the pdRevcieve status block, allowing the esp32 to blink always and not just when there is a new addition to queue 1 (for 1 cycle)
// Also updated delays, sending or receving from queues dont need dealys, but we shou,d add delays to 
// while checking for serial input(Task A), Task B anyway delays when blinking 
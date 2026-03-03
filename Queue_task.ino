#define LED_BUILTIN 2
QueueHandle_t Queue1;
QueueHandle_t Queue2;
void setup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN, OUTPUT);
  Queue1 = xQueueCreate(5, sizeof(int32_t));
  Queue2 = xQueueCreate(5, sizeof(int32_t));

  Serial.begin(300);
  vTaskDelay(1000);
  Serial.println("Enter a value:");
}
void TaskA(void *pvParameters) {
  int aSend;
  int aRecieve;
  while (1) {
    while (Serial.available()>0) {
      aSend =aSend*10+ Serial.parseInt();
      Serial.print("aSend values");
      Serial.println(aSend);
    }
    if (aSend > 0) {
      Serial.print("Task A recieved from User: ");
      Serial.println(aSend);
      xQueueSend(Queue1, &aSend, 10 / portTICK_PERIOD_MS);
      vTaskDelay(1000);
    }

    BaseType_t xRecieveStatus = xQueueReceive(Queue2, &aRecieve, portMAX_DELAY);
    if (xRecieveStatus == pdPASS) {
      Serial.print("Task A recieved value from Queue 2:");
      Serial.println(aRecieve);
    }
    vTaskDelay(1000);
  }
}
void TaskB(void *pvParameters) {
  char bSend[] = "Blinked";
  int bRecieve;
  for (int i = 1;; i++) {
    if (i % 100 == 0 && i > 0) {
      Serial.print("Task B sent  value to Queue 2:");
      Serial.println(bSend);
      xQueueSend(Queue2, &bSend, 10 / portTICK_PERIOD_MS);
      vTaskDelay(1000);
      i = 1;
    }

    BaseType_t xRecieveStatus = xQueueReceive(Queue1, &bRecieve, 10 / portMAX_DELAY);
    if (xRecieveStatus == pdPASS) {
      Serial.print("Task B got  value from Queue 1:");
      Serial.println(bRecieve);
      digitalWrite(LED_BUILTIN, HIGH);
      vTaskDelay(bRecieve / portTICK_PERIOD_MS);
      digitalWrite(LED_BUILTIN, LOW);
      vTaskDelay(bRecieve / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  xTaskCreate(TaskA, "TaskA", 1024, NULL, 1, NULL);
  xTaskCreate(TaskB, "TaskB", 1024, NULL, 1, NULL);
}

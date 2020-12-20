#include <painlessMesh.h>
// #include "stdio.h"

#define MESH_PREFIX "MeshSSID"
#define MESH_PASSWORD "MeshPass"
#define MESH_PORT 5555

//#define _LCL_DEBUG_

// Variables
painlessMesh mesh;
uint16_t onConnectBlinkInterval = 2000;
uint16_t onDisconnectBlinkInterval = 500;

// Funtion prototypes
void onNewConnection(uint32_t nodeId);
void onDroppedConnection(uint32_t nodeId);
void onReceive(uint32_t id, String& msg);

// Task function prototypes
void tcb_blinkLED();
void tcb_sendSensorValue();

Scheduler ts;

Task tBlinkLED(500, TASK_FOREVER, &tcb_blinkLED, &ts);
Task tSendSensorValue(TASK_SECOND * 1, TASK_FOREVER, &tcb_sendSensorValue, &ts);

/************************* setup() *************************/
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &ts, MESH_PORT, WIFI_AP_STA, 6);
  mesh.setContainsRoot(true);

  mesh.onNewConnection(&onNewConnection);
  mesh.onDroppedConnection(&onDroppedConnection);

  tBlinkLED.enable();
  tSendSensorValue.enable();
}

/************************* loop() *************************/
void loop() {
  // it will run the user scheduler as well
  mesh.update();
  ts.execute();
}

void onNewConnection(uint32_t nodeId) {
#if defined(_LCL_DEBUG_)
  Serial.println("\nonNewConnection(): ENTER");
  Serial.printf("New connection: %u\n", nodeId);
#endif

  if (tBlinkLED.getInterval() != onConnectBlinkInterval) {
    tBlinkLED.setInterval(onConnectBlinkInterval);
  }

#if defined(_LCL_DEBUG_)
  Serial.println("onNewConnection(): EXIT\n");
#endif
}

void onDroppedConnection(uint32_t nodeId) {
#if defined(_LCL_DEBUG_)
  Serial.println("\nDroppedConnection(): ENTER");
  Serial.printf("Connection dropped: %u\n", nodeId);
#endif
  
  if (tBlinkLED.getInterval() != onDisconnectBlinkInterval) {
    tBlinkLED.setInterval(onDisconnectBlinkInterval);
  }

#if defined(_LCL_DEBUG_)
  Serial.println("onDroppedConnection(): EXIT\n");
#endif
}

/***************** Task callback functions *****************/

void tcb_blinkLED() { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }

void tcb_sendSensorValue() {
#if defined(_LCL_DEBUG_)
  Serial.println("\ntcb_sendSensorValue(): ENTER");
#endif

  uint16_t sensorIn = analogRead(A0);
  mesh.sendBroadcast(String(sensorIn));

#if defined(_LCL_DEBUG_)
  Serial.println("tcb_sendSensorValue(): EXIT\n");
#endif
}

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

// WiFi
const char *ssid = "ESP32 Firmware";
const char *password = "ffffffff";

// WebSockets
WebSocketsServer webSocketsServer(81);

// Motor Pins
const int L1 = 18;
const int L2 = 19;
const int R1 = 21;
const int R2 = 22;

// Command
char command = 'S';

// Prototypes
void webSocketsServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void applyCommand(char cmd);

void moveForward();
void moveBack();
void moveLeft();
void moveRight();
void stopMoving();

// Setup
void setup()
{
  Serial.begin(115200);

  IPAddress local_ip(192, 168, 20, 1);
  IPAddress gateway(192, 168, 20, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.softAPIP());

  webSocketsServer.begin();
  webSocketsServer.onEvent(webSocketsServerEvent);

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  stopMoving();
}

// Loop
void loop()
{
  webSocketsServer.loop();

  applyCommand(command);
}

// WebSockets Server Event
void webSocketsServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_TEXT)
  {
    String msg = String((char *)payload);

    Serial.print("CMD: ");
    Serial.println(msg);

    if (msg == "F")
      command = 'F';
    else if (msg == "B")
      command = 'B';
    else if (msg == "L")
      command = 'L';
    else if (msg == "R")
      command = 'R';
    else
      command = 'S';
  }
}

// Motor Control Router
void applyCommand(char cmd)
{
  switch (cmd)
  {
  case 'F':
    moveForward();
    break;
  case 'B':
    moveBack();
    break;
  case 'L':
    moveLeft();
    break;
  case 'R':
    moveRight();
    break;
  default:
    stopMoving();
    break;
  }
}

// Motors
void moveForward()
{
  digitalWrite(L1, LOW);
  digitalWrite(L2, HIGH);
  digitalWrite(R1, LOW);
  digitalWrite(R2, HIGH);
}

void moveBack()
{
  digitalWrite(L1, HIGH);
  digitalWrite(L2, LOW);
  digitalWrite(R1, HIGH);
  digitalWrite(R2, LOW);
}

void moveLeft()
{
  digitalWrite(L1, HIGH);
  digitalWrite(L2, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, HIGH);
}

void moveRight()
{
  digitalWrite(L1, LOW);
  digitalWrite(L2, HIGH);
  digitalWrite(R1, HIGH);
  digitalWrite(R2, LOW);
}

void stopMoving()
{
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
}
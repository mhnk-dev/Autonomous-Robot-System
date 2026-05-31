#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

// WiFi
const char *ssid = "ESP32 Firmware";
const char *password = "ffffffff";

// WebSockets
WebSocketsServer webSocketsServer(81);

// Motor A (Left)
const int AIN1 = 16;
const int AIN2 = 17;
const int PWMA = 21;

// Motor B (Right)
const int BIN1 = 18;
const int BIN2 = 19;
const int PWMB = 22;

// Standby
const int STBY = 23;

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

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(PWMA, HIGH);
  digitalWrite(PWMB, HIGH);
  digitalWrite(STBY, HIGH);

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
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void moveBack()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void moveLeft()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void moveRight()
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void stopMoving()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}
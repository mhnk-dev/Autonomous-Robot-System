#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

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

// PWM
const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;

const int CHANNEL_A = 0;
const int CHANNEL_B = 1;

// RobotState
enum RobotState
{
  IDLE,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  STOP,
  EMERGENCY
};

RobotState state = IDLE;

int motorSpeed = 255;

unsigned long lastCommandTime = 0;
const unsigned long TIMEOUT = 800;

// Prototypes
void webSocketsServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void applyState(int speed);

void moveForward(int speed);
void moveBack(int speed);
void moveLeft(int speed);
void moveRight(int speed);
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

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  // PWM
  ledcSetup(CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(PWMA, CHANNEL_A);
  ledcAttachPin(PWMB, CHANNEL_B);

  ledcWrite(CHANNEL_A, 0);
  ledcWrite(CHANNEL_B, 0);

  webSocketsServer.begin();
  webSocketsServer.onEvent(webSocketsServerEvent);

  stopMoving();
}

// Loop
void loop()
{
  webSocketsServer.loop();

  if (millis() - lastCommandTime > TIMEOUT)
  {
    state = STOP;
  }

  if (state == STOP || state == IDLE)
  {
    stopMoving();
  }
  else
  {
    applyState(motorSpeed);
  }
}

// WebSockets Server Event
void webSocketsServerEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_CONNECTED:
  {
    Serial.printf("Client %u Connected\n", num);
    break;
  }
  case WStype_DISCONNECTED:
  {
    Serial.printf("Client %u Disconnected\n", num);
    state = STOP;
    stopMoving();
    break;
  }
  case WStype_TEXT:
  {
    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, payload);

    if (error)
    {
      Serial.print("JSON Error: ");
      Serial.println(error.c_str());
      return;
    }

    const char *cmd = doc["cmd"];
    int speed = doc["speed"] | 0;

    motorSpeed = constrain(speed, 0, 255);
    lastCommandTime = millis();

    if (cmd != nullptr)
    {
      String c = cmd;

      if (c == "F")
        state = FORWARD;
      else if (c == "B")
        state = BACKWARD;
      else if (c == "L")
        state = LEFT;
      else if (c == "R")
        state = RIGHT;
      else if (c == "S")
        state = STOP;
      else if (c == "E")
        state = EMERGENCY;
    }
    break;
  }
  default:
    break;
  }
}

// Motor Control Router
void applyState(int speed)
{
  switch (state)
  {
  case FORWARD:
    moveForward(speed);
    break;

  case BACKWARD:
    moveBack(speed);
    break;

  case LEFT:
    moveLeft(speed);
    break;

  case RIGHT:
    moveRight(speed);
    break;

  case STOP:
    state = IDLE;
    stopMoving();
    break;
  case IDLE:
  default:
    stopMoving();
    break;
  }
}

// Motors
void moveForward(int speed)
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(CHANNEL_A, speed);
  ledcWrite(CHANNEL_B, speed);
}

void moveBack(int speed)
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(CHANNEL_A, speed);
  ledcWrite(CHANNEL_B, speed);
}

void moveLeft(int speed)
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(CHANNEL_A, speed);
  ledcWrite(CHANNEL_B, speed);
}

void moveRight(int speed)
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(CHANNEL_A, speed);
  ledcWrite(CHANNEL_B, speed);
}

void stopMoving()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  ledcWrite(CHANNEL_A, 0);
  ledcWrite(CHANNEL_B, 0);
}
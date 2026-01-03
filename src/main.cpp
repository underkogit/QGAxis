

#if !(defined(ESP32))
#error This code is designed for (ESP32_S2/S3/C3 + LwIP ENC28J60) to run on ESP32 platform! Please check your Tools->Board setting.
#endif

#include "includes.h"
#include <httpServer.h>
#include <mpuManager.h>
#include <udpManager.h>

void Init()
{
  Serial.begin(115200);
}

void setup()
{
  Init();
  WifiConnect();

  InitHttpServer();
  startUdpServer();

  InitMPU6050();
}

void loop()
{
}

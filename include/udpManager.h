#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>
#include <WiFi.h>
#include "mpuManager.h"

WiFiUDP udp;


void udpServerTask(void *pvParameters)
{
    char packetBuffer[255];
    while (true)
    {
        int packetSize = udp.parsePacket();
        if (packetSize)
        {
            int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
            if (len > 0 && MpuStatusConnection())
            {
                packetBuffer[len] = '\0';
                udp.beginPacket(udp.remoteIP(), udp.remotePort());
                const char *ackMessage = GetSensorDataJson().c_str();
                udp.write((const uint8_t *)ackMessage, strlen(ackMessage));
                udp.endPacket();
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void startUdpServer()
{
    InitMPU6050();
    if (udp.begin(localPort))
    {
        BaseType_t taskCreated = xTaskCreate(
            udpServerTask,
            "udp_server",
            4096,
            NULL,
            1,
            NULL);
    }
}
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>
#include <WiFi.h>
#include "mpuManager.h"

WiFiUDP udp;
const unsigned int localPort = 8888;


void udpServerTask(void *pvParameters)
{
    char packetBuffer[255];

    Serial.println("UDP Server task started");

    while (true)
    {
        int packetSize = udp.parsePacket();

        if (packetSize)
        {
            Serial.print("Received packet from: ");
            Serial.print(udp.remoteIP());
            Serial.print(":");
            Serial.println(udp.remotePort());
            Serial.print("Packet size: ");
            Serial.println(packetSize);

            int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
            if (len > 0 && MpuStatusConnection())
            {
                packetBuffer[len] = '\0';

                Serial.println(packetBuffer);

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
        Serial.print("UDP Server started on port ");

        Serial.println(WiFi.localIP());

        Serial.println(localPort);
    }
    else
    {
        Serial.println("Failed to start UDP Server!");
        return;
    }

    BaseType_t taskCreated = xTaskCreate(
        udpServerTask,
        "udp_server",
        4096,
        NULL,
        1,
        NULL);

    if (taskCreated != pdPASS)
    {
        Serial.println("Failed to create UDP server task!");
    }
    else
    {
        Serial.println("UDP server task created successfully");
    }
}
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>
#include <WiFi.h>

WiFiUDP udp;
const unsigned int localPort = 8888;

// Функция задачи для обработки UDP-пакетов
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

            // Читаем пакет в буфер
            int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1); // -1 для нуль-терминатора
            if (len > 0)
            {
                packetBuffer[len] = '\0'; // Нуль-терминируем строку

                Serial.print("Message: ");
                Serial.println(packetBuffer);

                // Отправляем ответ
                udp.beginPacket(udp.remoteIP(), udp.remotePort());

                const char *ackMessage = "ACK";
                udp.write((const uint8_t *)ackMessage, strlen(ackMessage));
                udp.endPacket();
                Serial.println("ACK sent");
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

// Функция запуска UDP-сервера
void startUdpServer()
{
    // Инициализируем UDP
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

    // Создаем задачу для обработки UDP
    BaseType_t taskCreated = xTaskCreate(
        udpServerTask, // Функция задачи
        "udp_server",  // Имя задачи (до 16 символов)
        4096,          // Размер стека (увеличил для надежности)
        NULL,          // Параметры
        1,             // Приоритет
        NULL           // Хэндл задачи
    );

    if (taskCreated != pdPASS)
    {
        Serial.println("Failed to create UDP server task!");
    }
    else
    {
        Serial.println("UDP server task created successfully");
    }
}
#pragma once
#include <WiFi.h>
#include "wifiConfig.h"

bool WifiConnect(unsigned long timeoutMs = 30000)
{
    unsigned long startTime = millis();
    int connectionAttempts = 0;

    Serial.print("Connecting to Wi-Fi: ");
    

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        // Проверка таймаута
        if (millis() - startTime > timeoutMs)
        {
            Serial.println("\nWifi Connection timeout!");
            return false;
        }

        // Индикация прогресса
        if (connectionAttempts % 10 == 0)
        {
            Serial.print(".");
        }
        connectionAttempts++;

        delay(500);
    }

    Serial.println("\nConnected to Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    return true;
}
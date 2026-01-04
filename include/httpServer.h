#pragma once

#include <WebServer.h>
#include "contentType.h"
#include <ArduinoJson.h>
#include <esp_system.h>
#include "mpuManager.h"

void StartHttpServer();

WebServer server(80);
StaticJsonDocument<500> doc;
String jsonOutput;
SensorData data;

size_t getMaximumPossibleHeapFree()
{
    return ESP.getHeapSize();
}

size_t getMaximumEverSeenFree()
{
    static size_t max_seen = 0;
    size_t current = esp_get_free_heap_size();
    if (current > max_seen)
    {
        max_seen = current;
    }
    return max_seen;
}

void handleFreeSize()
{
    doc["bytest"] = esp_get_free_heap_size();
    doc["kb"] = esp_get_free_heap_size() / 1024;
    doc["MaximumPossibleHeap"] = getMaximumPossibleHeapFree();
    doc["MaximumPossibleHeapKB"] = getMaximumPossibleHeapFree() / 1024;
    doc["MaximumEverSeen"] = getMaximumEverSeenFree();
    doc["MaximumEverSeenKB"] = getMaximumEverSeenFree() / 1024;
    serializeJsonPretty(doc, jsonOutput);
    server.send(200, content_type, jsonOutput);
}

void handleNotFound()
{
    doc["status"] = 404;
    doc["content"] = "Not Found";
    serializeJsonPretty(doc, jsonOutput);
    server.send(404, content_type, jsonOutput);
}

String GetSensorDataJson()
{
    if (MpuStatusConnection())
    {
        data = readSensorData();
        doc["temperature"] = data.temperature;
        doc["pitch"] = data.angle.pitch;
        doc["roll"] = data.angle.roll;
    }
    else
    {
        doc["temperature"] = 0;
        doc["pitch"] = 0;
        doc["roll"] = 0;
    }
    serializeJsonPretty(doc, jsonOutput);
    return jsonOutput;
}

void handleGetData()
{
    server.send(200, content_type, GetSensorDataJson());
}

void handleRestart()
{
    server.send(200, content_type, "Restart");
    ESP.restart();
}

void handleResetOffset()
{
    resetOffset();
    server.send(200, content_type, "resetOffset");
}

void handleCalibration()
{
    calibration();
    server.send(200, content_type, "calibration");
}

void InitHttpServer()
{
    server.on("/freesize", HTTP_GET, handleFreeSize);
    server.on("/mpu6050", HTTP_GET, handleGetData);
    server.on("/restart", HTTP_GET, handleRestart);
    server.on("/resetoffset", HTTP_GET, handleResetOffset);
    server.on("/calibration", HTTP_GET, handleCalibration);
    server.onNotFound(handleNotFound);
    server.begin();
    StartHttpServer();
}

void httpServerTask(void *pvParameters)
{
    while (true)
    {
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void StartHttpServer()
{
    xTaskCreate(httpServerTask, "Http server", 2048, NULL, 1, NULL);
}
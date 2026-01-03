#pragma once



#include <WebServer.h>
#include "contentType.h"
#include <ArduinoJson.h>
#include <esp_system.h>

void StartHttpServer();

WebServer server(80);

size_t getMaximumPossibleHeapFree()
{
    // Это ВСЯ heap память ESP32 (теоретический максимум)
    return ESP.getHeapSize(); // Обычно 327680 байт (320KB)
}

size_t getMaximumEverSeenFree()
{
    // Максимальное значение, которое вы видели во время работы
    static size_t max_seen = 0;
    size_t current = esp_get_free_heap_size();

    if (current > max_seen)
    {
        max_seen = current;
    }

    return max_seen; // Обычно ~294000 байт после загрузки
}

void handleFreeSize()
{
    StaticJsonDocument<500> doc;

    doc["bytest"] = esp_get_free_heap_size();
    doc["kb"] = esp_get_free_heap_size() / 1024;

    doc["MaximumPossibleHeap"] = getMaximumPossibleHeapFree();
    doc["MaximumPossibleHeapKB"] = getMaximumPossibleHeapFree() / 1024;

    doc["MaximumEverSeen"] = getMaximumEverSeenFree();
    doc["MaximumEverSeenKB"] = getMaximumEverSeenFree() / 1024;


    String jsonOutput;
    serializeJsonPretty(doc, jsonOutput);

    server.send(200, content_type, jsonOutput);
}

void handleNotFound()
{
    StaticJsonDocument<500> doc;

    doc["status"] = 404;
    doc["content"] = "Not Found";
    String jsonOutput;
    serializeJsonPretty(doc, jsonOutput);

    server.send(404, content_type, jsonOutput);
}

void InitHttpServer()
{
    server.on("/freesize", HTTP_GET, handleFreeSize);

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

    xTaskCreate(
        httpServerTask,
        "Http server",
        2048,
        NULL,
        1,
        NULL);
}
#pragma once
#include "includes.h"

MPU6050 mpu;

bool InitMPU6050(unsigned int maxAttempts = 10)
{
    Wire.begin();
    mpu.initialize();

    Serial.println("Initializing MPU6050...");

    Wire.begin();
    mpu.initialize();

    for (unsigned int attempt = 1; attempt <= maxAttempts; attempt++)
    {
        if (mpu.testConnection())
        {
            Serial.println("✓ MPU6050 connected successfully");

            // Дополнительная инициализация MPU
            mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
            mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
            mpu.setDLPFMode(MPU6050_DLPF_BW_42);

            return true;
        }

        Serial.print("MPU6050 connection attempt ");
        Serial.print(attempt);
        Serial.println(" failed");

        if (attempt < maxAttempts)
        {
            delay(500);
        }
    }

    Serial.println("✗ Failed to initialize MPU6050 after all attempts");
    return false;
}

#pragma once
#include "includes.h"
#define BUFFER_SIZE 100

void calibration();
void resetOffset();

struct AngleData
{
    float pitch, roll;
};

struct SensorData
{
    AngleData angle;
    float temperature;
};

SensorData readSensorData();
MPU6050 mpu;
float accOffsetX = 0, accOffsetY = 0, accOffsetZ = 0;
float accScaleX = 1.0, accScaleY = 1.0, accScaleZ = 1.0;
const float LSB_2G = 16384.0;
AngleData angle;
int16_t rawAx, rawAy, rawAz, gx, gy, gz;
long offsets[6];
long offsetsOld[6];
int16_t mpuGet[6];
bool connection = false;

bool MpuStatusConnection()
{
    return connection;
}

bool InitMPU6050(unsigned int maxAttempts = 10)
{
    Wire.begin(23, 22);
    mpu.initialize();
    Serial.println("Initializing MPU6050...");
    for (unsigned int attempt = 1; attempt <= maxAttempts; attempt++)
    {
        if (mpu.testConnection())
        {
            Serial.println("✓ MPU6050 connected successfully");
            resetOffset();
            calibration();
            mpu.setDLPFMode(MPU6050_DLPF_BW_42);
            connection = true;
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
    connection = false;
    return false;
}

void getMotion6Update()
{
    mpu.getMotion6(&rawAx, &rawAy, &rawAz, &gx, &gy, &gz);
}

AngleData readSensorDataAngle()
{
    getMotion6Update();
    float ax = (rawAx - accOffsetX) * accScaleX;
    float ay = (rawAy - accOffsetY) * accScaleY;
    float az = (rawAz - accOffsetZ) * accScaleZ;
    float ax_g = ax / LSB_2G;
    float ay_g = ay / LSB_2G;
    float az_g = az / LSB_2G;
    angle.roll = atan2(ay_g, az_g) * 180.0 / PI;
    angle.pitch = atan2(-ax_g, sqrt(ay_g * ay_g + az_g * az_g)) * 180.0 / PI;
    return angle;
}

SensorData readSensorData()
{
    SensorData data;
    data.angle = readSensorDataAngle();
    data.temperature = (mpu.getTemperature() / 340.0) + 36.53;
    return data;
}

void resetOffset()
{
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
}

void calibration()
{
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    delay(10);
    Serial.println("Calibration start. It will take about 5 seconds");
    for (byte n = 0; n < 10; n++)
    {
        for (byte j = 0; j < 6; j++)
        {
            offsets[j] = 0;
        }
        for (byte i = 0; i < 100 + BUFFER_SIZE; i++)
        {
            mpu.getMotion6(&mpuGet[0], &mpuGet[1], &mpuGet[2], &mpuGet[3], &mpuGet[4], &mpuGet[5]);
            if (i >= 99)
            {
                for (byte j = 0; j < 6; j++)
                {
                    offsets[j] += (long)mpuGet[j];
                }
            }
        }
        for (byte i = 0; i < 6; i++)
        {
            offsets[i] = offsetsOld[i] - ((long)offsets[i] / BUFFER_SIZE);
            if (i == 2)
                offsets[i] += 16384;
            offsetsOld[i] = offsets[i];
        }
        mpu.setXAccelOffset(offsets[0] / 8);
        mpu.setYAccelOffset(offsets[1] / 8);
        mpu.setZAccelOffset(offsets[2] / 8);
        mpu.setXGyroOffset(offsets[3] / 4);
        mpu.setYGyroOffset(offsets[4] / 4);
        mpu.setZGyroOffset(offsets[5] / 4);
        delay(2);
    }
}
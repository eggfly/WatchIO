#ifndef _IMU_H_
#define _IMU_H_

#include "Wire.h"
#include "src/MPU6050_tockn/src/MPU6050_tockn.h"

MPU6050 mpu6050(Wire);

long imu_timer = 0;

// 初始化
void imu_init(void) {
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void read_imu(int16_t *accX, int16_t *accY) {
  if (millis() - imu_timer > 50) {
    mpu6050.update();
    //    Serial.println("=======================================================");
    //    Serial.print("temp : "); Serial.println(mpu6050.getTemp());
    //    Serial.print("accX : "); Serial.print(mpu6050.getAccX());
    //    Serial.print("\taccY : "); Serial.print(mpu6050.getAccY());
    //    Serial.print("\taccZ : "); Serial.println(mpu6050.getAccZ());
    //
    //    Serial.print("accAngleX : "); Serial.print(mpu6050.getAccAngleX());
    //    Serial.print("\taccAngleY : "); Serial.println(mpu6050.getAccAngleY());
    //
    //    Serial.print("angleX : "); Serial.print(mpu6050.getAngleX());
    //    Serial.print("\tangleY : "); Serial.print(mpu6050.getAngleY());
    //    Serial.print("\tangleZ : "); Serial.println(mpu6050.getAngleZ());
    //    Serial.println("=======================================================\n");
    imu_timer = millis();
  }
  // reverse x and y, this is a hack
  *accX = -1800 * mpu6050.getAccY();
  *accY = -1800 * mpu6050.getAccX();
  // Serial.printf("%ld,%ld\r\n", *accX, *accY);
}

#endif // _IMU_H_

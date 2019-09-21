#ifndef _IMU_H_
#define _IMU_H_

#include "Wire.h"
#include "warning.h"
#include "src/MPU6050_tockn/src/MPU6050_tockn.h"

MPU6050 mpu6050(Wire);


#define MOTION_THRESHOLD       60
#define MOTION_EVENT_DURATION   5

#define INTERRUPT_PIN           4
#define IMU_SAMPLE_DURATION     50
long imu_timer = -IMU_SAMPLE_DURATION;

// 初始化
void imu_init(void) {
  mpu6050.begin();
  // mpu6050.calcGyroOffsets(true);
}

bool imu_interrupted = false;

void isr_imu_interrupt() {
  imu_interrupted = true;
  feed_battery_warning();
  // sleep_disable();
  // detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
  // ledState = !ledState;
}

void imu_init_interrupt() {
  mpu6050.setAccelerometerPowerOnDelay(3);
  mpu6050.setInterruptMode(true); // Interrupts enabled
  mpu6050.setInterruptLatch(0); // 55 Interrupt pulses when triggered instead of remaining on until cleared
  mpu6050.setIntMotionEnabled(true); // Interrupts sent when motion detected

  // Set sensor filter mode.
  // 0 -> Reset (disable high pass filter)
  // 1 -> On (5Hz)
  // 2 -> On (2.5Hz)
  // 3 -> On (1.25Hz)
  // 4 -> On (0.63Hz)
  // 5 -> Hold (Future outputs are relative to last output when this mode was set)
  // register 28
  mpu6050.setDHPFMode(1);

  // Motion detection acceleration threshold. 1LSB = 2mg.
  mpu6050.setMotionDetectionThreshold(MOTION_THRESHOLD);

  // Motion detection event duration in ms
  mpu6050.setMotionDetectionDuration(MOTION_EVENT_DURATION);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), isr_imu_interrupt, RISING);
}

void read_imu(int16_t *accX, int16_t *accY, int16_t *accZ) {
  if (millis() - imu_timer > IMU_SAMPLE_DURATION) {
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
  *accX = -3000 * mpu6050.getAccY();
  *accY = -3000 * mpu6050.getAccX();
  *accZ = -3000 * mpu6050.getAccZ();
  // Serial.printf("%ld,%ld,%ld\r\n", *accX, *accY, *accZ);
}

#endif // _IMU_H_

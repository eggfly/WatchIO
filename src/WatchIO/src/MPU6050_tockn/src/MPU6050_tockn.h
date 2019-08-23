#ifndef MPU6050_TOCKN_H
#define MPU6050_TOCKN_H

#include "Arduino.h"
#include "Wire.h"

#define MPU6050_ADDR         0x68
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6b
#define MPU6050_TEMP_H       0x41
#define MPU6050_TEMP_L       0x42

#ifndef BUFFER_LENGTH
// band-aid fix for platforms without Wire-defined BUFFER_LENGTH (removed from some official implementations)
#define BUFFER_LENGTH 32
#endif

#define MPU6050_RA_MOT_DETECT_CTRL      0x69
#define MPU6050_DETECT_ACCEL_ON_DELAY_BIT       5
#define MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH    2


#define MPU6050_RA_MOT_DUR          0x20

#define MPU6050_RA_INT_PIN_CFG      0x37

#define MPU6050_RA_INT_ENABLE       0x38

#define MPU6050_RA_ACCEL_CONFIG     0x1C

#define MPU6050_RA_MOT_THR          0x1F


#define MPU6050_INTCFG_INT_LEVEL_BIT        7
#define MPU6050_INTCFG_INT_OPEN_BIT         6
#define MPU6050_INTCFG_LATCH_INT_EN_BIT     5
#define MPU6050_INTCFG_INT_RD_CLEAR_BIT     4
#define MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT  3
#define MPU6050_INTCFG_FSYNC_INT_EN_BIT     2
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT    1
#define MPU6050_INTCFG_CLKOUT_EN_BIT        0


#define MPU6050_INTERRUPT_FF_BIT            7
#define MPU6050_INTERRUPT_MOT_BIT           6
#define MPU6050_INTERRUPT_ZMOT_BIT          5
#define MPU6050_INTERRUPT_FIFO_OFLOW_BIT    4
#define MPU6050_INTERRUPT_I2C_MST_INT_BIT   3
#define MPU6050_INTERRUPT_PLL_RDY_INT_BIT   2
#define MPU6050_INTERRUPT_DMP_INT_BIT       1
#define MPU6050_INTERRUPT_DATA_RDY_BIT      0


#define MPU6050_ACONFIG_XA_ST_BIT           7
#define MPU6050_ACONFIG_YA_ST_BIT           6
#define MPU6050_ACONFIG_ZA_ST_BIT           5
#define MPU6050_ACONFIG_AFS_SEL_BIT         4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH      2
#define MPU6050_ACONFIG_ACCEL_HPF_BIT       2
#define MPU6050_ACONFIG_ACCEL_HPF_LENGTH    3


class MPU6050{
  public:

  MPU6050(TwoWire &w);
  MPU6050(TwoWire &w, float aC, float gC);

  void begin();

  void setGyroOffsets(float x, float y, float z);

  void writeMPU6050(byte reg, byte data);
  byte readMPU6050(byte reg);

  int16_t getRawAccX(){ return rawAccX; };
  int16_t getRawAccY(){ return rawAccY; };
  int16_t getRawAccZ(){ return rawAccZ; };

  int16_t getRawTemp(){ return rawTemp; };

  int16_t getRawGyroX(){ return rawGyroX; };
  int16_t getRawGyroY(){ return rawGyroY; };
  int16_t getRawGyroZ(){ return rawGyroZ; };

  float getTemp(){ return temp; };

  float getAccX(){ return accX; };
  float getAccY(){ return accY; };
  float getAccZ(){ return accZ; };

  float getGyroX(){ return gyroX; };
  float getGyroY(){ return gyroY; };
  float getGyroZ(){ return gyroZ; };

	void calcGyroOffsets(bool console = false, uint16_t delayBefore = 1000, uint16_t delayAfter = 3000);

  float getGyroXoffset(){ return gyroXoffset; };
  float getGyroYoffset(){ return gyroYoffset; };
  float getGyroZoffset(){ return gyroZoffset; };

  void update();

  uint8_t getAccelerometerPowerOnDelay();
  void setAccelerometerPowerOnDelay(uint8_t delay);

  uint8_t getIntEnabled();
  void setIntEnabled(uint8_t enabled);

  bool getInterruptMode();
  void setInterruptMode(bool mode);

  bool getInterruptLatch();
  void setInterruptLatch(bool latch);
  bool getInterruptLatchClear();
  void setInterruptLatchClear(bool clear);
  
  bool getIntMotionEnabled();
  void setIntMotionEnabled(bool enabled);

  uint8_t getDHPFMode();
  void setDHPFMode(uint8_t mode);

  // MOT_THR register
  uint8_t getMotionDetectionThreshold();
  void setMotionDetectionThreshold(uint8_t threshold);

  // MOT_DUR register
  uint8_t getMotionDetectionDuration();
  void setMotionDetectionDuration(uint8_t duration);


  float getAccAngleX(){ return angleAccX; };
  float getAccAngleY(){ return angleAccY; };

  float getGyroAngleX(){ return angleGyroX; };
  float getGyroAngleY(){ return angleGyroY; };
  float getGyroAngleZ(){ return angleGyroZ; };

  float getAngleX(){ return angleX; };
  float getAngleY(){ return angleY; };
  float getAngleZ(){ return angleZ; };

  private:
  uint8_t buffer[14];

  TwoWire *wire;

  int16_t rawAccX, rawAccY, rawAccZ, rawTemp,
  rawGyroX, rawGyroY, rawGyroZ;

  float gyroXoffset, gyroYoffset, gyroZoffset;

  float temp, accX, accY, accZ, gyroX, gyroY, gyroZ;

  float angleGyroX, angleGyroY, angleGyroZ,
  angleAccX, angleAccY, angleAccZ;

  float angleX, angleY, angleZ;

  float interval;
  long preInterval;

  float accCoef, gyroCoef;
};

#endif

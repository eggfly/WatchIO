
#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

long timer = 0;

#define MOTION_THRESHOLD       65
#define MOTION_EVENT_DURATION   5

#define INTERRUPT_PIN           4

#define TFT_BACKLIGHT   15 // Display backlight pin


void setup() {
  backlight_setup();
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  // mpu6050.calcGyroOffsets(true);


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
  mpu6050.setDHPFMode(2);

  // Motion detection acceleration threshold. 1LSB = 2mg.
  mpu6050.setMotionDetectionThreshold(MOTION_THRESHOLD);

  // Motion detection event duration in ms
  mpu6050.setMotionDetectionDuration(MOTION_EVENT_DURATION);

  // attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), wakeUp, RISING);
  delay(1000);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low

  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");

}

bool wakedUp = false;

void wakeUp() {
  wakedUp = true;
  // sleep_disable();
  // detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
  // ledState = !ledState;
}


static bool ledc_setup = false;

void backlight_setup() {
  if (!ledc_setup) {
    ledcSetup(0, 5000, 8);
    ledcAttachPin(TFT_BACKLIGHT, 0);
    ledc_setup = true;
    // low is backlight on
    ledcWrite(0, 0);
  }
}

long last_time = 0;

void loop() {
  if (wakedUp) {
    wakedUp = false;
    Serial.println("I'm waked up by mpu!");
  }
  if (millis() - last_time > 1000) {
    last_time = millis();
    Serial.println("loop..");
  }
}

void loop_old() {

  if (millis() - timer > 1000) {
    mpu6050.update();
    Serial.printf("getAccelerometerPowerOnDelay=%d\r\n", mpu6050.getAccelerometerPowerOnDelay());

    Serial.println("=======================================================");
    Serial.print("temp : "); Serial.println(mpu6050.getTemp());
    Serial.print("accX : "); Serial.print(mpu6050.getAccX());
    Serial.print("\taccY : "); Serial.print(mpu6050.getAccY());
    Serial.print("\taccZ : "); Serial.println(mpu6050.getAccZ());

    Serial.print("gyroX : "); Serial.print(mpu6050.getGyroX());
    Serial.print("\tgyroY : "); Serial.print(mpu6050.getGyroY());
    Serial.print("\tgyroZ : "); Serial.println(mpu6050.getGyroZ());

    Serial.print("accAngleX : "); Serial.print(mpu6050.getAccAngleX());
    Serial.print("\taccAngleY : "); Serial.println(mpu6050.getAccAngleY());

    Serial.print("gyroAngleX : "); Serial.print(mpu6050.getGyroAngleX());
    Serial.print("\tgyroAngleY : "); Serial.print(mpu6050.getGyroAngleY());
    Serial.print("\tgyroAngleZ : "); Serial.println(mpu6050.getGyroAngleZ());

    Serial.print("angleX : "); Serial.print(mpu6050.getAngleX());
    Serial.print("\tangleY : "); Serial.print(mpu6050.getAngleY());
    Serial.print("\tangleZ : "); Serial.println(mpu6050.getAngleZ());
    Serial.println("=======================================================\n");
    timer = millis();

  }

}

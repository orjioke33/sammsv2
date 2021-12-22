#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Kylefruit_ADXL343.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/** The input pin to enable the interrupt on, connected to INT1 on the ADXL. */
#define INPUT_PIN_INT1   2 // SAMD21/SAMD51 = 5 for interrupt pin

volatile uint32_t g_tap_count = 0;
int_config g_int_config_enabled = { 0 };
int_config g_int_config_map = { 0 };

volatile bool event = false;


/** Interrupt service routine for INT1 events. This will be called when a single tap is detected. */
void int1_isr(void)
{
    event = true;
}

void setup(void)
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("ADXL343 Single Tap INT Tester"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_4_G);
  accel.setDataRate(ADXL343_DATARATE_800_HZ);
  /* Configure the HW interrupts. */
  pinMode(INPUT_PIN_INT1, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT1), int1_isr, RISING);

  /* Enable single tap interrupts on the accelerometer. */
  g_int_config_enabled.bits.single_tap = false;
  g_int_config_enabled.bits.overrun = false;
  g_int_config_enabled.bits.data_ready = false;
  g_int_config_enabled.bits.watermark = true;
  accel.enableInterrupts(g_int_config_enabled);

  /* Map single tap interrupts to INT1 pin. */
  g_int_config_map.bits.watermark = ADXL343_INT1;
  accel.mapInterrupts(g_int_config_map);

  accel.setFifoParameters(0,32);
  accel.setFifoMode(2); // set mode to stream

  /* Reset tap counter. */
  g_tap_count = 0;

  Serial.println("ADXL343 init complete. Waiting for single tap INT activity.");
}

uint8_t int_source = 0;

void loop(void)
{
  if(event)
  {
    accel.setMeasureMode(false);
    //accel.writeRegister(); // set to stop measurements
    // Read and clear interrupts
    int_source = accel.readRegister(0x30); // read interrupt source register

    printFifo();
    
    event = false;
    // Read and clear interrupts
    accel.readRegister(0x30); // read interrupt source register

    // start measurements
    accel.setMeasureMode(true);

    
  }
  /* Get a new sensor event */

}
uint8_t accelBuffer[6];
void printFifo()
{
  
  for (int i = 0; i <32; i++)
  {
    accel.read48(&accelBuffer[0]);
    //uint16_t val = accel.getX();
    Serial.print(i);Serial.print(": ");
    Serial.println(accelBuffer[0]);
  }
}

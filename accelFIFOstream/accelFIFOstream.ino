#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Kylefruit_ADXL343.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/** The input pin to enable the interrupt on, connected to INT1 on the ADXL. */
#define INPUT_PIN_INT1   5 // SAMD21/SAMD51 = 5 for interrupt pin
#define GRAVITY_SCALE_FACTOR 3.9 // 3.9mg per LSB

volatile uint32_t g_tap_count = 0;
int_config g_int_config_enabled = { 0 };
int_config g_int_config_map = { 0 };

volatile bool event = false;

/** Timer interrupt service routine for int2 events. Called every 0.04 seconds. Returns the time value in micro-seconds until interrupt occurs again. **/
int int2_isr(void)
{
  event = true;
  return 40000;
}

void setup(void)
{
  Wire.begin();
  Wire.setClock(400000);
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

  // Attach a timer interrupt to the data pulling
  attachTimerInterrupt(int2_isr, 40000);

  //pinMode(INPUT_PIN_INT1, INPUT);
  //attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT1), int1_isr, RISING);

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
  uint8_t fifo_ctl = 0;
  fifo_ctl = accel.readRegister(0x38); // read interrupt source register

  /* Reset tap counter. */
  g_tap_count = 0;

  Serial.println("ADXL343 init complete. Waiting for single tap INT activity.");
}

uint8_t int_source = 0;

void loop(void)
{
  if(event)
  {
    // Turn off measurements
    accel.setMeasureMode(false);
    
    // Read and clear interrupts
    int_source = accel.readRegister(0x30); // read interrupt source register

    // Read all 32 values out of fifo
    printFifo();
    event = false;
    
    // Read and clear interrupts
    accel.readRegister(0x30); // read interrupt source register

    // start measurements
    accel.setMeasureMode(true);
 
  }
}


uint8_t accelBuffer[6];
uint16_t accelerationVal = 0;
int accelerationValScaled = 0;
float acclVal = 0;
/** Helper function to read all values out of fifo and then print integer values to terminal **/
void printFifo()
{
  
  for (int i = 0; i < 32; i++)
  {
    accel.read48(&accelBuffer[0]);
    //uint16_t val = accel.getX();
    Serial.print(i);Serial.print(": ");

    for (int k = 0; k < 5; k+=2)
    {
      accelerationVal = ((accelBuffer[k+1] << 8)|(accelBuffer[k]));
      accelerationValScaled = twos_complement(accelerationVal, 16);
      acclVal =  (float) GRAVITY_SCALE_FACTOR * accelerationValScaled;
      Serial.println(acclVal);
    }
  }
}

/* Helper function to take bit value and change to twos complement */ 
int twos_complement(uint16_t input, int numBits)
{
  int output = 0;
  output = (int) input;
  if (output & (1<<(numBits-1)))
  {
    output -= 1 << numBits;
  }
  return output;
}

/** Helper function to print out register map in binary **/ 
void printRegs()
{
  int byteRead = 0;
  for (int i = 29; i < 58; i++)
  {
    byteRead = accel.readRegister(i);    
    Serial.print("Reg ");Serial.print(i); Serial.print(" : ");Serial.println(byteRead,BIN);
  }
}

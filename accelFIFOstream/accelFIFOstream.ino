#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Kylefruit_ADXL343.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/** The input pin to enable the interrupt on, connected to INT1 on the ADXL. */
#define INPUT_PIN_INT1   2 // SAMD21/SAMD51 = 5 for interrupt pin

uint32_t g_tap_count = 0;
int_config g_int_config_enabled = { 0 };
int_config g_int_config_map = { 0 };

void displayDataRate(void)
{
  Serial.print ("Data Rate: ");
  switch(accel.getDataRate())
  {
    case ADXL343_DATARATE_3200_HZ:
      Serial.print ("3200 ");
      break;
    case ADXL343_DATARATE_1600_HZ:
      Serial.print ("1600 ");
      break;
    case ADXL343_DATARATE_800_HZ:
      Serial.print ("800 ");
      break;
    case ADXL343_DATARATE_400_HZ:
      Serial.print ("400 ");
      break;
    case ADXL343_DATARATE_200_HZ:
      Serial.print ("200 ");
      break;
    case ADXL343_DATARATE_100_HZ:
      Serial.print ("100 ");
      break;
    case ADXL343_DATARATE_50_HZ:
      Serial.print ("50 ");
      break;
    case ADXL343_DATARATE_25_HZ:
      Serial.print ("25 ");
      break;
    case ADXL343_DATARATE_12_5_HZ:
      Serial.print ("12.5 ");
      break;
    case ADXL343_DATARATE_6_25HZ:
      Serial.print ("6.25 ");
      break;
    case ADXL343_DATARATE_3_13_HZ:
      Serial.print ("3.13 ");
      break;
    case ADXL343_DATARATE_1_56_HZ:
      Serial.print ("1.56 ");
      break;
    case ADXL343_DATARATE_0_78_HZ:
      Serial.print ("0.78 ");
      break;
    case ADXL343_DATARATE_0_39_HZ:
      Serial.print ("0.39 ");
      break;
    case ADXL343_DATARATE_0_20_HZ:
      Serial.print ("0.20 ");
      break;
    case ADXL343_DATARATE_0_10_HZ:
      Serial.print ("0.10 ");
      break;
    default:
      Serial.print ("???? ");
      break;
  }
  Serial.println(" Hz");
}

void displayRange(void)
{
  Serial.print ("Range: +/- ");
  switch(accel.getRange())
  {
    case ADXL343_RANGE_16_G:
      Serial.print ("16 ");
      break;
    case ADXL343_RANGE_8_G:
      Serial.print ("8 ");
      break;
    case ADXL343_RANGE_4_G:
      Serial.print ("4 ");
      break;
    case ADXL343_RANGE_2_G:
      Serial.print ("2 ");
      break;
    default:
      Serial.print ("?? ");
      break;
  }
  Serial.println(" g");
}

/** Interrupt service routine for INT1 events. This will be called when a single tap is detected. */
void int1_isr(void)
{
    g_tap_count++;
}

void setup(void)
{
  Serial.begin(9600);
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
  accel.setRange(ADXL343_RANGE_16_G);

  /* Configure the HW interrupts. */
  pinMode(INPUT_PIN_INT1, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT1), int1_isr, RISING);

  /* Enable single tap interrupts on the accelerometer. */
  g_int_config_enabled.bits.single_tap = true;
  accel.enableInterrupts(g_int_config_enabled);

  /* Map single tap interrupts to INT1 pin. */
  g_int_config_map.bits.single_tap = ADXL343_INT1;
  accel.mapInterrupts(g_int_config_map);

  /* Reset tap counter. */
  g_tap_count = 0;

  Serial.println("ADXL343 init complete. Waiting for single tap INT activity.");
  displayRange();
  displayDataRate();
  accel.printSensorDetails();
}

void loop(void)
{
  /* Get a new sensor event */
  /* Reading data clears the interrupts. */
  sensors_event_t event;
  accel.getEvent(&event);
  delay(10);

  while (g_tap_count) {
      Serial.println("Single tap detected!");
      /* Clear the interrupt by check the source register.. */
      uint8_t source = accel.checkInterrupts();
      /* Decrement the local interrupt counter. */
      g_tap_count--;
  }
}

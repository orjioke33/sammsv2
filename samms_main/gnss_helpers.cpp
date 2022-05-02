/* include the GNSS library */
#include <Arduino.h> // Need this forward declaration for Serial files
#include <GNSS.h>
#define STRING_BUFFER_SIZE  128       /**< %Buffer size */
#define RESTART_CYCLE       (60 * 5)  /**< positioning test term */
static SpGnss Gnss;                   /**< SpGnss object */

/**
 * @enum ParamSat
 * @brief Satellite system
 */
enum ParamSat {
  eSatGps,            /**< GPS                     World wide coverage  */
  eSatGlonass,        /**< GLONASS                 World wide coverage  */
  eSatGpsSbas,        /**< GPS+SBAS                North America        */
  eSatGpsGlonass,     /**< GPS+Glonass             World wide coverage  */
  eSatGpsBeidou,      /**< GPS+BeiDou              World wide coverage  */
  eSatGpsGalileo,     /**< GPS+Galileo             World wide coverage  */
  eSatGpsQz1c,        /**< GPS+QZSS_L1CA           East Asia & Oceania  */
  eSatGpsGlonassQz1c, /**< GPS+Glonass+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsBeidouQz1c,  /**< GPS+BeiDou+QZSS_L1CA    East Asia & Oceania  */
  eSatGpsGalileoQz1c, /**< GPS+Galileo+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsQz1cQz1S,    /**< GPS+QZSS_L1CA+QZSS_L1S  Japan                */
};

/* Set this parameter depending on your current region. */
static enum ParamSat satType =  eSatGps;

static void select_gnss_system() {
/* Setup GNSS
     *  It is possible to setup up to two GNSS satellites systems.
     *  Depending on your location you can improve your accuracy by selecting different GNSS system than the GPS system.
     *  See: https://developer.sony.com/develop/spresense/developer-tools/get-started-using-nuttx/nuttx-developer-guide#_gnss
     *  for detailed information.
    */
    switch (satType)
    {
    case eSatGps:
      Gnss.select(GPS);
      break;

    case eSatGpsSbas:
      Gnss.select(GPS);
      Gnss.select(SBAS);
      break;

    case eSatGlonass:
      Gnss.select(GLONASS);
      break;

    case eSatGpsGlonass:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      break;

    case eSatGpsBeidou:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      break;

    case eSatGpsGalileo:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      break;

    case eSatGpsQz1c:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsQz1cQz1S:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      Gnss.select(QZ_L1S);
      break;

    case eSatGpsBeidouQz1c:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGalileoQz1c:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGlonassQz1c:
    default:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      Gnss.select(QZ_L1CA);
      break;
    }
}

/**
 * @brief %Print position information.
 */
static void print_pos(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];

  /* print time */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%04d/%02d/%02d ", pNavData->time.year, pNavData->time.month, pNavData->time.day);
  Serial.print(StringBuffer);

  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%02d:%02d:%02d.%06ld, ", pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec);
  Serial.print(StringBuffer);

  /* print satellites count */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "numSat:%2d, ", pNavData->numSatellites);
  Serial.print(StringBuffer);

  /* print position data */
  if (pNavData->posFixMode == FixInvalid)
  {
    Serial.print("No-Fix, ");
  }
  else
  {
    Serial.print("Fix, ");
  }
  if (pNavData->posDataExist == 0)
  {
    Serial.print("No Position");
  }
  else
  {
    Serial.print("Lat=");
    Serial.print(pNavData->latitude, 6);
    Serial.print(", Lon=");
    Serial.print(pNavData->longitude, 6);
  }

  Serial.println("");
}

/**
 * @brief %Print satellite condition.
 */
static void print_condition(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];
  unsigned long cnt;

  /* Print satellite count. */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "numSatellites:%2d\n", pNavData->numSatellites);
  Serial.print(StringBuffer);

  for (cnt = 0; cnt < pNavData->numSatellites; cnt++)
  {
    const char *pType = "---";
    SpSatelliteType sattype = pNavData->getSatelliteType(cnt);

    /* Get satellite type. */
    /* Keep it to three letters. */
    switch (sattype)
    {
      case GPS:
        pType = "GPS";
        break;

      case GLONASS:
        pType = "GLN";
        break;

      case QZ_L1CA:
        pType = "QCA";
        break;

      case SBAS:
        pType = "SBA";
        break;

      case QZ_L1S:
        pType = "Q1S";
        break;

      case BEIDOU:
        pType = "BDS";
        break;

      case GALILEO:
        pType = "GAL";
        break;

      default:
        pType = "UKN";
        break;
    }

    /* Get print conditions. */
    unsigned long Id  = pNavData->getSatelliteId(cnt);
    unsigned long Elv = pNavData->getSatelliteElevation(cnt);
    unsigned long Azm = pNavData->getSatelliteAzimuth(cnt);
    float sigLevel = pNavData->getSatelliteSignalLevel(cnt);

    /* Print satellite condition. */
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "[%2ld] Type:%s, Id:%2ld, Elv:%2ld, Azm:%3ld, CN0:", cnt, pType, Id, Elv, Azm );
    Serial.print(StringBuffer);
    Serial.println(sigLevel, 6);
  }
}

void setup_gnss() {
  int error_flag = 0;
  /* Set Debug mode to Info */
  Gnss.setDebugMode(PrintInfo);

  int result;

  /* Activate GNSS device */
  result = Gnss.begin();

  if (result != 0)
  {
    Serial.println("Gnss begin error!!");
    error_flag = 1;
  }
  else
  {
    select_gnss_system();
    /* Start positioning */
    result = Gnss.start(COLD_START);
    if (result != 0)
    {
      Serial.println("Gnss start error!!");
      error_flag = 1;
    }
    else
    {
      Serial.println("Gnss setup OK");
    }
  }

  /* Set error LED. */
  if (error_flag == 1)
  {
    exit(0);
  }
}

void update_gnss() {
  static int LastPrintMin = 0;

  /* Check update. */
  if (Gnss.waitUpdate(-1))
  {
    /* Get NaviData. */
    SpNavData NavData;
    Gnss.getNavData(&NavData);

    /* Print satellite information every minute. */
    if (NavData.time.minute != LastPrintMin)
    {
      print_condition(&NavData);
      LastPrintMin = NavData.time.minute;
    }

    /* Print position information. */
    print_pos(&NavData);
  }
  else
  {
    /* Not update. */
    Serial.println("data not update");
  }
}

void restart_gnss_on_timeout() {
  int error_flag = 0;
  static int LoopCount = 0;
  
  /* Check loop count. */
  LoopCount++;
  if (LoopCount >= RESTART_CYCLE)
  {
    int error_flag = 0;

    /* Restart GNSS. */
    if (Gnss.stop() != 0)
    {
      Serial.println("Gnss stop error!!");
      error_flag = 1;
    }
    else if (Gnss.end() != 0)
    {
      Serial.println("Gnss end error!!");
      error_flag = 1;
    }
    else
    {
      Serial.println("Gnss stop OK.");
    }

    if (Gnss.begin() != 0)
    {
      Serial.println("Gnss begin error!!");
      error_flag = 1;
    }
    else if (Gnss.start(HOT_START) != 0)
    {
      Serial.println("Gnss start error!!");
      error_flag = 1;
    }
    else
    {
      Serial.println("Gnss restart OK.");
    }

    LoopCount = 0;

    /* Set error LED. */
    if (error_flag == 1)
    {
      exit(0);
    }

  }
}

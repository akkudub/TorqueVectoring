/**************************** INFO ********************************/

// This code expects a message in the format: H 12.00,-345.00,678.00

/******************************************************************/

#include <TextFinder.h>

/*** Defines the frequency at which the data is requested ***/
/*** frequency f=1/T, T=period; ie. 100ms --> f=10Hz, 200ms --> f=5Hz ***/
#define PERIOD      20 // [ms]

/*** Vars for IMU ***/
const int NUMBER_OF_FIELDS = 3; // how many comma seperated fields we expect  
TextFinder  finder1(Serial1);                                           
float rpy1[NUMBER_OF_FIELDS];    // array holding values for all the fields

TextFinder  finder2(Serial2);                                           
float rpy2[NUMBER_OF_FIELDS];    // array holding values for all the fields

float curr[2] = {0};
float prev[2] = {0};
/************************************************************/
/*** Setup
/************************************************************/
void setup()
{
  Serial.begin(57600);  // init the Serial port to print the data to PC
  Serial1.begin(57600); // init the Serial1 port to get data from the IMU
  Serial2.begin(57600); // init the Serial1 port to get data from the IMU

  delay(500);

  initIMUs();
}

/************************************************************/
/*** Loop
/************************************************************/
void loop()
{
  // print manager timer
  static unsigned long timer = 0;
  static unsigned long currentTime = 0;

  /************************************************************/
  /*** Request after a specific period for the data
  /************************************************************/
  currentTime = millis();
  if(currentTime - timer >= PERIOD){
    // Request one output frame from the IMU
    // #f only requests one reply, replies are still bound to the internal 20ms (50Hz) time raster.
    // So worst case delay that #f can add is 19.99ms.
    Serial1.write("#f");
    Serial2.write("#f");
    /************************************************************/
    /*** Get the IMU values
    /************************************************************/

    // the current field being received
    int fieldIndex = 0;
    // search the Serial Buffer as long as the header character is found
    boolean found_HeaderChar = finder1.find("H");

    // for IMU1
    if (found_HeaderChar){
      // Get all 3 values (yaw, pitch, roll) from the Serial Buffer
      while(fieldIndex < NUMBER_OF_FIELDS){
        rpy1[fieldIndex++] = finder1.getFloat();
      }      
      prev[0] = curr[0];
      curr[0] = rpy1[0];
    }

    // for IMU2
    fieldIndex = 0;
    found_HeaderChar = finder2.find("H");
    if (found_HeaderChar){
      // Get all 3 values (yaw, pitch, roll) from the Serial Buffer
      while(fieldIndex < NUMBER_OF_FIELDS){
        rpy2[fieldIndex++] = finder2.getFloat();
      }      
      prev[1] = curr[1];
      curr[1] = rpy2[0];
    }
    
    /************************************************************/
    /*** Print out the values
    /*** Format: yaw, pitch, roll, left_Encoder, right_Encoder
    /************************************************************/
    if (found_HeaderChar){
      // print Interval
//      Serial.print(currentTime - timer);
//      Serial.print("ms, ");

      // print IMU values
      // for(fieldIndex=0; fieldIndex < NUMBER_OF_FIELDS; fieldIndex++)
      // {
      //   Serial.print(rpy[fieldIndex]);
      //   Serial.print(",");
      // }
      float yaw = (prev[0]+prev[1]-curr[0]-curr[1])/((float)PERIOD/500.0);
//      float yaw = (prev[0]-curr[0]);
//       float yaw2 = (prev[1]-curr[1]);
      Serial.print("IMU yaw: ");
      Serial.println(yaw);
//      Serial.print("IMU2 yaw: ");
//      Serial.println(yaw2);
    }
    timer = millis();
  }
}

/********************************/
/*** Initialize Functions
/********************************/

void initIMUs(){
  // Output angles in TEXT format & Turn off continuous streaming output & Disable error message output
  Serial1.write("#ot#o0#oe0");
  Serial1.flush();
  Serial2.write("#ot#o0#oe0");
  Serial2.flush();
}

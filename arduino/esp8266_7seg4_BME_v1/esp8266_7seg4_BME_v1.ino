#include <ESP8266WiFi.h>
#include <BME280_MOD-1022.h>
#include <Wire.h>
const char* ssid = "";
const char* password = "";

const char* mHost     = "api.thingspeak.com";
String mAPI_KEY="your-KEY";

//const char* mHostTime = "";

//LCD
#define ADDR 0x3e
uint8_t CONTROLL[] = {
  0x00, // Command
  0x40, // Data
};

const int C_COMM = 0;
const int C_DATA = 1;

const int LINE = 2;
String buff[LINE];
//
int mMode=0;
int mMode_TMP =0;
int mMode_HUM =1;
//int mMode_PRE =2;

float mTemp=0;
float mHumidity=0;
float mPressure=0;

uint32_t mNextHttp= 30000;
uint32_t mTimerTmpInit=30000;
uint32_t mTimerDisp;
uint32_t mTimerTime;
uint32_t mTimerTemp;

//String mReceive="";
String mTimeStr="0000";

// Arduino needs this to pring pretty numbers
void printFormattedFloat(float x, uint8_t precision) {
char buffer[10];

  dtostrf(x, 7, precision, buffer);
  Serial.print(buffer);

}

// print out the measurements
void printCompensatedMeasurements(void) {

//float temp, humidity,  pressure, pressureMoreAccurate;
float  pressureMoreAccurate;
double tempMostAccurate, humidityMostAccurate, pressureMostAccurate;
char buffer[80];

//  temp      = BME280.getTemperature();
//  humidity  = BME280.getHumidity();
//  pressure  = BME280.getPressure();
  mTemp      = BME280.getTemperature();
  mHumidity  = BME280.getHumidity();
  mPressure  = BME280.getPressure();
  
  pressureMoreAccurate = BME280.getPressureMoreAccurate();  // t_fine already calculated from getTemperaure() above
  
  tempMostAccurate     = BME280.getTemperatureMostAccurate();
  humidityMostAccurate = BME280.getHumidityMostAccurate();
  pressureMostAccurate = BME280.getPressureMostAccurate();
  /*
  Serial.println("                Good  Better    Best");
  Serial.print("Temperature  ");
  printFormattedFloat(mTemp, 2);
  Serial.print("         ");
  printFormattedFloat(tempMostAccurate, 2);
  Serial.println();
  
  Serial.print("Humidity     ");
  printFormattedFloat(mHumidity, 2);
  Serial.print("         ");
  printFormattedFloat(humidityMostAccurate, 2);
  Serial.println();

  Serial.print("Pressure     ");
  printFormattedFloat(mPressure, 2);
  Serial.print(" ");
  printFormattedFloat(pressureMoreAccurate, 2);
  Serial.print(" ");
  printFormattedFloat(pressureMostAccurate, 2);
  Serial.println();
  */
}

//
void init_BME280(){
  uint8_t chipID;
  
  //Serial.println("Welcome to the BME280 MOD-1022 weather multi-sensor test sketch!");
  //Serial.println("Embedded Adventures (www.embeddedadventures.com)");
  chipID = BME280.readChipId();
  
  // find the chip ID out just for fun
  Serial.print("ChipID = 0x");
  Serial.print(chipID, HEX);
  
 
  // need to read the NVM compensation parameters
  BME280.readCompensationParams();
  
  // Need to turn on 1x oversampling, default is os_skipped, which means it doesn't measure anything
  BME280.writeOversamplingPressure(os1x);  // 1x over sampling (ie, just one sample)
  BME280.writeOversamplingTemperature(os1x);
  BME280.writeOversamplingHumidity(os1x);
  
  // example of a forced sample.  After taking the measurement the chip goes back to sleep
  BME280.writeMode(smForced);
  while (BME280.isMeasuring()) {
    Serial.println("Measuring...");
    delay(50);
  }
  Serial.println("Done!");
  
  // read out the data - must do this before calling the getxxxxx routines
  BME280.readMeasurements();
  /*
  Serial.print("Temp=");
  Serial.println(BME280.getTemperature());
  Serial.print("Humidity=");
  Serial.println(BME280.getHumidity());
  Serial.print("Pressure=");
  Serial.println(BME280.getPressure());
  Serial.print("PressureMoreAccurate=");
  Serial.println(BME280.getPressureMoreAccurate());
  Serial.print("TempMostAccurate=");
  Serial.println(BME280.getTemperatureMostAccurate());
  Serial.print("HumidityMostAccurate=");
  Serial.println(BME280.getHumidityMostAccurate());
  Serial.print("PressureMostAccurate=");
  Serial.println(BME280.getPressureMostAccurate());
  */  
  // Example for "indoor navigation"
  // We'll switch into normal mode for regular automatic samples  
  BME280.writeStandbyTime(tsb_0p5ms);        // tsb = 0.5ms
  BME280.writeFilterCoefficient(fc_16);      // IIR Filter coefficient 16
  BME280.writeOversamplingPressure(os16x);    // pressure x16
  BME280.writeOversamplingTemperature(os2x);  // temperature x2
  BME280.writeOversamplingHumidity(os1x);     // humidity x1
  
  BME280.writeMode(smNormal);
}

//
//void proc_http(String sTemp, String sHum){
void proc_http(String sTemp, String sHum, String sPre){
//Serial.print("connecting to ");
//Serial.println(mHost);  
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(mHost, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp +"&field2=" + sHum+ "&field3="+sPre;
//      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp; 
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + mHost + "\r\n" + 
        "Connection: close\r\n\r\n");
      delay(10);      
      int iSt=0;
      while(client.available()){
          String line = client.readStringUntil('\r');
//Serial.print(line);
      }    
}
//
void setup() {  
  mTimerTemp= mTimerTmpInit;
  Wire.begin(); // (SDA,SCL)
  Serial.begin(9600);
  Serial.println("# Start-setup");
  // LCD初期化
  //write(C_COMM, settings, sizeof(settings));
  init_BME280();

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);  
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  
}

//
void loop() {
    delay(100);
    while (BME280.isMeasuring()) {
    }
    String sTmp="";
    String sHum="";
    String sPre="";  
    if (millis() > mTimerDisp) {
        mTimerDisp = millis()+ 3000;    
        // read out the data - must do this before calling the getxxxxx routines
        BME280.readMeasurements();
        printCompensatedMeasurements();
        
        //String line_1 ="00:00,";
        //String line_2 ="Temp:";
        int itemp  =(int)mTemp;   
        int iHum   = (int)mHumidity;   
        int iPress = (int)mPressure;          
        //sTmp=String(itemp);
        //sHum=String(iHum);
        //sPre=String(iPress);
Serial.print("dat=");
//dat=002300680970
        char cTemp[4+1]; 
        char cHum[4+1]; 
        char cPres[4+1]; 
        sprintf(cTemp, "%04d", itemp);
        sprintf(cHum, "%04d", iHum);
        sprintf(cPres, "%04d", iPress);
Serial.print(cTemp);
Serial.print(cHum);
Serial.println(cPres);
    } // if_timeOver
    if (millis() > mTimerTemp) {
      mTimerTemp = millis()+ mNextHttp+ mTimerTmpInit;
      int itemp  =(int)mTemp;   
      int iHum   = (int)mHumidity;   
      int iPress = (int)mPressure;          
      sTmp=String(itemp);
      sHum=String(iHum);
      sPre=String(iPress);      
      proc_http(sTmp, sHum, sPre);
      delay(1000);
    }    
}







//define
const int mVoutPin = 0;
const static int mAnode[4] = {9, 10, 11, 12 };
const static int mCathod[7] ={2,3,4,5,6,8,7};

int   mCathod_pin[7]   ={6,4,2,1,0,5,3 };

const int mMaxAnode=4;
int mSTAT =0;
int mSTAT_CONECT=1;
int mSTAT_DISP=2;
int iMode=0;
int mMode_TMP =0;
int mMode_HUM =1;
int mMode_PRES=2;
//int mMode_TIME=2;

int mTempNum=0;
int mHumNum=0;

//static uint32_t mTimerTmp;
String mReceive="";

bool Num_Array[10][8]={
// a b c d e f g dp 
{ 1,1,1,1,1,1,0,0}, //0 :0
{ 0,1,1,0,0,0,0,0}, //1 :1
{ 1,1,0,1,1,0,1,0}, //2 :2
{ 1,1,1,1,0,0,1,0}, //3 :3
{ 0,1,1,0,0,1,1,0}, //4 :4
{ 1,0,1,1,0,1,1,0}, //5 :5
{ 1,0,1,1,1,1,1,0}, //6 :6
{ 1,1,1,0,0,0,0,0}, //7 :7
{ 1,1,1,1,1,1,1,0}, //8 :8
{ 1,1,1,1,0,1,1,0}, //9 :9   
};

//
void setup() {
  Serial.begin(9600);
  //dht.begin();
  Serial.println("# Start-setup");
  mSTAT =mSTAT_CONECT;
  //cathod
  for(int i= 0 ;i< 7;i++){
    pinMode(mCathod[i], OUTPUT);
  }
  for(int j=0 ;j<  mMaxAnode ; j++){
    pinMode(mAnode[j], OUTPUT);
  }
}

//
void NumPrint(int Number){
//Serial.println(Num_Array[iDeg_1][k]);
  for (int i=0; i< 7; i++){
    int iTmp= mCathod_pin[i];
    if(Num_Array[Number][i]==1){
       digitalWrite(mCathod[iTmp ], LOW);
//Serial.print(mCathod[iTmp ]);
    }
    //else{
    //   digitalWrite(mCathod[iTmp ], HIGH );
    //}
    delayMicroseconds( 500 );
    if(Num_Array[Number][i]==1){
       digitalWrite(mCathod[iTmp ], HIGH);
//Serial.print("0");
    }
//Serial.println("");    
  }
}
//
void set_anode(int pinAnode){
  for(int i=0;i <mMaxAnode ; i++){
    if(mAnode[i]==pinAnode){
       digitalWrite(mAnode[ i  ], HIGH);
    }
    else{
       digitalWrite(mAnode[ i  ], LOW);
    }
  }
}
//
//void proc_dispTime(String sTime){
void proc_disp4dig(String sTime, int typMode){
  if(sTime.length() < 4){ return ; }
  String sOne1000= sTime.substring(0,1);
  String sOne100 = sTime.substring(1,2);
  String sOne10  = sTime.substring(2,3);
  String sOne1   = sTime.substring(3,4);
  int iDeg_1000= sOne1000.toInt();  
  int iDeg_100= sOne100.toInt();  
  int iDeg_10 = sOne10.toInt();
  int iDeg_1  = sOne1.toInt();
  if(typMode==mMode_TMP){
     for(int i=0;  i< 100; i++ ){
         //iDeg_1000
         set_anode(mAnode[3]);
         NumPrint(iDeg_1000 );
         //iDeg_100
         set_anode(mAnode[2]);
         NumPrint(iDeg_100);
         // dig-10
         set_anode(mAnode[1]);
         NumPrint(iDeg_10);
         set_anode(mAnode[0]);
         NumPrint(iDeg_1);       
     }
  }else{
    for(int j=0; j<3; j++){
     for(int i=0;  i< 50 ; i++ ){
         //iDeg_1000
         set_anode(mAnode[3]);
         NumPrint(iDeg_1000 );
         //iDeg_100
         set_anode(mAnode[2]);
         NumPrint(iDeg_100);
         // dig-10
         set_anode(mAnode[1]);
         NumPrint(iDeg_10);
         set_anode(mAnode[0]);
         NumPrint(iDeg_1);       
     }
     delay(100);
    }
  } //end. if_typMode
}

//
long convert_Map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//
void proc_uart(){
    String sRecv="";
    while( Serial.available()){
      char c = Serial.read();
      sRecv.concat(c);
//Serial.print("sRecv=");
//Serial.println( sRecv );             
      if(  sRecv.length() >= 16 ){
        String sHead= sRecv.substring(0,4);
        if(sHead =="dat="){
          mReceive=sRecv.substring(4 );
          mSTAT =mSTAT_DISP;
        }
        sRecv="";
        return;
      }
    }      
}
//
void loop() {
  delay( 100 );
  //int iNum=0;
  if(mSTAT == mSTAT_CONECT){
      proc_uart();
  }else if(mSTAT == mSTAT_DISP){
      //String sBuff="";
      if(mReceive.length() >=12){
            String sTemp= mReceive.substring(0,4);
            String sHum = mReceive.substring(4,8);
            String sPres= mReceive.substring(8,12);
          if(iMode== mMode_TMP){
Serial.print("sTemp=");
Serial.println(sTemp);
            proc_disp4dig(sTemp, mMode_TMP);
            iMode=iMode+1;      
          }else if(iMode== mMode_HUM){
Serial.print("sHum=");
Serial.println(sHum);
            proc_disp4dig(sHum, mMode_HUM);
            iMode=iMode+1;      
          }else{
Serial.print("sPres=");
Serial.println(sPres);
            proc_disp4dig(sPres, mMode_PRES);
            iMode=mMode_TMP;
          }        
      }
       mSTAT =mSTAT_CONECT;
  } //if_STAT
} //end_Loop






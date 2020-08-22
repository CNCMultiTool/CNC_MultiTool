#include <SPI.h>
#include <SD.h>
#include <string.h>
#include <PID_v1.h>


struct StepMotorBig {
  //Settings
  int pinENA;
  int pinDIR;
  int pinPUL;

  //endswitches
  int pinNull;
  int pinPositiv;
  float SwitchID;
  
  //processing
  int steps_pmm;//steps pro mm (constant für x y z)  
  long act_step;//actueller position der achse in step
  long soll_step;//
  double act_posi;//actuelle position in mm
  double soll_posi;//soll position in mm
  unsigned long time_pstep;
  unsigned long time_next_step;
  unsigned long step_div; 
  //acc variablen
  double Se;
  double Vm;
  double Vmin;
  double Bm;
  double Sb;
  double Sv;
  double Tmove;
  int sendonce;
};

struct StepMotorBig Xachse;
struct StepMotorBig Yachse;
struct StepMotorBig Zachse;
struct StepMotorBig Eachse;

double acc_max = 0;
double Speed = 10; //mm pro minute
double Speed_use;
double Speed_max = 30;
double Speed_min = 5; //mm pro minute
double Speed_dif;
double Se;
double Vm;
double dist;
double ges_time;
double tb;
double te;
double tv;

unsigned long time_now = micros();
unsigned long cycle_time1 = micros();

int sensorPin = A1;
double bitwertNTC = 0;
double widerstand1=690;                   //Ohm
double bWert =3950;                           // B- Wert vom NTC
double widerstandNTC =0;
double kelvintemp = 273.15;                // 0°Celsius in Kelvin
double Tn=kelvintemp + 25;                 //Nenntemperatur in Kelvin
double Rn = 100000;
double TKelvin = 0;                        //Die errechnete Isttemperatur
double T = 0;
//PID
double Output;
double KP = 35;
double KI = 0.2;
double KD = 750;
double soll_T = 0;
int temprelai = 2;

//PID myPID(&soll_T, &Output, &T, KP, KI, KD,P_ON_M, REVERSE);
PID myPID(&soll_T, &Output, &T, KP, KI, KD,P_ON_E, REVERSE);
unsigned long WindowSize = 100;

int sensorPin_Bed = A0;
double bitwertNTC_Bed = 0;
double widerstand1_Bed=690;                   //Ohm
double bWert_Bed =3950;                           // B- Wert vom NTC
double widerstandNTC_Bed =0;
double kelvintemp_Bed = 273.15;                // 0°Celsius in Kelvin
double Tn_Bed=kelvintemp_Bed + 25;                 //Nenntemperatur in Kelvin
double Rn_Bed = 100000;
double TKelvin_Bed = 0;                        //Die errechnete Isttemperatur
double T_Bed = 0;
double soll_T_Bed = 0;
int temprelai_Bed = 3;

enum eGCodeState{
  GCodeRun,
  GCodePause,
  GCodeStop  
};
enum eComandName{
  FAIL,
  G1,//move
  G9,//stop
  G92,//set position to
  M104,//set hotend temperature
  M140,//set bed temperatur
  Q10,//max Accelaration
  Q20,//max speed
  Q21,//min speed
  Q100,//Execute G-Code if File exist
  Q101,//open file to write G-Code in it
  Q102,//close file after write G-Code
  Q103,//Pause G-Code
  Q104,//Continue G-Code
  Q105,//Abord G-code
  Q106,//get list of File on SD Card
  Q107//Delete File
};
struct sComand {
  eComandName eIndent;
  char cName[6];
};
sComand lComandList[16] = {
  {G1,"G1"},
  {G9,"G9"},
  {G92,"G92"},
  {M104,"M104"},
  {M140,"M140"},
  {Q10,"Q10"},
  {Q20,"Q20"},
  {Q21,"Q21"}, 
  {Q100,"Q100"},//Execute G-Code if File exist
  {Q101,"Q101"},//open file to write G-Code in it
  {Q102,"Q102"},//close file after write G-Code
  {Q103,"Q103"},//Pause G-Code
  {Q104,"Q104"},//Continue G-Code
  {Q105,"Q105"},//Abord G-code
  {Q106,"Q106"},//get list of File on SD Card
  {Q107,"Q107"}//Delete File 
};
char reciveBuf[128];
unsigned int reciveWindex = 0; 

const unsigned long threshold = 4294967295/2;

File myFile;
char * GcodeLine;

bool send_once = true;
int motors_in_move = 0;
eGCodeState GState = GCodeStop;

void setup() {
  // put your setup code here, to run once:
  Xachse.soll_posi = 0;
  Xachse.act_posi = 0;
  Xachse.soll_step = 0;
  Xachse.act_step = 0;
  Xachse.time_next_step = 0;
  Xachse.time_pstep = 0;
  Xachse.steps_pmm = 50;
  Xachse.pinENA = 37;
  Xachse.pinDIR = 39;
  Xachse.pinPUL = 41;
  Xachse.pinPositiv = 44;
  Xachse.pinNull = 46;
  Xachse.SwitchID = 0;

  Yachse.soll_posi = 0;
  Yachse.act_posi = 0;
  Yachse.soll_step = 0;
  Yachse.act_step = 0;
  Yachse.time_next_step = 0;
  Yachse.time_pstep = 0;
  Yachse.steps_pmm = 50;
  Yachse.pinENA = 43;
  Yachse.pinDIR = 45;
  Yachse.pinPUL = 47;
  Yachse.pinPositiv = 40;
  Yachse.pinNull = 42;
  Yachse.SwitchID = 0;

  Zachse.soll_posi = 0;
  Zachse.act_posi = 0;
  Zachse.soll_step = 0;
  Zachse.act_step = 0;
  Zachse.time_next_step = 0;
  Zachse.time_pstep = 0;
  Zachse.steps_pmm = 50;
  Zachse.pinENA = 25;
  Zachse.pinDIR = 27;
  Zachse.pinPUL = 29;
  Zachse.pinPositiv = 36;
  Zachse.pinNull = 38;
  Zachse.SwitchID = 0;
  
  Eachse.soll_posi = 0;
  Eachse.act_posi = 0;
  Eachse.soll_step = 0;
  Eachse.act_step = 0;
  Eachse.time_next_step = 0;
  Eachse.time_pstep = 0;
  Eachse.steps_pmm = 35;
  Eachse.pinENA = 31;
  Eachse.pinDIR = 33;
  Eachse.pinPUL = 35;
    
  pinMode(Xachse.pinENA,OUTPUT);
  pinMode(Xachse.pinDIR,OUTPUT);
  pinMode(Xachse.pinPUL,OUTPUT);
  pinMode(Xachse.pinNull,INPUT_PULLUP);
  pinMode(Xachse.pinPositiv,INPUT_PULLUP);

  pinMode(Yachse.pinENA,OUTPUT);
  pinMode(Yachse.pinDIR,OUTPUT);
  pinMode(Yachse.pinPUL,OUTPUT);
  pinMode(Yachse.pinNull,INPUT_PULLUP);
  pinMode(Yachse.pinPositiv,INPUT_PULLUP);

  pinMode(Zachse.pinENA,OUTPUT);
  pinMode(Zachse.pinDIR,OUTPUT);
  pinMode(Zachse.pinPUL,OUTPUT);
  pinMode(Zachse.pinNull,INPUT_PULLUP);
  pinMode(Zachse.pinPositiv,INPUT_PULLUP);
  
  pinMode(Eachse.pinENA,OUTPUT);
  pinMode(Eachse.pinDIR,OUTPUT);
  pinMode(Eachse.pinPUL,OUTPUT);

  pinMode(temprelai,OUTPUT);
  pinMode(temprelai_Bed,OUTPUT);
  myPID.SetOutputLimits(0, 255);
  myPID.SetSampleTime(WindowSize);
  myPID.SetMode(AUTOMATIC);

  pinMode(22,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(28,OUTPUT);
  digitalWrite(22,LOW);
  digitalWrite(24,LOW);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(53)) {
    while (true) {
      Serial.println("SD initialization failed!");
    }
  }
  else
  {
    Serial.println("SD initializ");
  }

  //if(SD.exists("test.txt")){
  //  SD.remove("test.txt");
  //}
  //myFile = SD.open("test.txt", FILE_WRITE);
  //myFile.println("G92 X0 Y0 Z0 E0");
  //myFile.close();
  //if(myFile) {
  Serial.println("RESTART Arduino compleated");
}

void loop(){
  // put your main code here, to run repeatedly:
  time_now = micros();
  checkCommandFromSerial();

  TempControle();
  
  checkEndswitches();
  //motors drive
  motors_in_move = 0;
  motors_in_move += treiberBig(Xachse);
  motors_in_move += treiberBig(Yachse);
  motors_in_move += treiberBig(Zachse);
  motors_in_move += treiberBig(Eachse);

  if(motors_in_move == 0){
    if(send_once == false){
      send_once = true;
      Serial.print("X: ");
      Serial.print(Xachse.soll_posi);
      Serial.print(" Y: ");
      Serial.print(Yachse.soll_posi);
      Serial.print(" Z: ");
      Serial.print(Zachse.soll_posi);
      Serial.print(" E: ");
      Serial.print(Eachse.soll_posi);
      Serial.print(" F: ");
      Serial.println(Speed);
    }
  }
  //read next G-Code Line from File if exist
  if(GState == GCodeRun && motors_in_move == 0)
  {
    executeNextGCodeLine(SDreadLine());
  }
}
bool is_time_over(unsigned long value){
  if(value < threshold && time_now > threshold)
  {
    return 0;
  }
  if(time_now >= value)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
//check if one endswitch had changed
float checkEndswitches(){
  float newXSwitchID = checkEndswitch(Xachse);
  float newYSwitchID = checkEndswitch(Yachse);
  float newZSwitchID = checkEndswitch(Zachse);
  if(Xachse.SwitchID != newXSwitchID || Yachse.SwitchID != newYSwitchID || Zachse.SwitchID != newZSwitchID){
    Xachse.SwitchID = newXSwitchID;
    Yachse.SwitchID = newYSwitchID;
    Zachse.SwitchID = newZSwitchID;
  }
}
float checkEndswitch(struct StepMotorBig &StepM){
  float switchID;
  if(digitalRead(StepM.pinNull)==HIGH&& StepM.soll_step <= StepM.act_step){
    StepM.soll_step = StepM.act_step;
    StepM.act_posi = (double)StepM.act_step/StepM.steps_pmm;
    switchID = -1;
  }else if(digitalRead(StepM.pinPositiv)==HIGH&& StepM.soll_step >= StepM.act_step){
    StepM.soll_step = StepM.act_step;
    StepM.act_posi = (double)StepM.act_step/StepM.steps_pmm;
    switchID = 1;
  }else{
    switchID = 0;
  }
  return(switchID);
}
void TempControle(){
  if(myPID.Compute())
  {
    bitwertNTC = analogRead(sensorPin);      // lese Analogwert an A0 aus
    widerstandNTC = widerstand1*(((double)bitwertNTC/1024)/(1-((double)bitwertNTC/1024)));
    TKelvin = 1/((1/Tn)+((double)1/bWert)*log((double)widerstandNTC/Rn));
    T = TKelvin-kelvintemp;
    analogWrite(temprelai,Output);
    if(is_time_over(cycle_time1))
    {
      if(abs(T-soll_T)>2)
      {
        cycle_time1 = time_now + 1000000;
      }
      else
      {
        cycle_time1 = time_now + 5000000;
      }
      
      bitwertNTC_Bed = analogRead(sensorPin_Bed);      // lese Analogwert an A0 aus
      widerstandNTC_Bed = widerstand1_Bed*(((double)bitwertNTC_Bed/1024)/(1-((double)bitwertNTC_Bed/1024)));
      TKelvin_Bed = 1/((1/Tn_Bed)+((double)1/bWert_Bed)*log((double)widerstandNTC_Bed/Rn_Bed));
      T_Bed = TKelvin_Bed-kelvintemp_Bed;
      if(soll_T_Bed+5 < T_Bed)
      {
        digitalWrite(temprelai_Bed,HIGH);
      }
      else if(soll_T_Bed-5 > T_Bed)
      {
        digitalWrite(temprelai_Bed,LOW);
      }
    }
  }    
}
//strops the movement (set soll pos equal act pos)
void act_equal_soll(){
  Xachse.soll_step = Xachse.act_step;
  Yachse.soll_step = Yachse.act_step;
  Zachse.soll_step = Zachse.act_step;
  Eachse.soll_step = Eachse.act_step;
  act_steps_to_act_posi();
  Xachse.soll_posi = Xachse.act_posi;
  Yachse.soll_posi = Yachse.act_posi;
  Zachse.soll_posi = Zachse.act_posi;
  Eachse.soll_posi = Eachse.act_posi;
}
void act_steps_to_act_posi(){
  Xachse.act_posi = (double)Xachse.act_step/Xachse.steps_pmm;
  Yachse.act_posi = (double)Yachse.act_step/Yachse.steps_pmm;
  Zachse.act_posi = (double)Zachse.act_step/Zachse.steps_pmm;
  Eachse.act_posi = (double)Eachse.act_step/Eachse.steps_pmm;
}
void act_posi_to_act_steps(){
  Xachse.act_step = (double)Xachse.act_posi*Xachse.steps_pmm;
  Yachse.act_step = (double)Yachse.act_posi*Yachse.steps_pmm;
  Zachse.act_step = (double)Zachse.act_posi*Zachse.steps_pmm;
  Eachse.act_step = (double)Eachse.act_posi*Eachse.steps_pmm;
}
void setPose(){
  Xachse.soll_posi = Xachse.act_posi;
  Yachse.soll_posi = Yachse.act_posi;
  Zachse.soll_posi = Zachse.act_posi;
  Eachse.soll_posi = Eachse.act_posi;
  act_posi_to_act_steps();
  Xachse.soll_step = Xachse.act_step;
  Yachse.soll_step = Yachse.act_step;
  Zachse.soll_step = Zachse.act_step;
  Eachse.soll_step = Eachse.act_step;
}
//berechnet die  parameter für eine bewegung für eine achse
void getMoveParams(){
  dist = sqrt(pow(Xachse.soll_posi-Xachse.act_posi,2)+pow(Yachse.soll_posi-Yachse.act_posi,2)+pow(Zachse.soll_posi-Zachse.act_posi,2)); //gesamtdistans
  ges_time = (dist/Speed)*1000000.0;
  if(dist == 0){
    dist = abs(Eachse.soll_posi-Eachse.act_posi);
    ges_time = (dist/Speed)*1000000.0;
  }

  if(acc_max!=0 && Speed>Speed_min)
  {
    //get longest singel dist
    Se = abs(Xachse.soll_posi-Xachse.act_posi);
    if(Se<abs(Yachse.soll_posi-Yachse.act_posi))
      Se = abs(Yachse.soll_posi-Yachse.act_posi);
    if(Se<abs(Zachse.soll_posi-Zachse.act_posi))
      Se = abs(Zachse.soll_posi-Zachse.act_posi);
    if(Se<abs(Eachse.soll_posi-Eachse.act_posi))
      Se = abs(Eachse.soll_posi-Eachse.act_posi);

    Speed_use = (Se*1000000.0)/ges_time;
    Speed_dif = Speed_use - Speed_min;
    tb = Speed_dif/acc_max;
    te = Se/Speed_use+tb;
    tv = te - tb;
  }
  BigM_move_params(Xachse);
  BigM_move_params(Yachse);
  BigM_move_params(Zachse);
  BigM_move_params(Eachse); 
}
void BigM_move_params(struct StepMotorBig &StepM){
  StepM.soll_step = StepM.soll_posi*float(StepM.steps_pmm);
  StepM.step_div = abs(StepM.soll_step-StepM.act_step);
  StepM.time_pstep = ges_time/StepM.step_div;
  StepM.time_next_step = time_now;
  if(acc_max!=0 && Speed_use>Speed_min)
  {
    StepM.Se = abs(StepM.soll_posi-StepM.act_posi);
    StepM.Vmin = (Speed_min/Se) * StepM.Se;
    StepM.Vm = StepM.Se/tv-StepM.Vmin;
    StepM.Bm = StepM.Vm/tb;
    StepM.Sb = StepM.Bm*pow(tb,2)/2+StepM.Vmin*tb;
    StepM.Sv = StepM.Se-StepM.Sb;
    StepM.Tmove = time_now;
    StepM.sendonce = 0;
  }
}
unsigned long timeNextStep(struct StepMotorBig &StepM){
  unsigned long t_p_step = StepM.time_pstep;
  double V;
  if(acc_max==0 || Speed_use<=Speed_min)
    t_p_step = StepM.time_pstep;
  else
  {
    double S = ((float)abs(StepM.step_div) - (float)abs(StepM.soll_step-StepM.act_step))/(float)StepM.steps_pmm;
    if(S <= StepM.Sb && S <= (StepM.Se/2))
      V = StepM.Vmin + ((StepM.Vm)/StepM.Sb)*S;
    //else if(S > StepM.Sb && S < StepM.Sv)
    //  V = StepM.Vmin + StepM.Vm;
    else if(S >= StepM.Sv && S > (StepM.Se/2))
      V = StepM.Vmin + StepM.Vm - (StepM.Vm/StepM.Sb)*(S-StepM.Sv);
    else
      V = StepM.Vmin + StepM.Vm;
    t_p_step = 1000000.0/(StepM.steps_pmm*V);
  }
  return t_p_step;
}
///////////////////////////////////führt einen schrit aus und gibt den aktuellen an////////////////////////////7
int treiberBig(struct StepMotorBig &StepM){
  if(StepM.soll_step!=StepM.act_step){
    if(is_time_over(StepM.time_next_step)){
      if(StepM.soll_step>StepM.act_step){
        //one stapp forward
        digitalWrite(StepM.pinENA,LOW);
        digitalWrite(StepM.pinDIR,HIGH);
        digitalWrite(StepM.pinPUL,LOW);
        digitalWrite(StepM.pinPUL,HIGH);
        StepM.act_step++;
        StepM.time_next_step += timeNextStep(StepM);
      }else if(StepM.soll_step<StepM.act_step){
        //one stepp back
        digitalWrite(StepM.pinENA,LOW);
        digitalWrite(StepM.pinDIR,LOW);
        digitalWrite(StepM.pinPUL,LOW);
        digitalWrite(StepM.pinPUL,HIGH);
        StepM.act_step--;
        StepM.time_next_step += timeNextStep(StepM);
      }else if(is_time_over(StepM.time_next_step+500000)){
        //turn motor off
        digitalWrite(StepM.pinENA,HIGH);
      }else{
        //turn motor off
        digitalWrite(StepM.pinPUL,HIGH);
        digitalWrite(StepM.pinDIR,LOW);
      }
    }
    return 1;
  }
  return 0;
}
//read G-Code from SD-Card
int open_file(char fileName[]){
  *strchr(fileName,'\n') = '\0';
  char* newFileName = fileName+5;
  if(!SD.exists(newFileName))
    return -1;
  myFile = SD.open(newFileName);
  if(myFile)
    return 0;
  else
    return -1;
}
int create_file(char fileName[]){
  *strchr(fileName,'\n') = '\0';
  char* newFileName = fileName+5;
  if(SD.exists(newFileName))
    SD.remove(newFileName);
  myFile = SD.open(newFileName, FILE_WRITE);

  myFile.println("G92 X0 Y0 Z0 E0");
  myFile.println("G1 X10 Y0 Z0 E0");
  myFile.println("G1 X0 Y10 Z0 E0");
  myFile.println("G1 X0 Y0 Z10 E0");
  myFile.println("G1 X0 Y0 Z0 E10");
    
  if(myFile)
    return 0;
  else
    return -1;
}
int close_file(){
    myFile.close();
    return 0;
}
int remove_file(char fileName[]){
  *strchr(fileName,'\n') = '\0';
  char* newFileName = fileName+5;
  if(SD.exists(newFileName)){
    SD.remove(newFileName);
    return 0;
  }
  return -1;
}
void printDirectory(){
  File dir;
  int numTabs = 1;
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    //if (entry.isDirectory()) {
     // Serial.println("/");
     // printDirectory(entry, numTabs + 1);
    //} else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    //}
    entry.close();
  }
}
char* SDreadLine(){
  static char newLine[128];
  newLine[0] = '\0';
  if(myFile)
  {
    int write_idx = 0;
    char read_buf;
    while(myFile.available())
    {
      read_buf = myFile.read();
      if(read_buf == '\n')//detekt end of line
      {
        newLine[write_idx] = '\0';//end of string
        return newLine;
      }else if(read_buf == ';'){//remove comments out of the G-Code
        while(read_buf != '\n')
        {
          read_buf = myFile.read();
        }
        newLine[write_idx] = '\0';
        return newLine;      
      }
      else
      {//read chars from file and safe it
        newLine[write_idx] = read_buf;
        write_idx++;
      }
    }
  }
  else
  {
    GState = GCodeStop;
    return newLine;
  }
  return newLine;
}
void checkCommandFromSerial(){
  //read a G-Code line from the Serial and execute it
  char caLine[1];
  if(Serial.available())
  {
    Serial.readBytes(caLine,1);
    //Serial.print(caLine);
    reciveBuf[reciveWindex] = caLine[0];
    reciveWindex++;
    if(caLine[0] == '\n')
    {
      caLine[0] = ' ';
      reciveWindex = 0;
      //Serial.print("checkCommandFromSerial ");
      //Serial.print(reciveBuf);
      //Serial.println(strlen(reciveBuf));
      executeNextGCodeLine(reciveBuf);
      memset(&reciveBuf[0], 0, sizeof(reciveBuf));
    }
  }
}
eComandName ComandParser(char* command_line){
  //look for a G-Code Comand in Line
  char wort[10];
  char* command_end = strchr(command_line,' ');
  if(command_end == 0)
    return FAIL;
  int command_length = command_end - command_line;
  strncpy(wort, command_line, command_length);
  wort[command_length] = '\0';
  //Serial.print("wort ");
  //Serial.println(wort);
  for(int i = 0;i<sizeof(lComandList)/sizeof(lComandList[0]);i++)
  {
    if(strcmp(lComandList[i].cName,wort)==0)
    {
      //Serial.print("ComandParser: find ");
      //Serial.println(lComandList[i].cName);
      return lComandList[i].eIndent;
    }
  }
  //Serial.println("ComandParser: find no comand");
  return FAIL;
}
void LineParser(char* command_line,double* X,double* Y,double* Z,double* E,double* S,double* F){
  //cheack for values in the G-Code Line
  char trennzeichen[] = " ";
  char *wort;
  //Serial.print("LineParser ");
  //Serial.println(command_line);
  wort = strtok(command_line, trennzeichen);  
  while(wort != NULL){
    switch(wort[0]){
      case 'X':    
        wort++;
        *X = atof(wort);
        //Serial.print("LineParser X ");
        //Serial.println(*X);
        break;
      case 'Y':
        wort++;
        *Y = atof(wort);
        break;
      case 'Z':
        wort++;
        *Z = atof(wort);
        break;
      case 'E':
        wort++;
        *E = atof(wort);
        break;
      case 'S':
        wort++;
        *S = atof(wort);
        break;
      case 'F':
        wort++;
        *F = atof(wort);
        break;
    }
    wort = strtok(NULL, trennzeichen);
  }
}
void executeNextGCodeLine(char* GLine){
  double x = 0,y = 0,z = 0,e = 0,s = 0,f = 0;
  char * GLineCopy;
  //Serial.print("executeNextGCodeLine ");
  //Serial.println(GLine);
  switch(ComandParser(GLine))
  {
    case G1://G1 move
      Serial.println("executeNextGCodeLine: G1 found");
      LineParser(GLine,&Xachse.soll_posi,&Yachse.soll_posi,
        &Zachse.soll_posi,&Eachse.soll_posi,&s,&Speed);
      getMoveParams();
      send_once = false;
      break;
    case G9://G9 stop
      Serial.println("executeNextGCodeLine: G9 found");
      act_equal_soll();
      send_once = false;
      break;
    case G92://G92 set positioning
      Serial.println("executeNextGCodeLine: G92 found");
      LineParser(GLine,&Xachse.act_posi,&Yachse.act_posi,
        &Zachse.act_posi,&Eachse.act_posi,&s,&f);
      setPose();
      send_once = false;
      break;
    case M104://M104 set hotend temperatur
      Serial.println("executeNextGCodeLine: M104 found");
      LineParser(GLine,&x,&y,&z,&e,&soll_T,&f);
      break;
    case M140://M140 set hotend temperatur
      Serial.println("executeNextGCodeLine: M140 found");
      LineParser(GLine,&x,&y,&z,&e,&soll_T_Bed,&f);
      break;  
    case Q10://Q10 set max acc
      Serial.println("executeNextGCodeLine: Q10 found");
      LineParser(GLine,&x,&y,&z,&e,&acc_max,&f);
      break;
    case Q20://Q20 set max vcc
      Serial.println("executeNextGCodeLine: Q20 found");
      LineParser(GLine,&x,&y,&z,&e,&Speed_max,&f);
      break;
    case Q21://Q21 set min vcc
      Serial.println("executeNextGCodeLine: Q21 found");
      LineParser(GLine,&x,&y,&z,&e,&Speed_min,&f);
      break;
    case Q100://Execute G-Code if File exist
      Serial.println("executeNextGCodeLine: Q100 found");
      Serial.println(GLine);
      if(open_file(GLine)==0){GState = GCodeRun;}
      break;
    case Q101://open file to write G-Code in it
      Serial.println("executeNextGCodeLine: Q101 found");
      create_file(GLine);
      break;
    case Q102://close file after write G-Code
      Serial.println("executeNextGCodeLine: Q102 found");
      close_file();
      break;
    case Q103://Pause G-Code
      Serial.println("executeNextGCodeLine: Q103found");
      if(GState == GCodeRun){GState = GCodePause;}
      break;
    case Q104://Continue G-Code
      Serial.println("executeNextGCodeLine: Q104found");
      if(GState == GCodePause){GState = GCodeRun;}
      break;
    case Q105://Abord G-code
      Serial.println("executeNextGCodeLine: Q105 found");
      GState = GCodeStop;
      close_file();
      break;
    case Q106://get list of File on SD Card
      Serial.println("executeNextGCodeLine: Q106 found");
      printDirectory();
      break;
    case Q107://Delete File
      Serial.println("executeNextGCodeLine: Q107 found");
      remove_file(GLine+5);
      break;
    case FAIL:
      Serial.println("executeNextGCodeLine: no command found FAIL");
    default:
      Serial.println("executeNextGCodeLine: no command found");  
  }
}

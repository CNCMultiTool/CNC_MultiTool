#include <SPI.h>
#include <SD.h>
#include <string.h>
#include <PID_v1.h>
#include <math.h>


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
  //unsigned long time_next_step;
  unsigned long time_last_step;
  unsigned long step_div;
  //acc variablen
  double Se;
  double Vsoll;
  double Vmin;
  double Bm;
  double Sb;
  double Sv;
  double f;
};

struct timeing {
  unsigned long Start;
  unsigned long Min;
  unsigned long Max;
  unsigned long Average;
  unsigned long Count;
  unsigned long CntOvrAve;
  unsigned long CntOvrDoubleAve;
};

struct values {
  double X = 0;
  bool useX = false;
  double Y = 0;
  bool useY = false;
  double Z = 0;
  bool useZ = false;
  double E = 0;
  bool useE = false;
  double F = 0;
  bool useF = false;
  double S = 0;
  bool useS = false;
};

timeing cicle, cicle1;
values newValue;

struct StepMotorBig Xachse;
struct StepMotorBig Yachse;
struct StepMotorBig Zachse;
struct StepMotorBig Eachse;

double SeGes;
double threshAngle = 45;
double BmMax = 0;//maximale beschleinigung
double BmNext = 0;//beschleinigung zwischenspeicher für die nächste bewegung
double BmGes = 0;//verwendete beschleuningung für nächsten move
double Vmin = 5;
double Vsoll = 20;
double Vmax = 30;
double ges_time;
double tb;
double te;
double tv;

bool waitForHeat = false;

unsigned long time_now = micros();
unsigned long time_old = micros();
unsigned long cycle_time1 = micros();

int sensorPin = A1;
double bitwertNTC = 0;
double widerstand1 = 690;                 //Ohm
double bWert = 3950;                          // B- Wert vom NTC
double widerstandNTC = 0;
double kelvintemp = 273.15;                // 0°Celsius in Kelvin
double Tn = kelvintemp + 25;               //Nenntemperatur in Kelvin
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
PID myPID(&soll_T, &Output, &T, KP, KI, KD, P_ON_E, REVERSE);
unsigned long WindowSize = 100;

int sensorPin_Bed = A0;
double bitwertNTC_Bed = 0;
double widerstand1_Bed = 690;                 //Ohm
double bWert_Bed = 3950;                          // B- Wert vom NTC
double widerstandNTC_Bed = 0;
double kelvintemp_Bed = 273.15;                // 0°Celsius in Kelvin
double Tn_Bed = kelvintemp_Bed + 25;               //Nenntemperatur in Kelvin
double Rn_Bed = 100000;
double TKelvin_Bed = 0;                        //Die errechnete Isttemperatur
double T_Bed = 0;
double soll_T_Bed = 0;
int temprelai_Bed = 3;

enum eGCodeState {
  GCodeRun = 0,
  GCodePause = 1,
  GCodeStop =  2,
  GCodeCreate = 3
};
enum eComandName {
  FAIL,
  XXX,
  G1,//move
  G9,//stop
  G28,//move home
  G92,//set position to
  M104,//set hotend temperature
  M140,//set bed temperatur
  M109,//set hotend temperature
  M190,//set bed temperatur
  Q10,//max Accelaration//acc- max- min- speed
  Q20,//max speed//bitwertNTC widerstandNTC widerstand1   for hotend
  Q21,//min speed//P I D ON for hotend
  Q30,//min speed//bitwertNTC widerstandNTC widerstand1   for bed
  Q100,//Execute G-Code if File exist
  Q101,//open file to write G-Code in it
  Q102,//close file after write G-Code
  Q103,//Pause G-Code
  Q104,//Continue G-Code
  Q105,//Abord G-code
  Q106,//get list of File on SD Card
  Q107,//Delete File
};
struct sComand {
  eComandName eIndent;
  char cName[6];
};
sComand lComandList[21] = {
  {G1, "G1"},
  {G9, "G9"},
  {G28, "G28"},
  {G92, "G92"},
  {M104, "M104"},
  {M140, "M140"},
  {M109, "M109"},
  {M190, "M190"},
  {Q10, "Q10"},
  {Q20, "Q20"},
  {Q21, "Q21"},
  {Q30, "Q30"},
  {Q100, "Q100"}, //Execute G-Code if File exist
  {Q101, "Q101"}, //open file to write G-Code in it
  {Q102, "Q102"}, //close file after write G-Code
  {Q103, "Q103"}, //Pause G-Code
  {Q104, "Q104"}, //Continue G-Code
  {Q105, "Q105"}, //Abord G-code
  {Q106, "Q106"}, //get list of File on SD Card
  {Q107, "Q107"}, //Delete File
  {XXX, "XXX"}
};
char reciveBuf[128];
char *myStr;
unsigned int reciveWindex = 0;

const unsigned long threshold = 4294967295 / 2;

File GFile;
File HomeFile;

bool moveHome = false;
bool useEndSwitch = false;
//char * GcodeLine;

bool doneStep = false;
int prepSteps = 0;
eComandName newComand;

bool send_once = true;
int motors_in_move = 0;
eGCodeState GState = GCodeStop;

void setup() {
  // put your setup code here, to run once:
  Xachse.soll_posi = 0;
  Xachse.act_posi = 0;
  Xachse.soll_step = 0;
  Xachse.act_step = 0;
  Xachse.time_last_step = 0;
  Xachse.time_pstep = 0;
  Xachse.steps_pmm = 50;
  Xachse.pinENA = 37;//PC0
  Xachse.pinDIR = 39;//PG2
  Xachse.pinPUL = 41;//PG0
  Xachse.pinPositiv = 44;
  Xachse.pinNull = 46;
  Xachse.SwitchID = 0;

  Yachse.soll_posi = 0;
  Yachse.act_posi = 0;
  Yachse.soll_step = 0;
  Yachse.act_step = 0;
  Yachse.time_last_step = 0;
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
  Zachse.time_last_step = 0;
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
  Eachse.time_last_step = 0;
  Eachse.time_pstep = 0;
  Eachse.steps_pmm = 35;
  Eachse.pinENA = 31;
  Eachse.pinDIR = 33;
  Eachse.pinPUL = 35;

  pinMode(Xachse.pinENA, OUTPUT);
  pinMode(Xachse.pinDIR, OUTPUT);
  pinMode(Xachse.pinPUL, OUTPUT);
  pinMode(Xachse.pinNull, INPUT_PULLUP);
  pinMode(Xachse.pinPositiv, INPUT_PULLUP);

  pinMode(Yachse.pinENA, OUTPUT);
  pinMode(Yachse.pinDIR, OUTPUT);
  pinMode(Yachse.pinPUL, OUTPUT);
  pinMode(Yachse.pinNull, INPUT_PULLUP);
  pinMode(Yachse.pinPositiv, INPUT_PULLUP);

  pinMode(Zachse.pinENA, OUTPUT);
  pinMode(Zachse.pinDIR, OUTPUT);
  pinMode(Zachse.pinPUL, OUTPUT);
  pinMode(Zachse.pinNull, INPUT_PULLUP);
  pinMode(Zachse.pinPositiv, INPUT_PULLUP);

  pinMode(Eachse.pinENA, OUTPUT);
  pinMode(Eachse.pinDIR, OUTPUT);
  pinMode(Eachse.pinPUL, OUTPUT);

  pinMode(temprelai, OUTPUT);
  pinMode(temprelai_Bed, OUTPUT);
  myPID.SetOutputLimits(0, 255);
  myPID.SetSampleTime(WindowSize);
  myPID.SetMode(AUTOMATIC);

  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(28, OUTPUT);
  digitalWrite(22, LOW);
  digitalWrite(24, LOW);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  while(!SD.begin(53)) {
      Serial.println("SD initialization failed!");
      delay(1000);
  }
  Serial.println("SD initializ");

  Serial.println("RESTART Arduino compleated");
}
void loop() {
  // put your main code here, to run repeatedly:
  time_now = micros();
  //read Line From Serial
  checkCommandFromSerial();
  checkEndswitches();
  //motors drive
  motors_in_move = 0;
  motors_in_move += treiberBig(Xachse);
  motors_in_move += treiberBig(Yachse);
  motors_in_move += treiberBig(Zachse);
  motors_in_move += treiberBig(Eachse);

  if (motors_in_move == 0) {
    TempControle();
    if (send_once == false) {
      //timeDeb(&cicle,0,1);
      send_once = true;
      act_steps_to_act_posi();
      if (GState != GCodeRun || moveHome == true) {
        Serial.print("posX");
        Serial.print(Xachse.act_posi);
        Serial.print(" Y");
        Serial.print(Yachse.act_posi);
        Serial.print(" Z");
        Serial.print(Zachse.act_posi);
        Serial.print(" E");
        Serial.print(Eachse.act_posi);
        Serial.print(" F");
        Serial.println(Vsoll);
      }
    }
  }

  //read next G-Code Line from File if exist
  if (GState == GCodeRun && waitForHeat == false) 
  {
    if (doneStep || motors_in_move == 0)
    {
      switch (prepSteps)
      {
        case 0:
          if (moveHome == true) {
            myStr = SDreadLine(HomeFile);
            if (myStr[0] == '@') {
              Serial.println("SDreadLine: HomeFile Finish");
              moveHome = false;
              useEndSwitch = false;
              close_file(HomeFile);
            }
          } else {
            myStr = SDreadLine(GFile);
            if (myStr[0] == '@') {
              Serial.println("SDreadLine: GCode Finish");
              GState = GCodeStop;
              close_file(GFile);
              useEndSwitch = false;
            }
          }
          prepSteps = 1;
          break;
        case 1:
          newComand = ComandParser(myStr);
          prepSteps = 2;
          break;
        case 2:
          LineParser(myStr, &newValue);
          prepSteps = 3;
          break;
        case 3:
          BmNext = calcAcc(&newValue, newComand);
          prepSteps = 4;
          break;
      }
      if (prepSteps == 4 && motors_in_move == 0)
      {
        BmGes = BmNext;
        executeNextGCodeLine(myStr, &newValue, newComand);
        prepSteps = 0;
      }
    }
  } else {
    if (abs(soll_T - T) < 5 && waitForHeat) {
      waitForHeat = false;
      Serial.print("Reach heat ");
      Serial.println(soll_T);
    }
  }
}

float checkEndswitches() {
  //check if one endswitch had changed
  if (useEndSwitch == true)
  {
    float newXSwitchID = checkEndswitch(Xachse);
    float newYSwitchID = checkEndswitch(Yachse);
    float newZSwitchID = checkEndswitch(Zachse);
    if (Xachse.SwitchID != newXSwitchID || Yachse.SwitchID != newYSwitchID || Zachse.SwitchID != newZSwitchID) {
      Xachse.SwitchID = newXSwitchID;
      Yachse.SwitchID = newYSwitchID;
      Zachse.SwitchID = newZSwitchID;
      Serial.print("ES X");
      Serial.print(Xachse.SwitchID);
      Serial.print(" Y");
      Serial.print(Yachse.SwitchID);
      Serial.print(" Z");
      Serial.println(Zachse.SwitchID);
    }
  }
}
float checkEndswitch(struct StepMotorBig &StepM) {
  float switchID;
  if (digitalRead(StepM.pinNull) == HIGH && StepM.soll_step <= StepM.act_step) {
    StepM.soll_step = StepM.act_step;
    StepM.act_posi = (double)StepM.act_step / StepM.steps_pmm;
    switchID = -1;
  } else if (digitalRead(StepM.pinPositiv) == HIGH && StepM.soll_step >= StepM.act_step) {
    StepM.soll_step = StepM.act_step;
    StepM.act_posi = (double)StepM.act_step / StepM.steps_pmm;
    switchID = 1;
  } else {
    switchID = 0;
  }
  return (switchID);
}
void TempControle() {
  if (myPID.Compute())
  {
    bitwertNTC = analogRead(sensorPin);      // lese Analogwert an A0 aus
    widerstandNTC = widerstand1 * (((double)bitwertNTC / 1024) / (1 - ((double)bitwertNTC / 1024)));
    TKelvin = 1 / ((1 / Tn) + ((double)1 / bWert) * log((double)widerstandNTC / Rn));
    T = TKelvin - kelvintemp;
    analogWrite(temprelai, Output);
    if(time_now - cycle_time1 > 5000000)
    {
      if (soll_T != 1) {
        Serial.print("Temp THsoll");
        Serial.print(soll_T);
        Serial.print(" THist");
        Serial.print(T);
        Serial.print(" TBsoll");
        Serial.print(soll_T_Bed);
        Serial.print(" TBist");
        Serial.println(T_Bed);
      }
      cycle_time1 = time_now;

      bitwertNTC_Bed = analogRead(sensorPin_Bed);      // lese Analogwert an A0 aus
      widerstandNTC_Bed = widerstand1_Bed * (((double)bitwertNTC_Bed / 1024) / (1 - ((double)bitwertNTC_Bed / 1024)));
      TKelvin_Bed = 1 / ((1 / Tn_Bed) + ((double)1 / bWert_Bed) * log((double)widerstandNTC_Bed / Rn_Bed));
      T_Bed = TKelvin_Bed - kelvintemp_Bed;
      if (soll_T_Bed + 5 < T_Bed)
      {
        digitalWrite(temprelai_Bed, HIGH);
      }
      else if (soll_T_Bed - 5 > T_Bed)
      {
        digitalWrite(temprelai_Bed, LOW);
      }
    }
  }
}
void act_equal_soll() {
  //strops the movement (set soll pos equal act pos)
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
void act_steps_to_act_posi() {
  Xachse.act_posi = double(Xachse.act_step) / double(Xachse.steps_pmm);
  Yachse.act_posi = double(Yachse.act_step) / double(Yachse.steps_pmm);
  Zachse.act_posi = double(Zachse.act_step) / double(Zachse.steps_pmm);
  Eachse.act_posi = double(Eachse.act_step) / double(Eachse.steps_pmm);
}
void setPose(values *newValue) {
  if (newValue->useX){
    Xachse.soll_posi = Xachse.act_posi;
    Xachse.act_step = Xachse.act_posi * Xachse.steps_pmm;
    Xachse.soll_step = Xachse.act_step;
  }
  if (newValue->useY){
    Yachse.soll_posi = Yachse.act_posi;
    Yachse.act_step = Yachse.act_posi * Yachse.steps_pmm;
    Yachse.soll_step = Yachse.act_step;
  }
  if (newValue->useZ){
    Zachse.soll_posi = Zachse.act_posi;
    Zachse.act_step = Zachse.act_posi * Zachse.steps_pmm;
    Zachse.soll_step = Zachse.act_step;
  }
  if (newValue->useE){
    Eachse.soll_posi = Eachse.act_posi;
    Eachse.act_step = Eachse.act_posi * Eachse.steps_pmm;
    Eachse.soll_step = Eachse.act_step;
  }
}
double calcAcc(values *newValue, eComandName comand) {
  if (newValue->Z != Zachse.soll_posi || newValue->Z != Zachse.act_posi)
  {
    //Serial.println("use BmMax (Z)");
    return BmMax;
  }
  if (comand == G1) {
    float x1 = newValue->X - Xachse.soll_posi;
    float x2 = Xachse.soll_posi - Xachse.act_posi;
    float y1 = newValue->Y - Yachse.soll_posi;
    float y2 = Yachse.soll_posi - Yachse.act_posi;
    float angle = abs((atan2(y2, x2) - atan2(y1, x1)) * (180.0 / M_PI));
    if (angle > threshAngle && angle < (360 - threshAngle)) {
      //Serial.println("use BmMax (Big)");
      return BmMax;
    } else {
      //Serial.println("use 0 acc");
      return 0;
    }
  } else {
    //Serial.println("command not G1");
    return BmMax;
  }
  //Serial.println("come to end");
  return BmMax;
}
void getMoveParams() {
  //berechnet die  parameter für eine bewegung für eine achse
  if (Vmax < Vsoll) {
    Vsoll = Vmax;
  }
  SeGes = sqrt(pow(Xachse.soll_posi - Xachse.act_posi, 2) + pow(Yachse.soll_posi - Yachse.act_posi, 2) + pow(Zachse.soll_posi - Zachse.act_posi, 2)); //gesamtdistans
  ges_time = (SeGes / Vsoll) * 1000000.0;
  if (SeGes == 0) {
    SeGes = abs(Eachse.soll_posi - Eachse.act_posi);
    ges_time = (SeGes / Vsoll) * 10000000.0;
  }
  if (BmGes != 0 && Vsoll > Vmin) {
    tb = (Vsoll - Vmin) / BmGes;
    te = SeGes / Vsoll + tb;
    tv = te - tb;
  }
  BigM_move_params(Xachse);
  BigM_move_params(Yachse);
  BigM_move_params(Zachse);
  BigM_move_params(Eachse);
}
void BigM_move_params(struct StepMotorBig &StepM) {
  StepM.soll_step = StepM.soll_posi * float(StepM.steps_pmm);
  StepM.step_div = abs(StepM.soll_step - StepM.act_step);
  StepM.time_pstep = ges_time / StepM.step_div;
  StepM.time_last_step = 0;
  digitalWrite(StepM.pinENA, LOW);
  if (StepM.soll_step > StepM.act_step)
    digitalWrite(StepM.pinDIR, HIGH);
  else
    digitalWrite(StepM.pinDIR, LOW);
    
  StepM.time_last_step = micros();
  if (BmGes != 0) {
    StepM.Se = float(StepM.step_div) / float(StepM.steps_pmm);
    StepM.f = StepM.Se / SeGes;
    StepM.Vmin = StepM.f * Vmin;
    StepM.Vsoll = StepM.f * Vsoll;
    StepM.Bm = StepM.f * BmGes;
    StepM.Sb = StepM.Bm * pow(tb, 2) / 2 + StepM.Vmin * tb;
    StepM.Sv = abs(StepM.Se - StepM.Sb);
  }
}
unsigned long timeToStep(struct StepMotorBig &StepM) {
  unsigned long t_p_step = StepM.time_pstep;
  if (BmGes != 0)
  {
    double S = (float)(abs(StepM.step_div) - abs(StepM.soll_step - StepM.act_step)) / (float)StepM.steps_pmm;
    double V;
    if (S <= (StepM.Se / 2)) {
      V = StepM.Vmin + ((StepM.Vsoll - StepM.Vmin) / StepM.Sb) * S;
    } else {
      V = StepM.Vmin + (StepM.Vsoll - StepM.Vmin) - ((StepM.Vsoll - StepM.Vmin) / StepM.Sb) * (S - StepM.Sv);
    }
    if (V > StepM.Vsoll) {
      V = StepM.Vsoll;
    }
    if (V < StepM.Vmin) {
      V = StepM.Vmin;
    }
    t_p_step = 1000000.0 / (StepM.steps_pmm * V);
  }
  return t_p_step;
}
int treiberBig(struct StepMotorBig &StepM) {
  //führt einen schrit aus und gibt den aktuellen an
  if (time_now - StepM.time_last_step > StepM.time_pstep) {
    if (StepM.soll_step > StepM.act_step) {
      //one stapp forward
      digitalWrite(StepM.pinPUL, LOW);
      StepM.act_step++;
      StepM.time_pstep = timeToStep(StepM);
      StepM.time_last_step = time_now;
      doneStep = true;
      digitalWrite(StepM.pinPUL, HIGH);
    } else if (StepM.soll_step < StepM.act_step) {
      //one stepp back
      digitalWrite(StepM.pinPUL, LOW);
      StepM.act_step--;
      StepM.time_pstep = timeToStep(StepM);
      StepM.time_last_step = time_now;
      doneStep = true;
      digitalWrite(StepM.pinPUL, HIGH);
    } else if (time_now - StepM.time_last_step > 5000000) {
      //turn motor off
      digitalWrite(StepM.pinENA, HIGH);
    }
  }
  if (StepM.soll_step == StepM.act_step && (time_now - StepM.time_last_step > StepM.time_pstep)) {
    return 0;
  } else {
    return 1;
  }
}
//SD Card Functions
void start_gcode(File &myFile, char fileName[]) {
  if (open_file(myFile, fileName) == 0) {
    GState = GCodeRun;
    prepSteps = 0;
    doneStep = false;
  } else {
    Serial.println("start_gcode: File not exist");
  }
}
int open_file(File &myFile, char fileName[]) {
  if (!myFile)
    myFile.close();
  if (!SD.exists(fileName))
    return -1;
  myFile = SD.open(fileName);
  if (myFile)
    return 0;
  else
    return -1;
}
int create_file(File &myFile, char fileName[]) {
  if (!myFile)
    myFile.close();
  if (SD.exists(fileName))
    SD.remove(fileName);
  myFile = SD.open(fileName, FILE_WRITE);
  GState = GCodeCreate;
  if (myFile) {
    Serial.println("GC_NL");
  } else {
    Serial.println("create_file: errorFileOpen");
  }
}
void writeToSD(File &myFile, char* command_line) {
  char wort[10];
  char* command_end = strchr(command_line, ' ');
  //Serial.println("write to SD Card");
  if (command_end != 0) {
    int command_length = command_end - command_line;
    strncpy(wort, command_line, command_length);
    wort[command_length] = '\0';
    if (strcmp("Q102", wort) == 0)
    {
      Serial.println("Finish Create File");
      close_file(myFile);
      GState = GCodeStop;
      return;
    }
  }
  //Serial.println("writeToSD: befor write");
  myFile.print(command_line);
  Serial.println("GC_NL");
}
int close_file(File &myFile) {
  myFile.close();
  return 0;
}
int remove_file(File &myFile, char fileName[]) {
  if (!myFile)
    myFile.close();
  if (SD.exists(fileName)) {
    SD.remove(fileName);
    Serial.print("remove_file: delete ");
    Serial.println(fileName);
    return 0;
  }
  Serial.println("remove_file: file not exist");
  return -1;
}
void printDirectory(File &myFile, File dir, int numTabs) {
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
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(myFile, entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
char* SDreadLine(File &myFile) {
  static char newLine[128];
  newLine[0] = '@';
  if (myFile.available())
  {
    int write_idx = 0;
    char read_buf;
    while (myFile.available())
    {
      read_buf = myFile.read();
      if (read_buf == '\n') //detekt end of line
      {
        newLine[write_idx] = '\0';//end of
        return newLine;
      } else if (read_buf == ';') { //remove comments out of the G-Code
        while (read_buf != '\n')
        {
          read_buf = myFile.read();
        }
        newLine[write_idx] = '\0';
        return newLine;
      }
      else
      { //read chars from file and safe it
        newLine[write_idx] = read_buf;
        write_idx++;
      }
    }
  }

  return newLine;
}
//Lineparsing Functions
void checkCommandFromSerial() {
  //read a G-Code line from the Serial and execute it
  char caLine[1];
  if (Serial.available())
  {
    Serial.readBytes(caLine, 1);
    reciveBuf[reciveWindex] = caLine[0];
    reciveWindex++;
    if (caLine[0] == '\n')
    {
      caLine[0] = ' ';
      reciveWindex = 0;
      if (GState == GCodeCreate) {
        writeToSD(GFile, reciveBuf);
      } else {
        //Serial.println(reciveBuf);
        eComandName newComand = ComandParser(reciveBuf);
        //Serial.println(reciveBuf);
        LineParser(reciveBuf, &newValue);
        //Serial.println(reciveBuf);
        executeNextGCodeLine(reciveBuf, &newValue, newComand);
        //Serial.println(reciveBuf);
      }
      memset(&reciveBuf[0], 0, sizeof(reciveBuf));
    }
  }
}
eComandName ComandParser(char* command_line) {
  //look for a G-Code Comand in Line
  char wort[10];
  char* command_end = strchr(command_line, ' ');
  if (command_end == 0)
    return FAIL;
  int command_length = command_end - command_line;
  strncpy(wort, command_line, command_length);
  wort[command_length] = '\0';
  //Serial.print("wort ");
  //Serial.println(wort);
  for (int i = 0; i < sizeof(lComandList) / sizeof(lComandList[0]); i++)
  {
    if (strcmp(lComandList[i].cName, wort) == 0)
    {
      //Serial.print("ComandParser: find ");
      //Serial.println(lComandList[i].cName);
      return lComandList[i].eIndent;
    }
  }
  //Serial.println("ComandParser: find no comand");
  return FAIL;
}
void LineParser(char* command_line, values *newValue) {
  //cheack for values in the G-Code Line
  char trennzeichen[] = " ";
  char *wort;
  char worker[128];

  strcpy(worker, command_line);
  newValue->useX = false;
  newValue->useY = false;
  newValue->useZ = false;
  newValue->useE = false;
  newValue->useF = false;
  newValue->useS = false;

  wort = strtok(worker, trennzeichen);
  while (wort != NULL) {
    switch (wort[0]) {
      case 'X':
        wort++;
        newValue->X = atof(wort);
        newValue->useX = true;
        break;
      case 'Y':
        wort++;
        newValue->Y = atof(wort);
        newValue->useY = true;
        break;
      case 'Z':
        wort++;
        newValue->Z = atof(wort);
        newValue->useZ = true;
        break;
      case 'E':
        wort++;
        newValue->E = atof(wort);
        newValue->useE = true;
        break;
      case 'S':
        wort++;
        newValue->S = atof(wort);
        newValue->useS = true;
        break;
      case 'F':
        wort++;
        newValue->F = atof(wort) / 60;
        newValue->useF = true;
        break;
    }
    wort = strtok(NULL, trennzeichen);
  }
}
char* getName(char* command_line) {
  char trennzeichen[] = " ";
  char *wort;
  wort = strtok(command_line, trennzeichen);
  //Serial.println(wort);
  wort = strtok(NULL, trennzeichen);
  //Serial.println(wort);
  if (wort != NULL) {
    //Serial.print("getName ");
    //Serial.println(wort);
    return wort;
  }
  return NULL;
}
void applayValues(values *newValue, double *X, double *Y, double *Z, double *E, double *S, double *F) {
  if (newValue->useX)
    *X = newValue->X;
  if (newValue->useY)
    *Y = newValue->Y;
  if (newValue->useZ)
    *Z = newValue->Z;
  if (newValue->useE)
    *E = newValue->E;
  if (newValue->useF)
    *F = newValue->F;
  if (newValue->useS)
    *S = newValue->S;
}
int executeNextGCodeLine(char* GLine, values *newValue, eComandName newComand) {
  double x = 0, y = 0, z = 0, e = 0, s = 0, f = 0;
  double fila = -999;
  //Serial.print("executeNextGCodeLine ");
  //Serial.println(GLine);
  switch (newComand)
  {
    case XXX:
      act_steps_to_act_posi();
      useEndSwitch = !useEndSwitch;
      Serial.print("posX");
      Serial.print(Xachse.act_posi);
      Serial.print(" Y");
      Serial.print(Yachse.act_posi);
      Serial.print(" Z");
      Serial.println(Zachse.act_posi);
      Serial.print(" ES");
      Serial.println(useEndSwitch);
      return -1;
      break;
    case G1://G1 move
      //Serial.println("executeNextGCodeLine: G1 found");
      applayValues(newValue, &Xachse.soll_posi, &Yachse.soll_posi,
                   &Zachse.soll_posi, &Eachse.soll_posi, &s, &Vsoll);
      getMoveParams();
      send_once = false;
      break;
    case G9://G9 stop
      //Serial.println("executeNextGCodeLine: G9 found");
      act_equal_soll();
      send_once = false;
      break;
    case G28://G28 move home
      Serial.println("executeNextGCodeLine: G28 found");
      moveHome = true;
      useEndSwitch = true;
      start_gcode(HomeFile, "HOME.TXT");
      break;
    case G92://G92 set positioning
      //Serial.println("executeNextGCodeLine: G92 found");
      applayValues(newValue, &Xachse.act_posi, &Yachse.act_posi,
                   &Zachse.act_posi, &Eachse.act_posi, &s, &f);
      setPose(newValue);
      send_once = false;
      break;
    case M109://M109 wait for hotend reach temperatur
      waitForHeat = true;
      applayValues(newValue, &x, &y, &z, &e, &soll_T, &f);
      Serial.print("M109 S");
      Serial.println(soll_T);
      break;
    case M104://M109 wait for hotend reach temperatur
      //Serial.println("executeNextGCodeLine: M104 found");
      applayValues(newValue, &x, &y, &z, &e, &soll_T, &f);
      Serial.print("M104 S");
      Serial.println(soll_T);
      break;
    case M140://M140 set hotend temperatur
    case M190:
      //Serial.println("executeNextGCodeLine: M140 found");
      applayValues(newValue, &x, &y, &z, &e, &soll_T_Bed, &f);
      Serial.print("M140 S");
      Serial.println(soll_T_Bed);
      break;
    case Q10://Q10 set move params
      //Serial.println("executeNextGCodeLine: Q10 found");
      applayValues(newValue, &BmMax, &Vmin, &Vmax, &threshAngle, &fila, &f);
      if (fila != -999) {
        Eachse.steps_pmm = fila;
      }
      else {
        fila = Eachse.steps_pmm;
      }
      Serial.print("Q10 X");
      Serial.print(BmMax);
      Serial.print(" Y");
      Serial.print(Vmin);
      Serial.print(" Z");
      Serial.print(Vmax);
      Serial.print(" E");
      Serial.print(threshAngle);
      Serial.print(" S");
      Serial.println(fila);
      break;
    case Q20://Q20 set NTC values
      Serial.println("executeNextGCodeLine: Q20 found");
      applayValues(newValue, &bitwertNTC, &widerstandNTC, &widerstand1, &e, &s, &f);
      break;
    case Q21://Q21 set PID values
      Serial.println("executeNextGCodeLine: Q21 found");
      applayValues(newValue, &KP, &KI, &KD, &e, &s, &f);
      if (e < 0.5) {
        myPID.SetTunings(KP, KI, KD, P_ON_E);
      }
      else {
        myPID.SetTunings(KP, KI, KD, P_ON_M);
      }
      break;
    case Q30://Q30 set NTC values for bed
      Serial.println("executeNextGCodeLine: Q30 found");
      applayValues(newValue, &bitwertNTC_Bed, &widerstandNTC_Bed, &widerstand1_Bed, &e, &s, &f);
      break;
    case Q100://Execute G-Code if File exist
      Serial.println("executeNextGCodeLine: Q100 found");
      useEndSwitch = true;
      start_gcode(GFile, getName(GLine));
      break;
    case Q101://open file to write G-Code in it
      Serial.println("executeNextGCodeLine: Q101 found");
      create_file(GFile, getName(GLine));
      break;
    case Q102://close file after write G-Code
      Serial.println("executeNextGCodeLine: Q102 found");
      close_file(GFile);
      GState = GCodeStop;
      break;
    case Q103://Pause G-Code
      Serial.println("executeNextGCodeLine: Q103found");
      if (GState == GCodeRun) {
        GState = GCodePause;
      }
      break;
    case Q104://Continue G-Code
      Serial.println("executeNextGCodeLine: Q104found");
      if (GState == GCodePause) {
        GState = GCodeRun;
      }
      break;
    case Q105://Abord G-code
      Serial.println("executeNextGCodeLine: Q105 found");
      GState = GCodeStop;
      close_file(GFile);
      act_equal_soll();
      send_once = false;
      break;
    case Q107://Delete File
      Serial.println("executeNextGCodeLine: Q107 found");
      remove_file(GFile, getName(GLine));
      break;
    case Q106://get list of File on SD Card
      Serial.println("executeNextGCodeLine: Q106 found");
      File root = SD.open("/");
      printDirectory(GFile, root, 0);
      break;
    //warum auch immer muss Q107 for Q106 sitzen sons wird Q107 nicht gefunden
    case Q107://Delete File
      Serial.println("executeNextGCodeLine: Q107 found");
      remove_file(GFile, getName(GLine));
      break;
    case FAIL:
      Serial.println("executeNextGCodeLine: no command found FAIL");
      return -1;
      break;
    default:
      Serial.println("executeNextGCodeLine: no command found");
      return -1;
  }
  return 0;
}

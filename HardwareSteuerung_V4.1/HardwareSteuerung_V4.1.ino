#include <SPI.h>
#include <SD.h>
#include <string.h>
#include <math.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include <PID_v1.h>

#define CPU 16000000.00
#define T_MAX 65535.00

//i dont know why to init this funktion here
void startTimer(int precscalar);

enum eGCodeState {
  GCodeStart,     //0
  GCodeRun,       //1
  GCodePause,     //2
  GCodeStop,      //3
  GCodeCreate,    //4
  GCodeStartHome, //5
  GCodeRunHome,   //6
  GCodeStopHome   //7
};
enum eAchse {X, Y, Z, E, C}; //alles achsen und C für spezial comand
enum eComandName {
  FAIL,
  XXX,
  G1,//move
  G9,//stop
  G28,//move home
  G92,//set position to
  M104,//set hotend temperature
  M114,//get position
  M140,//set bed temperatur
  M109,//set hotend temperature
  M190,//set bed temperatur
  Q10,//max Accelaration//acc- max- min- speed
  Q11,//toggle endswitch use
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
typedef struct StepMotorBig {
  //Settings
  eAchse achse;
  int pinENA;
  int pinDIR;
  int pinPUL;
  int pinNull;
  int pinPlus;
  //processing
  int ESstate = 0;
  int steps_pmm;//steps pro mm (constant für x y z)
  long act_step = 0;//actueller position der achse in step
} StepMotorBig;
struct sComand {
  eComandName eIndent;
  char cName[6];
};
typedef struct comParam {//decoded comand line
  eComandName com;
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
  unsigned int check;
  char *txt;
} comParam;
typedef struct  stepParam {
  eAchse achse;
  bool dir;
  unsigned int ticks;
  int preScale;
  comParam *com = nullptr;
} stepParam;
typedef struct circular_buffer {
  void *buffer;     // data buffer
  void *buffer_end; // end of data buffer
  size_t capacity;  // maximum number of items in the buffer
  size_t count;     // number of items in the buffer
  size_t sz;        // size of each item in the buffer
  void *head;       // pointer to head
  void *tail;       // pointer to tail
} circular_buffer;
typedef struct MovePos {
  float Xp = 0;
  long Xs = 0;
  float Xv = 0;
  float Xb = 0;
  bool useX = false;
  float Yp = 0;
  long Ys = 0;
  float Yv = 0;
  float Yb = 0;
  bool useY = false;
  float Zp = 0;
  long Zs = 0;
  float Zv = 0;
  float Zb = 0;
  bool useZ = false;
  float Ep = 0;
  long Es = 0;
  float Ev = 0;
  float Eb = 0;
  bool useE = false;
  float Speed = 20;//mm/s
} MovePos;
StepMotorBig Xachse;
StepMotorBig Yachse;
StepMotorBig Zachse;
StepMotorBig Eachse;
sComand lComandList[24] = {
  {XXX, "XXX"},
  {G1, "G1"},
  {G9, "G9"},
  {G28, "G28"},
  {G92, "G92"},
  {M104, "M104"},
  {M114, "M114"},
  {M140, "M140"},
  {M109, "M109"},
  {M190, "M190"},
  {Q10, "Q10"},
  {Q11, "Q11"},
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
  {Q107, "Q107"}
};

circular_buffer cbSteps;
circular_buffer cbRawCommands;
circular_buffer cbModifiedCommands;

eGCodeState state;//current state of the machine

File GFile;
File HomeFile;

comParam nextMove;

MovePos prePos;
MovePos nextPrePos;

char reciveBuf[128];
unsigned int reciveWindex = 0;
char GCodeFileName[32];

//temp controle
unsigned long time_now = micros();
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

//settings/Parameter
bool useES = true;

double BmMax = 150;
double Vmin = 10;
double Vmax = 20;
bool waitForHeat = false;

unsigned long TlastX;
unsigned long TlastY;


void setup() {
  soll_T_Bed = 0;
  soll_T = 0;

  // put your setup code here, to run once:
  Xachse.achse = X;
  Xachse.act_step = 0;
  Xachse.steps_pmm = 50;
  Xachse.pinENA = 37;//PC0
  Xachse.pinDIR = 39;//PG2
  Xachse.pinPUL = 41;//PG0
  Xachse.pinPlus = 44;
  Xachse.pinNull = 46;
  pinMode(Xachse.pinENA, OUTPUT);
  pinMode(Xachse.pinDIR, OUTPUT);
  pinMode(Xachse.pinPUL, OUTPUT);
  pinMode(Xachse.pinNull, INPUT_PULLUP);
  pinMode(Xachse.pinPlus, INPUT_PULLUP);

  Yachse.achse = Y;
  Yachse.act_step = 0;
  Yachse.steps_pmm = 50;
  Yachse.pinENA = 43;
  Yachse.pinDIR = 45;
  Yachse.pinPUL = 47;
  Yachse.pinPlus = 40;
  Yachse.pinNull = 42;
  pinMode(Yachse.pinENA, OUTPUT);
  pinMode(Yachse.pinDIR, OUTPUT);
  pinMode(Yachse.pinPUL, OUTPUT);
  pinMode(Yachse.pinNull, INPUT_PULLUP);
  pinMode(Yachse.pinPlus, INPUT_PULLUP);

  Zachse.achse = Z;
  Zachse.act_step = 0;
  Zachse.steps_pmm = 50;
  Zachse.pinENA = 25;
  Zachse.pinDIR = 27;
  Zachse.pinPUL = 29;
  Zachse.pinPlus = 36;
  Zachse.pinNull = 38;
  pinMode(Zachse.pinENA, OUTPUT);
  pinMode(Zachse.pinDIR, OUTPUT);
  pinMode(Zachse.pinPUL, OUTPUT);
  pinMode(Zachse.pinNull, INPUT_PULLUP);
  pinMode(Zachse.pinPlus, INPUT_PULLUP);

  Eachse.achse = X;
  Eachse.act_step = 0;
  Eachse.steps_pmm = 35;
  Eachse.pinENA = 31;
  Eachse.pinDIR = 33;
  Eachse.pinPUL = 35;
  Eachse.pinPlus = 0;
  Eachse.pinNull = 0;
  pinMode(Eachse.pinENA, OUTPUT);
  pinMode(Eachse.pinDIR, OUTPUT);
  pinMode(Eachse.pinPUL, OUTPUT);

  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(28, OUTPUT);

  pinMode(temprelai, OUTPUT);
  pinMode(temprelai_Bed, OUTPUT);
  digitalWrite(temprelai, LOW);
  digitalWrite(temprelai_Bed, HIGH);//turn off heating
  myPID.SetOutputLimits(0, 255);
  myPID.SetSampleTime(WindowSize);
  myPID.SetMode(AUTOMATIC);

  digitalWrite(22, LOW);
  digitalWrite(24, LOW);
  digitalWrite(26, LOW);
  digitalWrite(28, LOW);

  Serial.begin(115200);

  for(int i =0;i<5;i++){
    if(SD.begin(53))break;
    Serial.println("ERROR: SD initialization failed!");
    delay(1000);
  }

  Serial.println("SD initializ");

  //init the timer5
  TCNT5 = 0;
  TCCR5A = 0x00;
  TCCR5B = 0x00;
  TCCR5C = 0x00;
  setBit(&TIMSK5, (1 << TOIE5));//enable timer overflow

  cb_init(&cbSteps, 100, sizeof(stepParam));
  cb_init(&cbRawCommands, 3, sizeof(comParam));
  cb_init(&cbModifiedCommands, 5, sizeof(comParam));
  
  setState(GCodeStop);
  Serial.println("RESTART Arduino compleated");
}

void loop() {
  //startTimer(1);//start intterrupt timer
  while (true) {
    time_now = micros();
    doStdTasks();
    if (state == GCodeStart) {
      if (SD_OpenFile(&GFile , GCodeFileName) == 0) {
        setMotorsENA(false);//start motors
        setState(GCodeRun);
      } else {
        setState(GCodeStop);
        setWaitForHeat(false);
      }
    } else if (state == GCodeRun) {
      setUseES(false);//disable endswitches to move undisturbed in GCode
      if (calcPreRunPointer(&GFile) == -1 && cbSteps.count == 0)
        setState(GCodeStop);
    } else if (state == GCodePause) {
      setUseES(true);//enable endswitches for manuel movements
      //calculateSteps();
    } else if (state == GCodeStop) {
      setUseES(true);//enable endswitches for manuel movements
      SD_CloseFile(&GFile);
      if (getTravelDist(&prePos, &nextPrePos) == 0) {
        setMotorsENA(true);
      }
      //calculateSteps();
    } else if (state == GCodeCreate) {
      while (getState() == GCodeCreate) {
        if (SR_CheckForLine() == 1) {
          SD_writeToSD(&GFile, reciveBuf);
        }
      }
    } else if (state == GCodeStartHome) {
      if (SD_OpenFile(&HomeFile , "home.txt") == 0) {
        setState(GCodeRunHome);
        setUseES(true);
        Xachse.ESstate = 0;//trigger send of all endswitches
        Yachse.ESstate = 0;
        Zachse.ESstate = 0;
      }
    } else if (state == GCodeRunHome) {
      if (calcPreRunPointer(&HomeFile) == -1 && cbSteps.count == 0) {
        setState(GCodeStopHome);
      }
    } else if (state == GCodeStopHome) {
      SD_CloseFile(&HomeFile);
      sendDeviceStatus();
      setState(GCodeRun);
    } else {
      Serial.println("ERROR: unknown State");
    }
    calculateSteps();
  }
}

void setMotorsENA(bool b) {
  digitalWrite(Xachse.pinENA, b);
  digitalWrite(Yachse.pinENA, b);
  digitalWrite(Zachse.pinENA, b);
  digitalWrite(Eachse.pinENA, b);
}
void calculateSteps() {
  //get travel dist
  /*
  double gesDif = getTravelDist(&prePos, &nextPrePos);
  if (gesDif < 0.01)
    return;
  //check for Vmax Vmin

  //get travel achseSpeed  Xv/Xdif = Speed/GesDif
  calcSpeedForAches(gesDif,&prePos, &nextPrePos);
  //calculate step time x y z e sort in buffer
  */
  unsigned long nextStepTime = 0;
  unsigned long noStep = -1;
  unsigned long lastMoveTime = 0;
  unsigned long nextX,nextY,nextZ,nextE;
  if (nextPrePos.Xs != prePos.Xs){
    nextX = 1000000 / (nextPrePos.Xv * Xachse.steps_pmm);
    nextPrePos.useX = true;
  }else{
    nextX = 4294967295;
    nextPrePos.useX = false;
  }
  if (nextPrePos.Ys != prePos.Ys){  
    nextY = 1000000 / (nextPrePos.Yv * Yachse.steps_pmm);
    nextPrePos.useY = true;
  }else{
    nextY = 4294967295;
    nextPrePos.useY = false;
  }
  if (nextPrePos.Zs != prePos.Zs){
    nextZ = 1000000 / (nextPrePos.Zv * Zachse.steps_pmm);
    nextPrePos.useZ = true;
  }else{
    nextZ = 4294967295;
    nextPrePos.useZ = false;
  }
  if (nextPrePos.Es != prePos.Es){
    nextE = 1000000 / (nextPrePos.Ev * Eachse.steps_pmm);
    nextPrePos.useE = true;
  }else{
    nextE = 4294967295;
    nextPrePos.useE = false;
  }

  bool XmakeStep = false;
  bool YmakeStep = false;
  bool ZmakeStep = false;
  bool EmakeStep = false;

  eAchse nextAchse;
  
  while (!prePointerOnPos()) {
    //if a axes get set to stop ore arrive its position it is cancelt from steps calculatioons
    doStdTasks();
    nextAchse = getSmalest(nextX, nextY, nextZ, nextE);
    if(nextAchse == X){
        if (nextPrePos.useX == false){
          nextX = 4294967295;
        }else{
          lastMoveTime = nextX;
          nextX += 1000000 / (abs(nextPrePos.Xv) * Xachse.steps_pmm);
          nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
          createStep(X, &nextPrePos.Xs, &prePos.Xs, nextStepTime - lastMoveTime);
          XmakeStep = true;
        }
      }else if(nextAchse == Y){
        if (nextPrePos.useY == false){
          nextY = 4294967295;
        }else{
          lastMoveTime = nextY;
          nextY += 1000000 / (abs(nextPrePos.Yv) * Yachse.steps_pmm);
          nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
          createStep(Y, &nextPrePos.Ys, &prePos.Ys, nextStepTime - lastMoveTime);
          YmakeStep = true;
        }
      }else if(nextAchse == Z){
        if (nextPrePos.useZ == false){
          nextZ = 4294967295;
        }else{
          lastMoveTime = nextZ;
          nextZ += 1000000 / (abs(nextPrePos.Zv) * Zachse.steps_pmm);
          nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
          createStep(Z, &nextPrePos.Zs, &prePos.Zs, nextStepTime - lastMoveTime);
          ZmakeStep = true;
        }
      }else if(nextAchse == E){
        if (nextPrePos.useE == false){
          nextE = 4294967295;
        }else{
          lastMoveTime = nextE;
          nextE += 1000000 / (abs(nextPrePos.Ev) * Eachse.steps_pmm);
          nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
          createStep(E, &nextPrePos.Es, &prePos.Es, nextStepTime - lastMoveTime);
          EmakeStep = true;
        }
      }else{
        Serial.println("ERROR: get falls step to plan calculateSteps");
        Serial.print(" X ");
        Serial.print(nextPrePos.Xs);
        Serial.print(" -> ");
        Serial.print(prePos.Xs);
        Serial.print(" Y ");
        Serial.print(nextPrePos.Ys);
        Serial.print(" -> ");
        Serial.print(prePos.Ys);
        Serial.print(" Z ");
        Serial.print(nextPrePos.Zs);
        Serial.print(" -> ");
        Serial.print(prePos.Zs);
        Serial.print(" E ");
        Serial.print(nextPrePos.Es);
        Serial.print(" -> ");
        Serial.print(prePos.Es);
      }
  }
  if(XmakeStep)prePos.Xp = float(prePos.Xs) / float(Xachse.steps_pmm);
  if(YmakeStep)prePos.Yp = float(prePos.Ys) / float(Yachse.steps_pmm);
  if(ZmakeStep)prePos.Zp = float(prePos.Zs) / float(Zachse.steps_pmm);
  if(EmakeStep)prePos.Ep = float(prePos.Es) / float(Eachse.steps_pmm);
}
void calcSpeedForAches(double gesDif,MovePos *startPos,MovePos *goalPos){
  if (Vmax < goalPos->Speed)goalPos->Speed = Vmax;
  if (Vmin > goalPos->Speed)goalPos->Speed = Vmin;

  if(startPos->Xs == goalPos->Xs)
    goalPos->Xv = 0;
  else
    goalPos->Xv = ((goalPos->Xp - startPos->Xp) / gesDif) * goalPos->Speed;

  if(startPos->Ys == goalPos->Ys)
    goalPos->Yv = 0;
  else 
    goalPos->Yv = ((goalPos->Yp - startPos->Yp) / gesDif) * goalPos->Speed;

  if(startPos->Zs == goalPos->Zs)
    goalPos->Zv = 0;
  else
    goalPos->Zv = ((goalPos->Zp - startPos->Zp) / gesDif) * goalPos->Speed;

  if(startPos->Es == goalPos->Es)
    goalPos->Ev = 0;
  else
    goalPos->Ev = ((goalPos->Ep - startPos->Ep) / gesDif) * goalPos->Speed;
  /*
  if(goalPos->Xv>Vmax){
    goalPos->Xv = Vmax;
    Serial.println("ERROR: X to fast");
  }
  if(goalPos->Yv>Vmax){
    goalPos->Yv = Vmax;
    Serial.println("ERROR: Y to fast");
  }
  if(goalPos->Zv>Vmax){
    goalPos->Zv = Vmax;
    Serial.println("ERROR: Z to fast");
  }
  if(goalPos->Ev>Vmax){
    goalPos->Ev = Vmax;
    Serial.println("ERROR: E to fast");
  }*/
}
void calcAccForAches(double gesDif,MovePos *startPos,MovePos *goalPos){
  if(startPos->Xs != goalPos->Xs)
    goalPos->Xb = (BmMax/gesDif)*abs(goalPos->Xp-startPos->Xp);

  if(startPos->Ys != goalPos->Ys)
    goalPos->Yb = (BmMax/gesDif)*abs(goalPos->Yp-startPos->Yp);

  if(startPos->Zs != goalPos->Zs)
    goalPos->Zb = (BmMax/gesDif)*abs(goalPos->Zp-startPos->Zp);

  if(startPos->Es != goalPos->Es)
    goalPos->Eb = (BmMax/gesDif)*abs(goalPos->Ep-startPos->Ep);
}
eAchse getSmalest(unsigned long x, unsigned long y, unsigned long z, unsigned long e) { 
  if (x < y && x < z && x < e)// && nextPrePos.useX)
    return X;
  else if (y < z && y < e)// && nextPrePos.useY)
    return Y;
  else if (z < e)// && nextPrePos.useZ)
    return Z;
  else// if (nextPrePos.useE)
    return E;
}
unsigned long getSmalestValue(unsigned long x, unsigned long y, unsigned long z, unsigned long e) {
  unsigned long smalest;
  switch(getSmalest(x,y,z,e)){
    case X:
      smalest = x;
      break;
    case Y:
      smalest =  y;
      break;
    case Z:
      smalest =  z;
      break;
    case E:
      smalest =  e;
      break;
    default:
      Serial.println("ERROR: getSmalestValue no match");
      return -1; 
  }  
  return smalest;
}
bool prePointerOnPos() {
  if (nextPrePos.Xs == prePos.Xs)
    nextPrePos.useX = false;
  //  else
  //    nextPrePos.useX = true;

  if (nextPrePos.Ys == prePos.Ys)
    nextPrePos.useY = false;
  //  else
  //    nextPrePos.useY = true;

  if (nextPrePos.Zs == prePos.Zs)
    nextPrePos.useZ = false;
  //  else
  //    nextPrePos.useZ = true;

  if (nextPrePos.Es == prePos.Es)
    nextPrePos.useE = false;
  //  else
  //    nextPrePos.useE = true;
 
  if(!nextPrePos.useX && !nextPrePos.useY && !nextPrePos.useZ && !nextPrePos.useE)
    return 1;

  return 0;
}
void createStep(eAchse achse, long *sollStep, long *istStep, double us) {
  stepParam newStep;
  newStep.achse = achse;
  if(us<0)Serial.println("ERROR: in createStep us < 0");
  usToTimer(&newStep, abs(us));
  if (*sollStep > *istStep) {
    newStep.dir = 1;
    cb_push_back(&cbSteps, &newStep);
    *istStep += 1;
  } else if (*sollStep < *istStep) {
    newStep.dir = 0;
    cb_push_back(&cbSteps, &newStep);
    *istStep -= 1;
  }else{
    Serial.println("ERROR: steps equal soll:");
  }
}
float getTravelDist(MovePos* pPos, MovePos* nPrePos) {
  float Xdif,Ydif,Zdif,Edif;
  if(pPos->Xs == nPrePos->Xs)
    Xdif = 0;
  else
    Xdif = pPos->Xp - nPrePos->Xp;

  if(pPos->Ys == nPrePos->Ys)
    Ydif = 0;
  else
    Ydif = pPos->Yp - nPrePos->Yp;
  
  if(pPos->Zs == nPrePos->Zs)
    Zdif = 0;
  else
    Zdif = pPos->Zp - nPrePos->Zp;

  float gesDif = sqrt(pow(Xdif, 2) + pow(Ydif, 2) + pow(Zdif, 2));
  if (gesDif < 0.02){
    if(pPos->Es == nPrePos->Es)
      Edif = 0;
    else
      Edif = (pPos->Ep - nPrePos->Ep)*2;//half the speed if only e is used
    return abs(Edif);
  }
  return gesDif;
}
void usToTimer(stepParam* myStep, double us) {
  //tick = 1/16.000.000 = 62.5ns
  // max_ticks = 65535
  //Serial.print("usToTime us:");
  //Serial.print(us);
  //Serial.print(" ");
  //double us = 1.0/StepPerSekond;
  //double usF = CPU;
  double usF = 16;
  if (us < 0) {
    myStep->ticks = T_MAX;
    myStep->preScale = 1;//4096us
    Serial.println("ERROR: us smaler 0");
  } else if (us < 4095) {
    myStep->ticks =  T_MAX - (usF / 1.0) * us;
    myStep->preScale = 1;//4096us
  } else if (us < 32767) {
    myStep->ticks = T_MAX - (usF / 8.0) * us;
    myStep->preScale = 8;//32767us
  } else if (us < 262139) {
    myStep->ticks = T_MAX - (usF / 64.0) * us;
    myStep->preScale = 64;//262139us
  } else if (us < 1048559) {
    myStep->ticks = T_MAX - (usF / 256.0) * us;
    myStep->preScale = 256;//1048559us
  } else { // if(us<4194239){
    myStep->ticks = T_MAX - (usF / 1024.0) * us;
    myStep->preScale = 1024;//4194239us
  }
  if (myStep->ticks > T_MAX) myStep->ticks = T_MAX;
  if (myStep->ticks < 0) myStep->ticks = 0;
  //if(myStep->ticks < 500)Serial.println("ERROR: ticks may to fast");
  //  Serial.print(" pre:");
  //  Serial.print(myStep->preScale);
  //  Serial.print(" ticks:");
  //  Serial.println(myStep->ticks);
}
int readNextCommand(File* gFile){
  char *newLine;
  comParam newCommand;
  while(cbRawCommands.count<2){
    newLine = SD_ReadLine(gFile);
    if(newLine[0] != '\0' && newLine[0] != '@'){
      //parse and push back
      Serial.print("readNextCommand:");
      Serial.println(newLine);
      newCommand = getCommandFromLine(newLine);
      cb_push_back(&cbRawCommands,&newCommand);
    }else if(newLine[0] == '@') {
      return -1;
    }
  }
  return 0;
}
comParam getCommandFromLine(char* newLine){
  comParam newCommand;
  newCommand.com = ComandParser(newLine);
  if (newCommand.com == Q100 ||
    newCommand.com == Q101 ||
    newCommand.com == Q107) {
    newCommand.txt = getName(newLine);
    Serial.print("name: ");
    Serial.println(newCommand.txt);
  } else if (newCommand.com == G9) {
    Serial.println("find G9 ");
  } else {
    LineParser(newLine, &newCommand);
    Serial.print("values: ");
    Serial.println(newLine);
  }
  return newCommand;
}
int calcPreRunPointer(File* gFile) {
  char *newLine;
  comParam nextCommand,peekCommand;
  if(waitForHeat == true)
    return 0; 

    if(cbModifiedCommands.count > 0){
      cb_pop_front(&cbModifiedCommands,&nextCommand);
      processComandLine(nextCommand,false);
      return 0;
    }

  //Serial.println("1 start");
  //fill first buffer with at least two points
  if(readNextCommand(gFile) == -1 && cbRawCommands.count == 0){
    //Serial.println("X stop");
    return -1;
  }
  //Serial.println("2 read command");

  if(cb_pop_front(&cbRawCommands,&nextCommand) == -1){
    Serial.println("ERROR: cant find objekt in rawCommand");
    return -1;
  }
  //Serial.println("3 get new command");
  //if next point in buffer is not G1
  //   push int in a second buffer
  if(nextCommand.com != G1 || BmMax == 0){//if BmMax == 0 do not use acc
    cb_push_back(&cbModifiedCommands,&nextCommand);
    //Serial.println("4 push new command");
  }else{
    if(cb_peek_front(&cbRawCommands, &peekCommand) == -1){
      Serial.println("ERROR: cant find objekt in peek");
      return -1;
    }
    //if point after next is not G1
    //  Calc a point in break distanst to the actual point and push it in buffer with wanted speed
    //  push actual point in Buffer with speed 0
    //if point after next is G1
    //  calc point in grind distent to the actuel point and push it in buffer with wanted speed
    if(peekCommand.com != G1){
      addStopPoint(&nextCommand);//to cbModifiedCommands
    }else{
      addStopPoint(&nextCommand);
      //addGrindPoint(&nextCommand,&peekCommand);//to cbModifiedCommands
    }
  }
    //Serial.println("5 execute");
    //next command is now used to execute all comands in cbModifiedCommands

  //need a raw command buffer with the actual commands from the G-Code
  //second buffer with the correctet and added grind points and commands  
  return 0;
}
void addStopPoint(comParam *nextCommand){
  comParam breakPoint;
  float xg = nextCommand->X - prePos.Xp;//einzel achsen strecke
  float yg = nextCommand->Y - prePos.Yp;
  float zg = nextCommand->Z - prePos.Zp;
  float eg = nextCommand->E - prePos.Ep;
  float sg = sqrt(pow(xg,2)+pow(yg,2)+pow(zg,2));//gesamt strecke
  if(sg<0.02) sg = abs(eg)*2;

  float tb = (nextCommand->F - Vmin)/BmMax;// zeit um ziehlgeschwindigkeit zu erreichen
  float sb = sg - BmMax*pow(tb,2)/2+Vmin*tb;//distanz von start punkt ab dem gebremst werden muss
  
  breakPoint.X = (sb/sg) * xg;
  breakPoint.Y = (sb/sg) * yg;
  breakPoint.Z = (sb/sg) * zg;
  breakPoint.E = (sb/sg) * eg;
  breakPoint.F = nextCommand->F;

  breakPoint.useX = nextCommand->useX;
  breakPoint.useY = nextCommand->useY;
  breakPoint.useZ = nextCommand->useZ;
  breakPoint.useE = nextCommand->useE;
  breakPoint.useF = nextCommand->useF;
  cb_push_back(&cbModifiedCommands,&breakPoint);

  nextCommand->F = 0;
  cb_push_back(&cbModifiedCommands,&nextCommand);
}
void calcNextPos(comParam *newCommand){
  setNextPrePos(newCommand);
  double gesDif = getTravelDist(&prePos, &nextPrePos);
  calcSpeedForAches(gesDif,&prePos, &nextPrePos);
  calcAccForAches(gesDif,&prePos, &nextPrePos);
}
void processComandLine(comParam newCommand,bool doNow) {

  Serial.print("Com ");
  Serial.print(newCommand.com);
  Serial.print(" X");
  Serial.print(newCommand.X);
  Serial.print(",");
  Serial.print(newCommand.useX);
  Serial.print(" Y");
  Serial.print(newCommand.Y);
  Serial.print(",");
  Serial.print(newCommand.useY);
  Serial.print(" Z");
  Serial.print(newCommand.Z);  
  Serial.print(",");
  Serial.print(newCommand.useZ);
  Serial.print(" E");
  Serial.println(newCommand.E);
  if (newCommand.com == Q100 ||
    newCommand.com == Q101 ||
    newCommand.com == Q107)
  {
    Serial.print(" txt");
    Serial.println(newCommand.txt);
  }
  //Serial.print("paresed ");
  if (newCommand.com == G1) {
    if (newCommand.useF) {
      if (Vmax < newCommand.F)newCommand.F = Vmax;
      if (Vmin > newCommand.F)newCommand.F = Vmin;
    }
    if (newCommand.useF && newCommand.F != nextPrePos.Speed) {
      Serial.print("G1 F");
      Serial.println(newCommand.F);
    }
    calcNextPos(&newCommand);
  } else if (newCommand.com == G9) { //stop movement
    Serial.println("G9");
    StopMove();
    sendDeviceStatus();
  } else if (newCommand.com == G28) { //start move home
    Serial.println("G28");
    addCommand(&newCommand,true);
  } else if (newCommand.com == G92) { //set pos
    //Serial.println("G92");
    setPrePos(&newCommand);
    setNextPrePos(&newCommand);
    addCommand(&newCommand,doNow);
    sendDeviceStatus();
  } else if (newCommand.com == M104 || //set temperature
             newCommand.com == M140) {
    Serial.println("M144");
    addCommand(&newCommand,doNow);
  } else if (newCommand.com == M114) {
    Serial.println("M114");
    sendDeviceStatus();
  } else if (newCommand.com == M109 || //set temperature
             newCommand.com == M190) {
    Serial.println("M199");
    addCommand(&newCommand,doNow);
  } else if (newCommand.com == Q10) { //max Accelaration//acc- max- min- speed
    Serial.println("Q10");
    double help;
    double fila = -1;
    applayValues(&newCommand, &BmMax, &Vmin, &Vmax, &help, &fila, &help);
    if (fila != -1)Eachse.steps_pmm = fila;
    
    //send endswitch status (may a get seperat command)
    Xachse.ESstate = 0;//trigger send of all endswitches
    Yachse.ESstate = 0;
    Zachse.ESstate = 0;
    setUseES(true);
    checkEndswitches();
    setUseES(false);

    Serial.print("Q10 X");
    Serial.print(BmMax);
    Serial.print(" Y");
    Serial.print(Vmin);
    Serial.print(" Z");
    Serial.print(Vmax);
    Serial.print(" E");
    Serial.print(-1);
    Serial.print(" S");
    Serial.println(Eachse.steps_pmm);
  } else if (newCommand.com == Q11) {
    setUseES(!useES);
  } else if (newCommand.com == Q20) { //max speed//bitwertNTC widerstandNTC widerstand1   for hotend
    Serial.println("Q20");
    bitwertNTC = newCommand.X;
    widerstandNTC = newCommand.Y;
    widerstand1 = newCommand.Z;
  } else if (newCommand.com == Q21) { //min speed//P I D ON for hotend
    Serial.println("Q21");
    KP = newCommand.X;
    KI = newCommand.Y;
    KD = newCommand.Z;
    if (newCommand.E < 0.5) {
      myPID.SetTunings(KP, KI, KD, P_ON_E);
    } else {
      myPID.SetTunings(KP, KI, KD, P_ON_M);
    }
  } else if (newCommand.com == Q30) { //min speed//bitwertNTC widerstandNTC widerstand1   for bed
    Serial.println("Q30");
    bitwertNTC_Bed = newCommand.X;
    widerstandNTC_Bed = newCommand.Y;
    widerstand1_Bed = newCommand.Z;
  } else if (newCommand.com == Q100) { //Execute G-Code if File exist
    Serial.println("Q100");
    strcpy(GCodeFileName, newCommand.txt);
    if (getState() == GCodeStop)
      setState(GCodeStart);
  } else if (newCommand.com == Q101) { //open file to write G-Code in it
    Serial.println("Q101");
    if (getState() == GCodeStop) {
      setState(GCodeCreate);
      SD_createFile(&GFile, newCommand.txt);
    } else
      Serial.println("ERROR: write can only start when stoped");
  } else if (newCommand.com == Q102) { //close file after write G-Code
    Serial.println("Q102");
  } else if (newCommand.com == Q103) { //Pause G-Code
    Serial.println("Q103");
  } else if (newCommand.com == Q104) { //Continue G-Code
    Serial.println("Q104");
  } else if (newCommand.com == Q105) { //Abord G-code
    Serial.println("Q105");
    if (getState() == GCodeRun) {
      setState(GCodeStop);
    } else if (getState() == GCodeRunHome) {
      setState(GCodeStopHome);
    }
    StopMove();
  } else if (newCommand.com == Q106) { //get list of File on SD Card
    Serial.println("Q106");
    File root = SD.open("/");
    SD_printDirectory(GFile, root, 0);
  } else if (newCommand.com == Q107) { //Delete File
    Serial.println("Q107");
    SD_removeFile(newCommand.txt);
  } else {
    Serial.print("ERROR: unknown GCode: ");
    Serial.println(newCommand.txt);
  }
  //sendDeviceStatus();
}
void setUseES(bool newES){
  if(newES != useES){
    useES = newES;
    Serial.print("useES ");
    Serial.println(useES);
  }
}
void sendDeviceStatus() {
  Serial.print("M114 X");
  Serial.print(Xachse.act_step / double(Xachse.steps_pmm));
  Serial.print(" Y");
  Serial.print(Yachse.act_step / double(Yachse.steps_pmm));
  Serial.print(" Z");
  Serial.print(Zachse.act_step / double(Zachse.steps_pmm));
  Serial.print(" E");
  Serial.println(Eachse.act_step / double(Eachse.steps_pmm));
  /*
  Serial.print("prePos X");
  Serial.print(prePos.Xp);
  Serial.print(" , ");
  Serial.print(prePos.Xs / double(Xachse.steps_pmm));
  Serial.print(" Y");
  Serial.print(prePos.Yp);
  Serial.print(" , ");
  Serial.print(prePos.Ys / double(Yachse.steps_pmm));
  Serial.print(" Z");
  Serial.print(prePos.Zp);
  Serial.print(" , ");
  Serial.print(prePos.Zs / double(Zachse.steps_pmm));
  Serial.print(" E");
  Serial.print(prePos.Ep);
  Serial.print(" , ");
  Serial.println(prePos.Es / double(Eachse.steps_pmm));
  
  Serial.print("nextPrePos X");
  Serial.print(nextPrePos.Xp);
  Serial.print(" , ");
  Serial.print(nextPrePos.Xs / double(Xachse.steps_pmm));
  Serial.print(" Y");
  Serial.print(nextPrePos.Yp);
  Serial.print(" , ");
  Serial.print(nextPrePos.Ys / double(Yachse.steps_pmm));
  Serial.print(" Z");
  Serial.print(nextPrePos.Zp);
  Serial.print(" , ");
  Serial.print(nextPrePos.Zs / double(Zachse.steps_pmm));
  Serial.print(" E");
  Serial.print(nextPrePos.Ep);
  Serial.print(" , ");
  Serial.println(nextPrePos.Es / double(Eachse.steps_pmm));
  */
}
void StopMove() {
  StopAchse(X);
  StopAchse(Y);
  StopAchse(Z);
  StopAchse(E);
  cb_clear(&cbSteps);
}
void StopAchse(eAchse achse) {
  //comParam newPos;
  switch (achse) {
    case X:
      nextPrePos.useX = false;
      nextPrePos.Xp = float(Xachse.act_step) / float(Xachse.steps_pmm);
      nextPrePos.Xs = Xachse.act_step;
      prePos.Xp = float(Xachse.act_step) / float(Xachse.steps_pmm);
      prePos.Xs = Xachse.act_step;
      break;
    case Y:
      nextPrePos.useY = false;
      nextPrePos.Yp = float(Yachse.act_step) / float(Yachse.steps_pmm);
      nextPrePos.Ys = Yachse.act_step;
      prePos.Yp = float(Yachse.act_step) / float(Yachse.steps_pmm);
      prePos.Ys = Yachse.act_step;
      break;
    case Z:
      nextPrePos.useZ = false;
      nextPrePos.Zp = float(Zachse.act_step) / float(Zachse.steps_pmm);
      nextPrePos.Zs = Zachse.act_step;
      prePos.Zp = float(Zachse.act_step) / float(Zachse.steps_pmm);
      prePos.Zs = Zachse.act_step;
      break;
    case E:
      nextPrePos.useE = false;
      nextPrePos.Ep = float(Eachse.act_step) / float(Eachse.steps_pmm);
      nextPrePos.Es = Eachse.act_step;
      prePos.Ep = float(Eachse.act_step) / float(Eachse.steps_pmm);
      prePos.Es = Eachse.act_step;
      break;
  }

}
void setNextPrePos(comParam* newPos) {
  if (newPos->useX) {
    nextPrePos.Xp = newPos->X;
    nextPrePos.Xs = nextPrePos.Xp * float(Xachse.steps_pmm);
  }
  if (newPos->useY) {
    nextPrePos.Yp = newPos->Y;
    nextPrePos.Ys = nextPrePos.Yp * float(Yachse.steps_pmm);
  }
  if (newPos->useZ) {
    nextPrePos.Zp = newPos->Z;
    nextPrePos.Zs = nextPrePos.Zp * float(Zachse.steps_pmm);
  }
  if (newPos->useE) {
    nextPrePos.Ep = newPos->E;
    nextPrePos.Es = nextPrePos.Ep * float(Eachse.steps_pmm);
  }
  if (newPos->useF)
    nextPrePos.Speed = newPos->F;

  double gesDif = getTravelDist(&prePos, &nextPrePos);
  calcSpeedForAches(gesDif,&prePos, &nextPrePos);
  calcAccForAches(gesDif,&prePos, &nextPrePos);
}
void setPrePos(comParam* newPos) {
  if (newPos->useX){
    prePos.Xp = newPos->X;
    prePos.Xs = prePos.Xp * float(Xachse.steps_pmm);
  }
  if (newPos->useY) {
    prePos.Yp = newPos->Y;
    prePos.Ys = prePos.Yp * float(Yachse.steps_pmm);
  }
  if (newPos->useZ) {
    prePos.Zp = newPos->Z;
    prePos.Zs = prePos.Zp * float(Zachse.steps_pmm);
  }
  if (newPos->useE) {
    prePos.Ep = newPos->E;
    prePos.Es = prePos.Ep * float(Eachse.steps_pmm);
  }
  if (newPos->useF)
    prePos.Speed = newPos->F;
}
void setRealPose(comParam* newPos) {
  if (newPos->useX)
    Xachse.act_step = newPos->X * float(Xachse.steps_pmm);
  if (newPos->useY)
    Yachse.act_step = newPos->Y * float(Yachse.steps_pmm);
  if (newPos->useZ)
    Zachse.act_step = newPos->Z * float(Zachse.steps_pmm);
  if (newPos->useE)
    Eachse.act_step = newPos->E * float(Eachse.steps_pmm);
}
comParam getRealPos(){
  comParam newPos;
  newPos.useX = true;
  newPos.X = float(Xachse.act_step) / float(Xachse.steps_pmm);
  newPos.useY = true;
  newPos.Y = float(Yachse.act_step) / float(Yachse.steps_pmm);
  newPos.useZ = true;
  newPos.Z = float(Zachse.act_step) / float(Zachse.steps_pmm);
  newPos.useE = true;
  newPos.E = float(Eachse.act_step) / float(Eachse.steps_pmm);
  return newPos;
}
void setState(eGCodeState newState) {
  state = newState;
  Serial.print("newState ");
  Serial.println(state);
}
eGCodeState getState() {
  return state;
}
void checkEndswitches() {
  if (getState() == GCodeRunHome || useES) {
    //Serial.println("check es");
    handleES(&Xachse, "X");
    handleES(&Yachse, "Y");
    handleES(&Zachse, "Z");
  }
}
void handleES(StepMotorBig* mot, char* msg) {
  if (digitalRead(mot->pinNull) == HIGH && mot->ESstate != -1){
    mot->ESstate = -1;
    //StopAchse(mot->achse);
    Serial.print("ES ");
    Serial.print(msg);
    Serial.println(mot->ESstate);
  }else if(digitalRead(mot->pinPlus) == HIGH && mot->ESstate != 1){
    mot->ESstate = 1;
    //StopAchse(mot->achse);
    Serial.print("ES ");
    Serial.print(msg);
    Serial.println(mot->ESstate);
  }
  if (digitalRead(mot->pinNull) == LOW && digitalRead(mot->pinPlus) == LOW && mot->ESstate != 0){
    mot->ESstate = 0;
    Serial.print("ES ");
    Serial.print(msg);
    Serial.println(mot->ESstate);
  }
}
int checkSerial() {
  if (SR_CheckForLine() == 1) {
    setMotorsENA(false);
    processComandLine(getCommandFromLine(reciveBuf),true);
    memset(&reciveBuf[0], 0, sizeof(reciveBuf));
  }
}
int SR_CheckForLine() {
  char caLine[1];
  comParam newCommand;
  while (Serial.available()) {
    Serial.readBytes(caLine, 1);
    if (caLine[0] == '\n')
    {
      reciveBuf[reciveWindex] = '\0';
      caLine[0] = ' ';
      reciveWindex = 0;
      return 1;
    }
    reciveBuf[reciveWindex] = caLine[0];
    reciveWindex++;
  }
  return 0;
}
ISR (TIMER5_OVF_vect) { // Timer1 ISR
  cli();
  //digitalWrite(22,HIGH);
  //unsigned long Tnow = micros();
  stepParam nextStep;
  comParam nextCommand;
  if (cb_pop_front(&cbSteps, &nextStep) == -1) {
    if(!waitForHeat && getState()==GCodeRun)Serial.println("Buffer was empty");
    startTimer(0);    
  } else {
    if(nextStep.achse == X) {
        performStep(&Xachse, nextStep.dir,nextStep.achse);
    }else if(nextStep.achse == Y) {
        performStep(&Yachse, nextStep.dir,nextStep.achse);
    }else if(nextStep.achse == Z) {
        performStep(&Zachse, nextStep.dir,nextStep.achse);
    }else if(nextStep.achse == E) {
        performStep(&Eachse, nextStep.dir,nextStep.achse);
    }else if(nextStep.achse == C) {
        performCommand(nextStep.com);
        delete nextStep.com;
    }else{
      Serial.print("ERROR: unknown step achse:");
      Serial.print(nextStep.achse);
      Serial.print(" ticks:");
      Serial.print(nextStep.ticks);
      Serial.print(" preScale:");
      Serial.print(nextStep.preScale);
    }

    startTimer(nextStep.preScale);
    //do not use 
    //TCNT5 = nextStep.ticks;
    if ((nextStep.ticks + (16.0 / (double)nextStep.preScale) * 40) > T_MAX) {
      TCNT5 = T_MAX;
    } else {
      TCNT5 = nextStep.ticks + (16.0 / (double)nextStep.preScale) * 40;
    }
  }
  //digitalWrite(22,LOW);
  sei();
}
void addCommand(comParam* newCommand,bool doNow) {
  if(doNow){
    performCommand(newCommand);
  }else{
    stepParam newStep;
    comParam* newCom = malloc(sizeof(comParam));
    memcpy(newCom, newCommand, sizeof(comParam));
    newStep.com = newCom;
    newStep.achse = C;
    newStep.preScale = 1;
    cb_push_back(&cbSteps, &newStep);
  }
}
void performStep(StepMotorBig *mot, bool dir, eAchse achse) {
  if (dir && useES && mot->pinPlus != 0) {
    if (digitalRead(mot->pinPlus) == HIGH) {
      StopAchse(achse);
      return;
    }
  } else if (!dir && useES && mot->pinNull != 0) {
    if (digitalRead(mot->pinNull) == HIGH) {
      StopAchse(achse);
      return;
    }
  }
  //Zachse.pinPlus = 36;
  //Zachse.pinNull = 38;
  digitalWrite(mot->pinPUL, LOW);
  if (digitalRead(mot->pinDIR) != dir)
    digitalWrite(mot->pinDIR, dir);

  if (dir)
    mot->act_step++;
  else
    mot->act_step--;
  digitalWrite(mot->pinPUL, HIGH);
}
void performCommand(comParam* newCommand) {
  switch (newCommand->com) {
    case G92:
      setRealPose(newCommand);
      //sendDeviceStatus();
      break;
    case G28:
      setState(GCodeStartHome);
      break;
    case M104:
      soll_T = newCommand->S;
      Serial.print("M104 S");
      Serial.println(soll_T);
      sendTemperature();
      break;
    case M140:
      soll_T_Bed = newCommand->S;
      Serial.print("M140 S");
      Serial.println(soll_T_Bed);
      sendTemperature();
      break;
    case M109:
      soll_T = newCommand->S;
      Serial.print("M109 S");
      Serial.println(soll_T);
      setWaitForHeat(true);
      sendTemperature();
      break;
    case M190:
      soll_T_Bed = newCommand->S;
      Serial.print("M190 S");
      Serial.println(soll_T_Bed);
      sendTemperature();
      break;
    default:
      Serial.print("ERROR: unknown Command ");
      Serial.println(newCommand->com);
  }
}
void startTimer(int prescale) {
  switch (prescale) {
    case 0:
      resetBit(&TCCR5B, (1 << CS10));
      resetBit(&TCCR5B, (1 << CS11));
      resetBit(&TCCR5B, (1 << CS12));
      break;
    case 1:
      setBit(&TCCR5B, (1 << CS10));
      resetBit(&TCCR5B, (1 << CS11));
      resetBit(&TCCR5B, (1 << CS12));
      break;
    case 8:
      resetBit(&TCCR5B, (1 << CS10));
      setBit(&TCCR5B, (1 << CS11));
      resetBit(&TCCR5B, (1 << CS12));
      break;
    case 64:
      setBit(&TCCR5B, (1 << CS10));
      setBit(&TCCR5B, (1 << CS11));
      resetBit(&TCCR5B, (1 << CS12));
      break;
    case 256:
      resetBit(&TCCR5B, (1 << CS10));
      resetBit(&TCCR5B, (1 << CS11));
      setBit(&TCCR5B, (1 << CS12));
      break;
    case 1024:
      setBit(&TCCR5B, (1 << CS10));
      resetBit(&TCCR5B, (1 << CS11));
      setBit(&TCCR5B, (1 << CS12));
      break;
    default:
      Serial.print("ERROR: unknown prescalar:");
      Serial.println(prescale);
      setBit(&TCCR5B, (1 << CS10));
      resetBit(&TCCR5B, (1 << CS11));
      resetBit(&TCCR5B, (1 << CS12));
      break;
  }
  setBit(&TIMSK5, (1 << TOIE5));
}
int SD_OpenFile(File *file , char *fileName) {
  file->close();
  if (!SD.exists(fileName)) {
    Serial.println("ERROR: file dose not exist");
    setState(GCodeStop);
    return -1;
  }
  *file = SD.open(fileName, FILE_READ);
  if (!file) {
    Serial.println("ERROR: file cant be open");
    setState(GCodeStop);
    return -1;
  }
  return 0;
}
char* SD_ReadLine(File* file) {
  static char newLine[128];
  newLine[0] = '@';
  if (!file) {
    Serial.println("ERROR: file is not open");
    return newLine;
  }
  if (file->available())
  {
    //cli();
    int write_idx = 0;
    char read_buf = '\0';
    while (file->available() && read_buf != '\n')
    {
      cli();
      read_buf = file->read();
      sei();
      if (read_buf == '\n') //detekt end of line
      {
        newLine[write_idx] = '\0';
      } else if (read_buf == ';') { //remove comments out of the G-Code
        while (read_buf != '\n')
        {
          cli();
          read_buf = file->read();
          sei();
        }
        newLine[write_idx] = '\0';
      }
      else
      { //read chars from file and safe it
        newLine[write_idx] = read_buf;
        write_idx++;
      }
    }
    //sei();
  }
  return newLine;
}
void SD_CloseFile(File* file) {
  file->close();
}
void SD_printDirectory(File &myFile, File dir, int numTabs) {
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
      SD_printDirectory(myFile, entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
int SD_removeFile(const char* fileName) {
  if (SD.exists(fileName)) {
    SD.remove(fileName);
    Serial.print("remove_file: delete ");
    Serial.println(fileName);
    return 0;
  }
  Serial.println("remove_file: file not exist");
  return -1;
}
void SD_createFile(File *myFile, const char* fileName) {
  if (SD.exists(fileName)) {
    SD.remove(fileName);
    Serial.println("remove file");
  }
  *myFile = SD.open(fileName, FILE_WRITE);
  Serial.print("create file to write ");
  Serial.println(fileName);
  if (!*myFile) {
    Serial.println("ERROR: create_file: errorFileOpen");
    setState(GCodeStop);
  } else {
    Serial.println("GC_NL");
  }
}
void SD_writeToSD(File *myFile, char* newLine) {
  if (ComandParser(newLine) == Q102)
  {
    Serial.println("Finish Create File");
    myFile->close();
    setState(GCodeStop);
    return;
  }
  //Serial.println("writeToSD: befor write");
  myFile->println(newLine);
  Serial.println("GC_NL");
}
eComandName ComandParser(char* command_line) {
  //look for a G-Code Comand in Line
  char wort[10];
  int command_length;
  char* command_end = strchr(command_line, ' ');
  if (command_end == 0 && sizeof(command_line) != 0)
    command_length = sizeof(command_line)+1;
  else
    command_length = command_end - command_line;
  strncpy(wort, command_line, command_length);
  wort[command_length] = '\0';
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
int LineParser(char* command_line, comParam *newParam) {
  //cheack for values in the G-Code Line
  char trennzeichen[] = " ";
  char *wort;
  char worker[128];

  strcpy(worker, command_line);
  newParam->useX = false;
  newParam->useY = false;
  newParam->useZ = false;
  newParam->useE = false;
  newParam->useF = false;
  newParam->useS = false;

  wort = strtok(worker, trennzeichen);
  while (wort != NULL) {
    switch (wort[0]) {
      case 'X':
        wort++;
        newParam->X = atof(wort);
        newParam->useX = true;
        //Xachse.ESstate = 0;
        break;
      case 'Y':
        wort++;
        newParam->Y = atof(wort);
        newParam->useY = true;
        //Yachse.ESstate = 0;
        break;
      case 'Z':
        wort++;
        newParam->Z = atof(wort);
        newParam->useZ = true;
        //Zachse.ESstate = 0;
        break;
      case 'E':
        wort++;
        newParam->E = atof(wort);
        newParam->useE = true;
        break;
      case 'S':
        wort++;
        newParam->S = atof(wort);
        newParam->useS = true;
        break;
      case 'F':
        wort++;
        newParam->F = atof(wort) / 60;
        newParam->useF = true;
        break;
    }
    wort = strtok(NULL, trennzeichen);
  }
  return 0;
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
void cb_init(circular_buffer *cb, size_t capacity, size_t sz) {
  cb->buffer = malloc(capacity * sz);
  if (cb->buffer == NULL) {}
  // handle error
  cb->buffer_end = (char *)cb->buffer + capacity * sz;
  cb->capacity = capacity;
  cb->count = 0;
  cb->sz = sz;
  cb->head = cb->buffer;
  cb->tail = cb->buffer;
}
void cb_free(circular_buffer *cb) {
  free(cb->buffer);
  // clear out other fields too, just to be safe
}
void cb_clear(circular_buffer *cb) {
  cb->count = 0;
  cb->head = cb->buffer;
  cb->tail = cb->buffer;
  Serial.println("WARNING deleting buffer");
}
int cb_push_back(circular_buffer *cb, const void *item) {
  while (cb->count == cb->capacity) {
    doStdTasks();
  }
  cli();
  memcpy(cb->head, item, cb->sz);
  cb->head = (char*)cb->head + cb->sz;
  if (cb->head == cb->buffer_end)
    cb->head = cb->buffer;
  cb->count++;
  if(!readBit(TCCR5B, (1 << CS10))&&!readBit(TCCR5B, (1 << CS11))&&!readBit(TCCR5B, (1 << CS12)))
  {
    //Serial.println("restat timer");
    TCNT5 = T_MAX;
    startTimer(1);
  }
  sei();
  return cb->count;
}
int cb_pop_front(circular_buffer *cb, void *item) {
  if (cb->count <= 0) {
    return -1;
  }
  memcpy(item, cb->tail, cb->sz);
  cb->tail = (char*)cb->tail + cb->sz;
  if (cb->tail == cb->buffer_end)
    cb->tail = cb->buffer;
  cb->count--;
  return cb->count;
}
int cb_peek_front(circular_buffer *cb, void *item) {
  if (cb->count <= 0) {
    return -1;
  }
  memcpy(item, cb->tail, cb->sz);
  return cb->count;
}
void doStdTasks() {
  //task that have to be performed also while waiting to add a point
  checkSerial();
  checkEndswitches();
  TempControle();
  //regulate temperature
  //status aupdates
}
void setWaitForHeat(bool h) {
  waitForHeat = h;
  Serial.print("waitForHeat ");
  Serial.println(waitForHeat);
}
void TempControle() {
  if (myPID.Compute())
  {
    bitwertNTC = analogRead(sensorPin);      // lese Analogwert an A0 aus
    widerstandNTC = widerstand1 * (((double)bitwertNTC / 1024) / (1 - ((double)bitwertNTC / 1024)));
    TKelvin = 1 / ((1 / Tn) + ((double)1 / bWert) * log((double)widerstandNTC / Rn));
    T = TKelvin - kelvintemp;
    analogWrite(temprelai, Output);

    if (waitForHeat == true) {
      if (abs(T - soll_T) < 5) {
        setWaitForHeat(false);
      }
    }
  }
  
  if (time_now - cycle_time1 > 5000000)
    {
      if (soll_T != 1) {
        sendTemperature();
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
void sendTemperature(){
  Serial.print("Temp THsoll");
  Serial.print(soll_T);
  Serial.print(" THist");
  Serial.print(T);
  Serial.print(" TBsoll");
  Serial.print(soll_T_Bed);
  Serial.print(" TBist");
  Serial.println(T_Bed);
}
void setBit(volatile uint8_t* B, unsigned char b) {
  //set a bit in a Byte
  *B |= b;
}
void resetBit(volatile uint8_t* B, unsigned char b) {
  //reset a bit in a Byte
  *B &= ~b;
}
bool readBit(unsigned char B, unsigned char b) {
  return b == (B & b);
}
void applayValues(comParam* newValue, double *X, double *Y, double *Z, double *E, double *S, double *F) {
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

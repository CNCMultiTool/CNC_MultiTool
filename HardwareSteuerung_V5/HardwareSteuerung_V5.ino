#include <SPI.h>
#include <SD.h>
#include <string.h>
#include <math.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include <PID_v1.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

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
  G92,//set position to
  M104,//set hotend temperature
  M114,//get position
  M140,//set bed temperatur
  M109,//set hotend temperature
  M190,//set bed temperatur
  Q10,//max Accelaration//acc- max- min- speed
  Q11,//set endswitch use
  Q12,//set motor ENA
  Q13,//reset wait for heat
  Q14,//empty movebuffer buffer
  Q20,//max speed//bitwertNTC widerstandNTC widerstand1   for hotend
  Q21,//min speed//P I D ON for hotend
  Q30,//min speed//bitwertNTC widerstandNTC widerstand1   for bed
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
  char com;
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
sComand lComandList[26] = {
  {XXX, "XXX"},
  {G1, "G1"},//move
  {G9, "G9"},//stop
  {G92, "G92"},//set pos
  {M104, "M104"},//set hotend temp
  {M114, "M114"},//get pos
  {M140, "M140"},//set bed temp
  {M109, "M109"},//set and wait hotend temp
  {M190, "M190"},//set and wait bed temp
  {Q10, "Q10"},//move params
  {Q11, "Q11"},//switch endswitch on off
  {Q12, "Q12"},// switch motors on off
  {Q13, "Q13"},//set wait for heat off
  {Q14, "Q14"},//empty command buffer
  {Q20, "Q20"},//thermo fühler NTC hotend
  {Q21, "Q21"},//regler hotend
  {Q30, "Q30"},//thermo fühler NTC bed
};

circular_buffer cbSteps;
circular_buffer cbCommands;

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

  //  for(int i =0;i<5;i++){
  //  if(SD.begin(53))break;
  //  Serial.println("ERROR: SD initialization failed!");
  //  delay(1000);
  //}

  //Serial.println("SD initializ");

  //init the timer5
  TCNT5 = 0;
  TCCR5A = 0x00;
  TCCR5B = 0x00;
  TCCR5C = 0x00;
  setBit(&TIMSK5, (1 << TOIE5));//enable timer overflow

  cb_init(&cbSteps, 100, sizeof(stepParam));
  //cb_init(&cbRawCommands, 3, sizeof(comParam));
  cb_init(&cbCommands, 15, sizeof(comParam));
  
  setState(GCodeStop);
  Serial.println("RESTART Arduino compleated");

  setMotorsENA(true);
}
void loop() {
  //startTimer(1);//start intterrupt timer
  while (true) {
    time_now = micros();
    doStdTasks();
    calcPreRunPointer();
    calculateSteps();
  }
}
void setMotorsENA(bool b) {
  Serial.print("motorState ");
  Serial.println(b);
  digitalWrite(Xachse.pinENA, !b);
  digitalWrite(Yachse.pinENA, !b);
  digitalWrite(Zachse.pinENA, !b);
  digitalWrite(Eachse.pinENA, !b);
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
    nextX = 1000000 / (abs(nextPrePos.Xv) * Xachse.steps_pmm);
    nextPrePos.useX = true;
    //Serial.print("activeX ");
    //Serial.println(nextPrePos.Xv);
  }else{
    nextX = 4294967295;
    nextPrePos.useX = false;
  }
  if (nextPrePos.Ys != prePos.Ys){  
    nextY = 1000000 / (abs(nextPrePos.Yv) * Yachse.steps_pmm);
    nextPrePos.useY = true;
    //Serial.print("activeY ");
    //Serial.println(nextPrePos.Yv);
  }else{
    nextY = 4294967295;
    nextPrePos.useY = false;
  }
  if (nextPrePos.Zs != prePos.Zs){
    nextZ = 1000000 / (abs(nextPrePos.Zv) * Zachse.steps_pmm);
    nextPrePos.useZ = true;
  }else{
    nextZ = 4294967295;
    nextPrePos.useZ = false;
  }
  if (nextPrePos.Es != prePos.Es){
    nextE = 1000000 / (abs(nextPrePos.Ev) * Eachse.steps_pmm);
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
    //doStdTasks();
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
  else
    nextPrePos.useX = true;

  if (nextPrePos.Ys == prePos.Ys)
    nextPrePos.useY = false;
  else
    nextPrePos.useY = true;

  if (nextPrePos.Zs == prePos.Zs)
    nextPrePos.useZ = false;
  else
    nextPrePos.useZ = true;

  if (nextPrePos.Es == prePos.Es)
    nextPrePos.useE = false;
  else
    nextPrePos.useE = true;
 
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
comParam getCommandFromLine(char* newLine){
  comParam newCommand;
  newCommand.com = ComandParser(newLine);
  if (newCommand.com == G9) {
    Serial.println("find G9 ");
  } else {
    LineParser(newLine, &newCommand);
    //Serial.print("values: ");
    //Serial.println(newLine);
  }
  return newCommand;
}
int calcPreRunPointer() {
  char *newLine;
  comParam nextCommand;
  comParam newCommand;

  if(waitForHeat == true ){
    return 0;
  }

  if(cbCommands.count == 0){
    doStdTasks();
    return 0;
  }

  cb_pop_front(&cbCommands,&newCommand);
  if(cbCommands.count < 10){
    requestNextCommands();
  }
  processComandLine(newCommand,false);
  return 0;
}
void calcNextPos(comParam *newCommand){
  setNextPrePos(newCommand);
  double gesDif = getTravelDist(&prePos, &nextPrePos);
  calcSpeedForAches(gesDif,&prePos, &nextPrePos);
  //calcAccForAches(gesDif,&prePos, &nextPrePos);
}
void processComandLine(comParam newCommand,bool doNow) {
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
    setUseES(newCommand.X);
  } else if (newCommand.com == Q12) {
    setMotorsENA(newCommand.X);
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
  } else {
    Serial.println("ERROR: unknown GCode");
  }
}
void setUseES(bool newES){
    useES = newES;
    Serial.print("useES ");
    Serial.println(useES);
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
    nextPrePos.useX = true;
  }
  if (newPos->useY) {
    nextPrePos.Yp = newPos->Y;
    nextPrePos.Ys = nextPrePos.Yp * float(Yachse.steps_pmm);
    nextPrePos.useY = true;
  }
  if (newPos->useZ) {
    nextPrePos.Zp = newPos->Z;
    nextPrePos.Zs = nextPrePos.Zp * float(Zachse.steps_pmm);
    nextPrePos.useZ = true;
  }
  if (newPos->useE) {
    nextPrePos.Ep = newPos->E;
    nextPrePos.Es = nextPrePos.Ep * float(Eachse.steps_pmm);
    nextPrePos.useE = true;
  }
  if (newPos->useF)
    nextPrePos.Speed = newPos->F;
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
  if (useES) {
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
  if(SR_CheckForLine() != 0) {
    char buffer[128];
    size_t recLen = cobsDecode(reciveBuf,reciveWindex, buffer);
    reciveWindex = 0;

    //check the length of the pac
    if(recLen-1 != int(buffer[0])){
      //error handling
      Serial.println("resend");
      return 0;
    }

    //check the checksumm of the package
    char checksumm = 0;
    for(int i = 1;i<recLen-2;i++){
      checksumm += buffer[i];
    }
    if(checksumm != buffer[recLen-2]){
      //error handling
      Serial.println("resend");
      return 0;
    }

    //pass all checks send package acknolage
    Serial.println("rec");//answer to stop the timeout
    //Serial.println("successfully");

    //read command
    //read values
    comParam recCom = parseValuesFromBinar(recLen,buffer);
    memset(&reciveBuf[0], 0, sizeof(reciveBuf));

    //sofort comands list
    if(recCom.com == 11){//G9
      Serial.println("find and process G9");
      processComandLine(recCom,false);
      return 0;
    }
    if(recCom.com == 20){//Q13
      setWaitForHeat(false);
      return 0;
    }
    if(recCom.com == 25){//Q14
      cb_clear(&cbCommands);
      StopMove();
      sendDeviceStatus();
      return 0;
    }

    if(cbCommands.count < cbCommands.capacity){
      cb_push_back(&cbCommands,&recCom);
      if(cbCommands.count < 10){
        requestNextCommands();
      }
    }else{
      Serial.println("error: buffer overrun");
    }
  }
}
comParam parseValuesFromBinar(int bufLen,char* buffer){
  comParam newParam;
  float f;
  newParam.useX = false;
  newParam.useY = false;
  newParam.useZ = false;
  newParam.useE = false;
  newParam.useF = false;
  newParam.useS = false;
  newParam.com = buffer[2];
  for(int i = 2;i+4 < bufLen-2;i+=5){
    char b[] = {buffer[i+1], buffer[i+2], buffer[i+3], buffer[i+4]};
    memcpy(&f, &b, sizeof(f));
    switch (buffer[i]) {
      case 1://'X':
        newParam.X = f;
        newParam.useX = true;
        break;
      case 2://'Y':
        newParam.Y = f;
        newParam.useY = true;
        break;
      case 3://'Z':
        newParam.Z = f;
        newParam.useZ = true;
        break;
      case 4://'E':
        newParam.E = f;
        newParam.useE = true;
        break;
      case 5://'S':
        newParam.S = f;
        newParam.useS = true;
        break;
      case 6://'F':
        newParam.F = f;
        newParam.useF = true;
        break;
    }
  }
  return newParam;
}
/** COBS encode data to buffer
	@param data Pointer to input data to encode
	@param length Number of bytes to encode
	@param buffer Pointer to encoded output buffer
	@return Encoded buffer length in bytes
	@note Does not output delimiter byte
*/
size_t cobsEncode(const void *data, size_t length, uint8_t *buffer)
{
	assert(data && buffer);

	uint8_t *encode = buffer; // Encoded byte pointer
	uint8_t *codep = encode++; // Output code pointer
	uint8_t code = 1; // Code value

	for (const uint8_t *byte = (const uint8_t *)data; length--; ++byte)
	{
		if (*byte) // Byte not zero, write it
			*encode++ = *byte, ++code;

		if (!*byte || code == 0xff) // Input is zero or block completed, restart
		{
			*codep = code, code = 1, codep = encode;
			if (!*byte || length)
				++encode;
		}
	}
	*codep = code; // Write final code value

	return (size_t)(encode - buffer);
}
/** COBS decode data from buffer
	@param buffer Pointer to encoded input bytes
	@param length Number of bytes to decode
	@param data Pointer to decoded output data
	@return Number of bytes successfully decoded
	@note Stops decoding if delimiter byte is found
*/
size_t cobsDecode(const uint8_t *buffer, size_t length, void *data)
{
	assert(buffer && data);

	const uint8_t *byte = buffer; // Encoded input byte pointer
	uint8_t *decode = (uint8_t *)data; // Decoded output byte pointer

	for (uint8_t code = 0xff, block = 0; byte < buffer + length; --block)
	{
		if (block) // Decode block byte
			*decode++ = *byte++;
		else
		{
			if (code != 0xff) // Encoded zero, write it
				*decode++ = 0;
			block = code = *byte++; // Next block length
			if (!code) // Delimiter code found
				break;
		}
	}

	return (size_t)(decode - (uint8_t *)data);
}
void requestNextCommands(){
  Serial.print("request ");
  Serial.println(10 - cbCommands.count);
}
int SR_CheckForLine() {
  char caLine[1];
  comParam newCommand;
  while (Serial.available()) {
    Serial.readBytes(caLine, 1);
    reciveBuf[reciveWindex] = caLine[0];
    reciveWindex++;
    if (caLine[0] == 0)
    {
      return reciveWindex;
    }
  }
  return 0;
}
ISR (TIMER5_OVF_vect) { // Timer1 ISR
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

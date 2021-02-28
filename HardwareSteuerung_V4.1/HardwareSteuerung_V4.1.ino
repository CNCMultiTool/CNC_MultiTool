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
  bool ESstate = false;
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
} comParam;
typedef struct  stepParam {
  eAchse achse;
  bool dir;
  unsigned int ticks;
  int preScale;
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
  float Yp = 0;
  long Ys = 0;
  float Yv = 0;
  float Zp = 0;
  long Zs = 0;
  float Zv = 0;
  float Ep = 0;
  long Es = 0;
  float Ev = 0;
  float Speed = 20;//mm/s
} MovePos;
StepMotorBig Xachse;
StepMotorBig Yachse;
StepMotorBig Zachse;
StepMotorBig Eachse;
sComand lComandList[22] = {
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
circular_buffer cbCommand;
eGCodeState state;
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

void setup() {
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
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  while (!SD.begin(53)) {
    Serial.println("SD initialization failed!");
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
  cb_init(&cbCommand, 10, sizeof(comParam));
  setState(GCodeStop);
  Serial.println("RESTART Arduino compleated");
}

void loop() {
  startTimer(1);//start intterrupt timer
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
      useES = false;//disable endswitches to move undisturbed in GCode
      if (calcPreRunPointer(&GFile) == -1 && cbSteps.count == 0)
        setState(GCodeStop);
    } else if (state == GCodePause) {
      useES = true;//enable endswitches for manuel movements
    } else if (state == GCodeStop) {
      useES = true;//enable endswitches for manuel movements
      SD_CloseFile(&GFile);
      if (getTravelDist(&prePos, &nextPrePos) == 0) {
        setMotorsENA(true);
      }
    } else if (state == GCodeCreate) {
      while (getState() == GCodeCreate) {
        if (SR_CheckForLine() == 1) {
          SD_writeToSD(&GFile, reciveBuf);
        }
      }
    } else if (state == GCodeStartHome) {
      if (SD_OpenFile(&HomeFile , "home.txt") == 0) {
        setState(GCodeRunHome);
        useES = true;
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
      Serial.println("unknown State");
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
//  prePos.Xs = float(prePos.Xp) * float(Xachse.steps_pmm);
//  prePos.Ys = float(prePos.Yp) * float(Yachse.steps_pmm);
//  prePos.Zs = float(prePos.Zp) * float(Zachse.steps_pmm);
//  prePos.Es = float(prePos.Ep) * float(Eachse.steps_pmm);
  //check for Vmax Vmin
  if (Vmax < nextPrePos.Speed)nextPrePos.Speed = Vmax;
  if (Vmin > nextPrePos.Speed)nextPrePos.Speed = Vmin;
  //get travel dist
  float gesDif = getTravelDist(&prePos, &nextPrePos);
  if (gesDif < 0.001)
    return;
  //get travel achseSpeed  Xv/Xdif = Speed/GesDif
  nextPrePos.Xv = (abs(prePos.Xp - nextPrePos.Xp) / gesDif) * nextPrePos.Speed;
  nextPrePos.Yv = (abs(prePos.Yp - nextPrePos.Yp) / gesDif) * nextPrePos.Speed;
  nextPrePos.Zv = (abs(prePos.Zp - nextPrePos.Zp) / gesDif) * nextPrePos.Speed;
  nextPrePos.Ev = (abs(prePos.Ep - nextPrePos.Ep) / gesDif) * nextPrePos.Speed;
  //calculate step time x y z e sort in buffer
  //  Serial.print("Xv");
  //  Serial.print(nextPrePos.Xv);
  //  Serial.print(" Yv");
  //  Serial.print(nextPrePos.Yv);
  //  Serial.print(" Zv");
  //  Serial.print(nextPrePos.Zv);
  //  Serial.print(" Ev");
  //  Serial.println(nextPrePos.Ev);
  unsigned long nextStepTime = 0;
  unsigned long noStep = -1;
  unsigned long lastMoveTime = 0;
  unsigned long nextX = 1000000 / (nextPrePos.Xv * Xachse.steps_pmm);
  if (nextPrePos.Xs == prePos.Xs)nextX = -1;
  unsigned long nextY = 1000000 / (nextPrePos.Yv * Yachse.steps_pmm);
  if (nextPrePos.Ys == prePos.Ys)nextY = -1;
  unsigned long nextZ = 1000000 / (nextPrePos.Zv * Zachse.steps_pmm);
  if (nextPrePos.Zs == prePos.Zs)nextZ = -1;
  unsigned long nextE = 1000000 / (nextPrePos.Ev * Eachse.steps_pmm);
  if (nextPrePos.Es == prePos.Es)nextE = -1;
  //  Serial.print("nextX");
  //  Serial.print(nextX);
  //  Serial.print(" nextY");
  //  Serial.print(nextY);
  //  Serial.print(" nextZ");
  //  Serial.print(nextZ);
  //  Serial.print(" nextE");
  //  Serial.println(nextE);
  while (!prePointerOnPos()) {
    doStdTasks();
    switch (getSmalest(nextX, nextY, nextZ, nextE))
    {
      case X:
        //      X
        lastMoveTime = nextX;
        nextX += 1000000 / (nextPrePos.Xv * Xachse.steps_pmm);
        nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
        createStep(X, &nextPrePos.Xs, &prePos.Xs, nextStepTime - lastMoveTime);
        //        Serial.print("X lastMoveTime:");
        //        Serial.print(lastMoveTime);
        //        Serial.print(" nextX:");
        //        Serial.print(nextX);
        //        Serial.print(" nextStepTime:");
        //        Serial.print(nextStepTime);
        //        Serial.print(" nextStepTime-lastMoveTime:");
        //        Serial.println(nextStepTime-lastMoveTime);
        break;
      case Y:
        //      Y
        lastMoveTime = nextY;
        nextY += 1000000 / (nextPrePos.Yv * Yachse.steps_pmm);
        nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
        createStep(Y, &nextPrePos.Ys, &prePos.Ys, nextStepTime - lastMoveTime);
        break;
      case Z:
        //Z
        lastMoveTime = nextZ;
        nextZ += 1000000 / (nextPrePos.Zv * Zachse.steps_pmm);
        nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
        createStep(Z, &nextPrePos.Zs, &prePos.Zs, nextStepTime - lastMoveTime);
        break;
      case E:
        //E
        lastMoveTime = nextE;
        nextE += 1000000 / (nextPrePos.Ev * Eachse.steps_pmm);
        nextStepTime = getSmalestValue(nextX, nextY, nextZ, nextE);
        createStep(E, &nextPrePos.Es, &prePos.Es, nextStepTime - lastMoveTime);
        break;
    }
  }

  prePos.Xp = float(prePos.Xs) / float(Xachse.steps_pmm);
  prePos.Yp = float(prePos.Ys) / float(Yachse.steps_pmm);
  prePos.Zp = float(prePos.Zs) / float(Zachse.steps_pmm);
  prePos.Ep = float(prePos.Es) / float(Eachse.steps_pmm);
}
eAchse getSmalest(unsigned long x, unsigned long y, unsigned long z, unsigned long e) {
  if (x < y && x < z && x < e)
    return X;
  if (y < z && y < e)
    return Y;
  if (z < e)
    return Z;
  return E;
}
unsigned long getSmalestValue(unsigned long x, unsigned long y, unsigned long z, unsigned long e) {
  if (x < y && x < z && x < e)
    return x;
  if (y < z && y < e)
    return y;
  if (z < e)
    return z;
  return e;
}
bool prePointerOnPos() {
  if (nextPrePos.Xs == prePos.Xs) {
    if (nextPrePos.Ys == prePos.Ys) {
      if (nextPrePos.Zs == prePos.Zs) {
        if (nextPrePos.Es == prePos.Es) {
          return 1;
        }
      }
    }
  }
  return 0;
}
void createStep(eAchse achse, long *sollStep, long *istStep, double us) {
  //Serial.print("createStep ");
  //Serial.println(achse);
  //Serial.println(us);
  stepParam newStep;
  newStep.achse = achse;
  usToTimer(&newStep, us);
  if (*sollStep > *istStep) {
    newStep.dir = 1;
    cb_push_back(&cbSteps, &newStep);
    *istStep += 1;
  } else if (*sollStep < *istStep) {
    newStep.dir = 0;
    cb_push_back(&cbSteps, &newStep);
    *istStep -= 1;
  }
}
float getTravelDist(MovePos* pPos, MovePos* nPrePos) {
  float Xdif = pPos->Xp - nPrePos->Xp;
  float Ydif = pPos->Yp - nPrePos->Yp;
  float Zdif = pPos->Zp - nPrePos->Zp;
  float gesDif = sqrt(pow(Xdif, 2) + pow(Ydif, 2) + pow(Zdif, 2));
  if (gesDif == 0)
    return abs(pPos->Ep - nPrePos->Ep);
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

  //  Serial.print(" pre:");
  //  Serial.print(myStep->preScale);
  //  Serial.print(" ticks:");
  //  Serial.println(myStep->ticks);
}
int calcPreRunPointer(File* gFile) {
  char *newLine;
  do {
    newLine = SD_ReadLine(gFile);
  } while (newLine[0] == '\0');
  if (newLine[0] == '@') {
    return -1;
  }
  processComandLine(newLine,false);
  return 0;
}
void processComandLine(char* newLine,bool doNow) {
  comParam newCommand;
  char* fileName;
  newCommand.com = ComandParser(newLine);

  if (newCommand.com == Q100 ||
      newCommand.com == Q101 ||
      newCommand.com == Q107) {
    fileName = getName(newLine);
    Serial.print("name: ");
    Serial.println(fileName);
  } else if (newCommand.com == G9) {
    Serial.println("find G9 ");
    startTimer(1);
  } else {
    LineParser(newLine, &newCommand);
    //Serial.print("values: ");
    Serial.println(newLine);
  }

  //Serial.print("paresed ");
  if (newCommand.com == G1) {
    if (newCommand.useF) {
      if (Vmax < newCommand.F)newCommand.F = Vmax;
      if (Vmin > newCommand.F)newCommand.F = Vmin;
    }
    if (newCommand.useF) {
      Serial.print("G1 F");
      Serial.println(newCommand.F);
    }
    setNextPrePos(&newCommand);
  } else if (newCommand.com == G9) { //stop movement
    Serial.println("G9");
    StopMove();
    sendDeviceStatus();
  } else if (newCommand.com == G28) { //start move home
    Serial.println("G28");
    setState(GCodeStartHome);
  } else if (newCommand.com == G92) { //set pos
    //Serial.println("G92");
    setPrePos(&newCommand);
    setNextPrePos(&newCommand);
    addCommand(&newCommand,doNow);
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
    strcpy(GCodeFileName, fileName);
    if (getState() == GCodeStop)
      setState(GCodeStart);
  } else if (newCommand.com == Q101) { //open file to write G-Code in it
    Serial.println("Q101");
    if (getState() == GCodeStop) {
      setState(GCodeCreate);
      SD_createFile(&GFile, fileName);
    } else
      Serial.println("write can only start when stoped");
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
    SD_removeFile(fileName);
  } else {
    Serial.print("unknown GCode: ");
    Serial.println(newLine);
  }
  sendDeviceStatus();
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
  cb_clear(&cbCommand);
}
void StopAchse(eAchse achse) {
  comParam newPos;
  switch (achse) {
    case X:
      newPos.useX = true;
      newPos.X = prePos.Xp;
      //newPos.X = float(Xachse.act_step) / float(Xachse.steps_pmm);
      break;
    case Y:
      newPos.useY = true;
      newPos.Y = prePos.Yp;
      //newPos.Y = float(Yachse.act_step) / float(Yachse.steps_pmm);
      break;
    case Z:
      newPos.useZ = true;
      newPos.Z = prePos.Zp;
      //newPos.Z = float(Zachse.act_step) / float(Zachse.steps_pmm);
      break;
    case E:
      newPos.useE = true;
      newPos.E = prePos.Ep;
      //newPos.E = float(Eachse.act_step) / float(Eachse.steps_pmm);
      break;
  }
  setNextPrePos(&newPos);
  setPrePos(&newPos);
}
void setNextPrePos(comParam* newPos) {
  if (newPos->useX) {
    Serial.println("set npp x ");
    //Serial.print(newPos->X);
    nextPrePos.Xp = newPos->X;
    nextPrePos.Xs = nextPrePos.Xp * float(Xachse.steps_pmm);
  }
  if (newPos->useY) {
    Serial.println("set npp y ");
    //Serial.print(newPos->Y);
    nextPrePos.Yp = newPos->Y;
    nextPrePos.Ys = nextPrePos.Yp * float(Yachse.steps_pmm);
  }
  if (newPos->useZ) {
    Serial.println("set npp z ");
    //Serial.print(newPos->Z);
    nextPrePos.Zp = newPos->Z;
    nextPrePos.Zs = nextPrePos.Zp * float(Zachse.steps_pmm);
  }
  if (newPos->useE) {
    Serial.println("set npp e ");
    nextPrePos.Ep = newPos->E;
    nextPrePos.Es = nextPrePos.Ep * float(Eachse.steps_pmm);
  }
  if (newPos->useF)
    nextPrePos.Speed = newPos->F;
  //Serial.println();
}
void setPrePos(comParam* newPos) {
  if (newPos->useX){
    Serial.println("set pp x ");
    //Serial.print(newPos->X);
    prePos.Xp = newPos->X;
    prePos.Xs = prePos.Xp * float(Xachse.steps_pmm);
  }
  if (newPos->useY) {
    Serial.println("set pp y ");
    //Serial.print(newPos->Y);
    prePos.Yp = newPos->Y;
    prePos.Ys = prePos.Yp * float(Yachse.steps_pmm);
  }
  if (newPos->useZ) {
    Serial.println("set pp z ");
    //Serial.print(newPos->Z);
    prePos.Zp = newPos->Z;
    prePos.Zs = prePos.Zp * float(Zachse.steps_pmm);
  }
  if (newPos->useE) {
    Serial.println("set pp e ");
    prePos.Ep = newPos->E;
    prePos.Es = prePos.Ep * float(Eachse.steps_pmm);
  }
  if (newPos->useF)
    prePos.Speed = newPos->F;
  //Serial.println();
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
  if (getState() == GCodeRunHome) {
    //Serial.println("check es");
    handleES(&Xachse, Xachse.pinNull);
    handleES(&Yachse, Yachse.pinNull);
    handleES(&Zachse, Zachse.pinPlus);
  }
}
void handleES(StepMotorBig* mot, int ES) {
  if (digitalRead(ES) == HIGH && mot->ESstate == false) {
    mot->ESstate = true;
    StopAchse(mot->achse);
    Serial.print("touch es ");
    Serial.println(mot->achse);
    
  }
  if (digitalRead(ES) == LOW)
    mot->ESstate = false;
}
int checkSerial() {
  if (SR_CheckForLine() == 1) {
    setMotorsENA(false);
    processComandLine(reciveBuf,true);
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
  digitalWrite(22, HIGH);
  cli();
  stepParam nextStep;
  comParam nextCommand;
  if (cb_pop_front(&cbSteps, &nextStep) == -1 || waitForHeat == true) {
    TCNT5 = 0;//T_MAX;
    startTimer(1);
  } else {
    switch (nextStep.achse) {
      case X:
        performStep(&Xachse, nextStep.dir);
        break;
      case Y:
        performStep(&Yachse, nextStep.dir);
        break;
      case Z:
        performStep(&Zachse, nextStep.dir);
        break;
      case E:
        performStep(&Eachse, nextStep.dir);
        break;
      case C:
        if (cb_pop_front(&cbCommand, &nextCommand) == -1)
          Serial.print("missing command");
        else
          performCommand(&nextCommand);
        break;
    }

    startTimer(nextStep.preScale);
    if ((nextStep.ticks + (16.0 / (double)nextStep.preScale) * 40) > T_MAX) {
      TCNT5 = T_MAX;
    } else {
      TCNT5 = nextStep.ticks + (16.0 / (double)nextStep.preScale) * 40;
    }

  }
  sei();
  digitalWrite(22, LOW);
}
void addCommand(comParam* newCommand,bool doNow) {
  if(doNow){
    performCommand(newCommand);
  }else{
    stepParam newStep;
    cb_push_back(&cbCommand, newCommand);
    newStep.achse = C;
    newStep.ticks = T_MAX;
    newStep.preScale = 1;
    cb_push_back(&cbSteps, &newStep);
  }
}
void performStep(StepMotorBig *mot, bool dir) {
  if (dir && useES && mot->pinPlus != 0) {
    if (digitalRead(mot->pinPlus) == HIGH) {
      return;
    }
  } else if (!dir && useES && mot->pinNull != 0) {
    if (digitalRead(mot->pinNull) == HIGH) {
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
      setWaitForHeat(false);
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
      Serial.print("unknown Command ");
      Serial.println(newCommand->com);
  }
}
void startTimer(int prescale = 1) {
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
      Serial.print("unknown prescalar:");
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
    Serial.println("ERROR file dose not exist");
    setState(GCodeStop);
    return -1;
  }
  *file = SD.open(fileName, FILE_READ);
  if (!file) {
    Serial.println("ERROR file cant be open");
    setState(GCodeStop);
    return -1;
  }
  return 0;
}
char* SD_ReadLine(File* file) {
  static char newLine[128];
  newLine[0] = '@';
  if (!file) {
    Serial.println("ERROR file is not open");
    return newLine;
  }
  if (file->available())
  {
    cli();
    int write_idx = 0;
    char read_buf = "";
    while (file->available() && read_buf != '\n')
    {
      read_buf = file->read();
      if (read_buf == '\n') //detekt end of line
      {
        newLine[write_idx] = '\0';
      } else if (read_buf == ';') { //remove comments out of the G-Code
        while (read_buf != '\n')
        {
          read_buf = file->read();
        }
        newLine[write_idx] = '\0';
      }
      else
      { //read chars from file and safe it
        newLine[write_idx] = read_buf;
        write_idx++;
      }
    }
    sei();
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
    Serial.println("create_file: errorFileOpen");
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
        Xachse.ESstate = false;
        break;
      case 'Y':
        wort++;
        newParam->Y = atof(wort);
        newParam->useY = true;
        Yachse.ESstate = false;
        break;
      case 'Z':
        wort++;
        newParam->Z = atof(wort);
        newParam->useZ = true;
        Zachse.ESstate = false;
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
}
int cb_push_back(circular_buffer *cb, const void *item) {
  while (cb->count == cb->capacity) {
    digitalWrite(24, HIGH);
    doStdTasks();
  }
  digitalWrite(24, LOW);
  memcpy(cb->head, item, cb->sz);
  cb->head = (char*)cb->head + cb->sz;
  if (cb->head == cb->buffer_end)
    cb->head = cb->buffer;
  cb->count++;
  return cb->count;
}
int cb_pop_front(circular_buffer *cb, void *item) {
  if (cb->count == 0) {
    return -1;
  }
  memcpy(item, cb->tail, cb->sz);
  cb->tail = (char*)cb->tail + cb->sz;
  if (cb->tail == cb->buffer_end)
    cb->tail = cb->buffer;
  cb->count--;
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
  Serial.print("wait for heat:");
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

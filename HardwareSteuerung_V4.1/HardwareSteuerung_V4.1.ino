#include <SPI.h>
#include <SD.h>
#include <string.h>
#include <math.h>
#include<avr/io.h>
#include<avr/interrupt.h>

#define CPU 16000000.00
#define T_MAX 65535.00
#define PRESCALE 64

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
  int ticks;
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
  float Yp = 0;
  long Ys = 0;
  float Zp = 0;
  long Zs = 0;
  float Ep = 0;
  long Es = 0;
  float Speed = 0;//mm/s
} MovePos;

StepMotorBig Xachse;
StepMotorBig Yachse;
StepMotorBig Zachse;
StepMotorBig Eachse;
sComand lComandList[20] = {
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

//settings/Parameter
bool useES = true;

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

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);//turn off heating

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

  cb_init(&cbSteps, 100, sizeof(stepParam));
  cb_init(&cbCommand, 10, sizeof(comParam));
  setState(GCodeStop);
  Serial.println("RESTART Arduino compleated");
}

void loop() {
  startTimer(PRESCALE);

  Serial.println("wait for serial");
  while (true) {
    doStdTasks();
    switch (state) {
      case GCodeStart:
        if (SD_OpenFile(&GFile , GCodeFileName) == 0)
          setState(GCodeRun);
        break;
      case GCodeRun:
        //useES = false;//disable endswitches to move undisturbed in GCode
        if (calcPreRunPointer(&GFile) == -1 && cbSteps.count == 0)
          setState(GCodeStop);
        break;
      case GCodePause:
        useES = true;//enable endswitches for manuel movements
        break;
      case GCodeStop:
        useES = true;//enable endswitches for manuel movements
        SD_CloseFile(&GFile);
        break;
      case GCodeCreate:
        break;
      case GCodeStartHome:
        if (SD_OpenFile(&HomeFile , "home.txt") == 0) {
          setState(GCodeRunHome);
          useES = true;
        }
        break;
      case GCodeRunHome:
        if (calcPreRunPointer(&HomeFile) == -1 && cbSteps.count == 0){
          setState(GCodeStopHome);
        }
        break;
      case GCodeStopHome:
        SD_CloseFile(&HomeFile);
        setState(GCodeRun);
        break;
      default:
        Serial.println("unknown State");
    }
    calculateSteps();

  }
}

void calculateSteps() {
  prePos.Xs = float(prePos.Xp) * float(Xachse.steps_pmm);
  prePos.Ys = float(prePos.Yp) * float(Yachse.steps_pmm);
  prePos.Zs = float(prePos.Zp) * float(Zachse.steps_pmm);
  prePos.Es = float(prePos.Ep) * float(Eachse.steps_pmm);

  fillSimpeSteps(&nextPrePos.Xs, &prePos.Xs, X);
  prePos.Xp = float(prePos.Xs) / float(Xachse.steps_pmm);
  fillSimpeSteps(&nextPrePos.Ys, &prePos.Ys, Y);
  prePos.Yp = float(prePos.Ys) / float(Yachse.steps_pmm);
  fillSimpeSteps(&nextPrePos.Zs, &prePos.Zs, Z);
  prePos.Zp = float(prePos.Zs) / float(Zachse.steps_pmm);
  fillSimpeSteps(&nextPrePos.Es, &prePos.Es, E);
  prePos.Ep = float(prePos.Es) / float(Eachse.steps_pmm);
}
void fillSimpeSteps(long *sollStep, long *istStep, eAchse achse) {
  stepParam newStep;
  if (*sollStep == *istStep) {
    return;
  } else if (*sollStep > *istStep) {
    newStep.achse = achse;
    newStep.ticks = T_MAX;
    newStep.dir = 1;
    while (*sollStep > *istStep) {
      cb_push_back(&cbSteps, &newStep);
      *istStep += 1;
    }
  } else if (*sollStep < *istStep) {
    newStep.dir = 0;
    newStep.achse = achse;
    newStep.ticks = T_MAX;
    while (*sollStep < *istStep) {
      cb_push_back(&cbSteps, &newStep);
      *istStep -= 1;
    }
  }
}
int calcPreRunPointer(File* gFile) {

  char *newLine;
  newLine = SD_ReadLine(gFile);
  if (newLine[0] == '@'){
    return -1;
  }
  processComandLine(newLine);
  return 0;
}
void processComandLine(char* newLine) {
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
  } else {
    LineParser(newLine, &newCommand);
    Serial.print("values: ");
    Serial.println(newLine);
  }

  Serial.print("paresed ");
  if (newCommand.com == G1) {
    Serial.println("G1");
    setNextPrePos(&newCommand);
  } else if (newCommand.com == G9) { //stop movement
    Serial.println("G9");
    StopMove();
    sendDeviceStatus();
  } else if (newCommand.com == G28) { //start move home
    Serial.println("G28");
    setState(GCodeStartHome);
  } else if (newCommand.com == G92) { //set pos
    Serial.println("G92");
    setPrePos(&newCommand);
    setNextPrePos(&newCommand);
    addCommand(&newCommand);
  } else if (newCommand.com == M104 || //set temperature
             newCommand.com == M140) {
    Serial.println("Q144");
    addCommand(&newCommand);
  } else if (newCommand.com == M109 || //set temperature
             newCommand.com == M190) {
    Serial.println("Q199");
    addCommand(&newCommand);
  } else if (newCommand.com == Q10) { //max Accelaration//acc- max- min- speed
    Serial.println("Q10");
  } else if (newCommand.com == Q20) { //max speed//bitwertNTC widerstandNTC widerstand1   for hotend
    Serial.println("Q20");
  } else if (newCommand.com == Q21) { //min speed//P I D ON for hotend
    Serial.println("Q21");
  } else if (newCommand.com == Q30) { //min speed//bitwertNTC widerstandNTC widerstand1   for bed
    Serial.println("Q30");
  } else if (newCommand.com == Q100) { //Execute G-Code if File exist
    Serial.println("Q100");
    Serial.println(fileName);
    //*GCodeFileName = fileName;
    strcpy(GCodeFileName,fileName);
    if (getState() == GCodeStop)
      setState(GCodeStart);
  } else if (newCommand.com == Q101) { //open file to write G-Code in it
    Serial.println("Q101");
  } else if (newCommand.com == Q102) { //close file after write G-Code
    Serial.println("Q102");
  } else if (newCommand.com == Q103) { //Pause G-Code
    Serial.println("Q103");
  } else if (newCommand.com == Q104) { //Continue G-Code
    Serial.println("Q104");
  } else if (newCommand.com == Q105) { //Abord G-code
    Serial.println("Q105");
    if (getState() == GCodeRun){
      setState(GCodeStop);
    }else if (getState() == GCodeRunHome){
      setState(GCodeStopHome);
    }
  } else if (newCommand.com == Q106) { //get list of File on SD Card
    Serial.println("Q106");
    File root = SD.open("/");
    SD_printDirectory(GFile, root, 0);
  } else if (newCommand.com == Q107) { //Delete File
    Serial.println("Q107");
    SD_removeFile(GFile, fileName);
  } else {
    Serial.println("unknown GCode");
  }
}
void sendDeviceStatus() {
  Serial.print("npP_x:");
  Serial.print(nextPrePos.Xp);
  Serial.print(" pP_x:");
  Serial.print(prePos.Xp);
  Serial.print(" rP_x:");
  Serial.println(float(Xachse.act_step) / float(Xachse.steps_pmm));
  Serial.print("npP_y:");
  Serial.print(nextPrePos.Yp);
  Serial.print(" pP_y:");
  Serial.print(prePos.Yp);
  Serial.print(" rP_y:");
  Serial.println(float(Yachse.act_step) / float(Yachse.steps_pmm));
  Serial.print("npP_z:");
  Serial.print(nextPrePos.Zp);
  Serial.print(" pP_z:");
  Serial.print(prePos.Zp);
  Serial.print(" rP_z:");
  Serial.println(float(Zachse.act_step) / float(Zachse.steps_pmm));
  Serial.print("npS_z:");
  Serial.print(float(nextPrePos.Zs));
  Serial.print(" pS_z:");
  Serial.print(float(prePos.Zs));
  Serial.print(" rS_z:");
  Serial.println(float(Zachse.act_step));
  Serial.print("count ");
  Serial.println(cbSteps.count);
  Serial.print("State ");
  Serial.println(state);
}
void StopMove() {
  StopAchse(X);
  StopAchse(Y);
  StopAchse(Z);
  StopAchse(E);
}
void StopAchse(eAchse achse) {
  comParam newPos;
  float pos;
  switch (achse) {
    case X:
      newPos.useX = true;
      newPos.X = float(Xachse.act_step) / float(Xachse.steps_pmm);
      break;
    case Y:
      newPos.useY = true;
      newPos.Y = float(Yachse.act_step) / float(Yachse.steps_pmm);
      break;
    case Z:
      newPos.useZ = true;
      newPos.Z = float(Zachse.act_step) / float(Zachse.steps_pmm);
      break;
    case E:
      newPos.useE = true;
      newPos.E = float(Eachse.act_step) / float(Eachse.steps_pmm);
      break;
  }
  setNextPrePos(&newPos);
  setPrePos(&newPos);
}
void setNextPrePos(comParam* newPos) {
  if (newPos->useX) {
    //Serial.print("set npp x ");
    //Serial.print(newPos->X);
    nextPrePos.Xp = newPos->X;
    nextPrePos.Xs = nextPrePos.Xp * float(Xachse.steps_pmm);
  }
  if (newPos->useY) {
    //Serial.print("set npp y ");
    //Serial.print(newPos->Y);
    nextPrePos.Yp = newPos->Y;
    nextPrePos.Ys = nextPrePos.Yp * float(Yachse.steps_pmm);
  }
  if (newPos->useZ) {
    //Serial.print("set npp z ");
    //Serial.print(newPos->Z);
    nextPrePos.Zp = newPos->Z;
    nextPrePos.Zs = nextPrePos.Zp * float(Zachse.steps_pmm);
  }
  if (newPos->useE) {
    nextPrePos.Ep = newPos->E;
    nextPrePos.Es = nextPrePos.Ep * float(Eachse.steps_pmm);
  }
  if (newPos->useF)
    nextPrePos.Speed = newPos->F;
  //Serial.println();
}
void setPrePos(comParam* newPos) {
  if (newPos->useX) {
    //Serial.print("set pp x ");
    //Serial.print(newPos->X);
    prePos.Xp = newPos->X;
    prePos.Xs = prePos.Xp * float(Xachse.steps_pmm);
  }
  if (newPos->useY) {
    //Serial.print("set pp y ");
    //Serial.print(newPos->Y);
    prePos.Yp = newPos->Y;
    prePos.Ys = prePos.Yp * float(Yachse.steps_pmm);
  }
  if (newPos->useZ) {
    //Serial.print("set pp z ");
    //Serial.print(newPos->Z);
    prePos.Zp = newPos->Z;
    prePos.Zs = prePos.Zp * float(Zachse.steps_pmm);
  }
  if (newPos->useE) {
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
    StopAchse(mot->achse);
    mot->ESstate = true;
    Serial.print("touch es ");
    Serial.println(mot->achse);
  }
  if (digitalRead(ES) == LOW)
    mot->ESstate = false;
}
int checkSerial() {
  if (SR_CheckForLine() == 1) {
    processComandLine(reciveBuf);
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
ISR (TIMER1_OVF_vect) { // Timer1 ISR
  digitalWrite(22, HIGH);
  stepParam nextStep;
  comParam nextCommand;
  if (cb_pop_front(&cbSteps, &nextStep) == -1) {
    TCNT1 = T_MAX;
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
    TCNT1 = nextStep.ticks;
    //TODO add set prescalar heare
  }
  digitalWrite(22, LOW);
}
void addCommand(comParam* newCommand) {
  stepParam newStep;
  cb_push_back(&cbCommand, newCommand);
  newStep.achse = C;
  newStep.ticks = 0;
  newStep.preScale = 1;
  cb_push_back(&cbSteps, &newStep);
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
      Serial.println("do G92");
      setRealPose(newCommand);
      break;
    case M104:
      Serial.println("do M104");
      break;
    case M140:
      Serial.println("do M140");
      break;
    case M109:
      Serial.println("do M109");
      break;
    case M190:
      Serial.println("do M190");
      break;
    default:
      Serial.println("unknown Command");
  }
}
void startTimer(int prescale = 1) {
  switch (prescale) {
    case 0:
      resetBit(&TCCR1B, (1 << CS10));
      resetBit(&TCCR1B, (1 << CS11));
      resetBit(&TCCR1B, (1 << CS12));
      break;
    case 1:
      setBit(&TCCR1B, (1 << CS10));
      resetBit(&TCCR1B, (1 << CS11));
      resetBit(&TCCR1B, (1 << CS12));
      break;
    case 8:
      resetBit(&TCCR1B, (1 << CS10));
      setBit(&TCCR1B, (1 << CS11));
      resetBit(&TCCR1B, (1 << CS12));
      break;
    case 64:
      setBit(&TCCR1B, (1 << CS10));
      setBit(&TCCR1B, (1 << CS11));
      resetBit(&TCCR1B, (1 << CS12));
      break;
    case 256:
      resetBit(&TCCR1B, (1 << CS10));
      resetBit(&TCCR1B, (1 << CS11));
      setBit(&TCCR1B, (1 << CS12));
      break;
    case 1024:
      setBit(&TCCR1B, (1 << CS10));
      resetBit(&TCCR1B, (1 << CS11));
      setBit(&TCCR1B, (1 << CS12));
      break;
    default:
      Serial.println("unknown prescalar");
      break;
  }
  setBit(&TIMSK1, (1 << TOIE1));
}
int SD_OpenFile(File *file , const char* fileName) {
  Serial.print("open file ");
  Serial.println(fileName);
  if (file)
    file->close();
  if (!SD.exists(fileName)) {
    Serial.println("ERROR file dose not exist");
    return -1;
  }
  *file = SD.open(fileName);
  if (!file) {
    Serial.println("ERROR file cant be open");
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
int SD_removeFile(File &myFile, const char* fileName) {
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
eComandName ComandParser(char* command_line) {
  //look for a G-Code Comand in Line
  char wort[10];
  char* command_end = strchr(command_line, ' ');
  if (command_end == 0)
    return FAIL;
  int command_length = command_end - command_line;
  strncpy(wort, command_line, command_length);
  wort[command_length] = '\0';
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
int cb_push_back(circular_buffer *cb, const void *item) {
  while (cb->count == cb->capacity) {
    doStdTasks();
  }
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
  //regulate temperature
  //status aupdates
}
void setBit(volatile uint8_t* B, unsigned char b) {
  //set a bit in a Byte
  *B |= b;
}
void resetBit(volatile uint8_t* B, unsigned char b) {
  //reset a bit in a Byte
  *B &= ~b;
}

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

typedef enum {
  GCodeStart,
  GCodeRun,
  GCodePause,
  GCodeStop,
  GCodeCreate,
  GCodeStartHome,
  GCodeRunHome,
  GCodeStopHome
} eGCodeState_t;
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
  M190//set bed temperatur
};

typedef struct StepMotorBig {
  //Settings
  int pinENA;
  int pinDIR;
  int pinPUL;
  int pinNull;
  int pinPlus;
  //processing
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
  float Yp = 0;
  float Zp = 0;
  float Ep = 0;
  float Speed = 0;//mm/s
} MovePos;

StepMotorBig Xachse;
StepMotorBig Yachse;
StepMotorBig Zachse;
StepMotorBig Eachse;
sComand lComandList[8] = {
  {G1, "G1"},
  {G28, "G28"},
  {G92, "G92"},
  {M104, "M104"},
  {M140, "M140"},
  {M109, "M109"},
  {M190, "M190"}
};
circular_buffer cbSteps;
circular_buffer cbCommand;

eGCodeState_t state;

File GFile;
File HomeFile;
comParam nextMove;
MovePos prePos;
MovePos nextPrePos;

//settings/Parameter
bool useES = true;

void setup() {
  // put your setup code here, to run once:
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

  Serial.begin(9600);
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
        if (SD_OpenFile(&GFile , "CIRCLE2.TXT") == 0)
          setState(GCodeRun);
        break;
      case GCodeRun:
        if (preRunPointer(&GFile) == -1 && cbSteps.count == 0)
          setState(GCodeStop);
        break;
      case GCodePause:
        break;
      case GCodeStop:
        SD_CloseFile(&GFile);
        break;
      case GCodeCreate:
        break;
      case GCodeStartHome:
        if (SD_OpenFile(&HomeFile , "home.txt") == 0){
          setState(GCodeRunHome);
          useES = true;
        }
        break;
      case GCodeRunHome:
        if (preRunPointer(&HomeFile) == -1)
          setState(GCodeStopHome);
          break;
        break;
      case GCodeStopHome:
        SD_CloseFile(&HomeFile);
        setState(GCodeRun);
        useES = false;
        break;
      default:
        Serial.println("unknown State");
    }
    calculateSteps();
    
  }
}

void calculateSteps(){
  fillSimpeSteps(&nextPrePos.Xp,&prePos.Xp,X);
  fillSimpeSteps(&nextPrePos.Yp,&prePos.Yp,Y);
  fillSimpeSteps(&nextPrePos.Zp,&prePos.Zp,Z);
}
void fillSimpeSteps(float *sollStep,float *istStep,eAchse achse){
  stepParam newStep;
  newStep.achse = achse;
  newStep.ticks = T_MAX;
  if(*sollStep > *istStep){
    newStep.dir = 1;
    while(*sollStep > *istStep){
       cb_push_back(&cbSteps, &newStep);
      *istStep += float(1/50);
    }
  }else if(*sollStep < *istStep){
    newStep.dir = 0;
    while(*sollStep < *istStep){
      cb_push_back(&cbSteps, &newStep);
      *istStep -= float(1/50);
    }
  }
}


int preRunPointer(File* gFile) {
  comParam newCommand;
  char *newLine;
  newLine = SD_ReadLine(gFile);
  if (newLine[0] == '@')
    return -1;
  //Serial.println(newLine);
  LineParser(newLine, &newCommand);
  switch(newCommand.com){
    case G1:
      setNextPrePos(&newCommand);
      break;
    case G28:
      setState(GCodeStartHome);
      break;
    case G92:
      setPrePos(&newCommand);
    case M104:
    case M140:
    case M109:
    case M190:
      addCommand(&newCommand);
      preRunPointer(gFile);
      break;
    case FAIL:
    default:
      preRunPointer(gFile);
  }
}
void setNextPrePos(comParam* newPos){
  if (newPos->useX)
    nextPrePos.Xp = newPos->X;
  if (newPos->useY)
    nextPrePos.Yp = newPos->Y;
  if (newPos->useZ)
    nextPrePos.Zp = newPos->Z;
  if (newPos->useE)
    nextPrePos.Ep = newPos->E;
  if (newPos->useF)
    nextPrePos.Speed = newPos->F;
}
void setPrePos(comParam* newPos){
  if (newPos->useX)
    prePos.Xp = newPos->X;
  if (newPos->useY)
    prePos.Yp = newPos->Y;
  if (newPos->useZ)
    prePos.Zp = newPos->Z;
  if (newPos->useE)
    prePos.Ep = newPos->E;
}
void setRealPose(comParam* newPos) {
  if (newPos->useX)
    Xachse.act_step = newPos->X * Xachse.steps_pmm;
  if (newPos->useY)
    Yachse.act_step = newPos->Y * Yachse.steps_pmm;
  if (newPos->useZ)
    Zachse.act_step = newPos->Z * Zachse.steps_pmm;
  if (newPos->useE)
    Eachse.act_step = newPos->E * Eachse.steps_pmm;
}
void setState(eGCodeState_t newState) {
  state = newState;
  Serial.print("newState ");
  Serial.println(state);
}
eGCodeState_t getState() {
  return state;
}
void checkEndswitches(){
  //check if in home
  if(digitalRead(Xachse.pinNull)==HIGH&&
  digitalRead(Yachse.pinNull)==HIGH&&
  digitalRead(Zachse.pinPlus)==HIGH){
    if(getState() == GCodeRunHome)
      Serial.println("is in Home");
      setState(GCodeStopHome);
  }
  comParam newPos;
  //prePos;
  //nextPrePos;
  handleES(&Xachse,&prePos.Xp,&nextPrePos.Xp);
  handleES(&Yachse,&prePos.Yp,&nextPrePos.Yp);
  handleES(&Zachse,&prePos.Zp,&nextPrePos.Zp);
}
void handleES(StepMotorBig *mot,float *preP,float *nextPreP){
  if(digitalRead(mot->pinNull)==HIGH){
    float pos = float(mot->act_step) / float(mot->steps_pmm);
    if(*preP<pos){
      *preP=pos;
      }
    if(*nextPreP<pos){
      *nextPreP=pos;
      }
    
  }
  if(digitalRead(mot->pinPlus)==HIGH){
    float pos = float(mot->act_step) / float(mot->steps_pmm);
    if(*preP>pos){*preP=pos;}
    if(*nextPreP>pos){*nextPreP=pos;}
  }
}


void checkSerial() {
  char caLine[2];
  comParam newCommand;
  if (Serial.available()) {
    Serial.print("get serial ");
    Serial.readBytes(caLine, 2);
    Serial.println(caLine[0]);
    if(caLine[0]=='M'){
        digitalWrite(Xachse.pinENA, LOW);
        digitalWrite(Yachse.pinENA, LOW);
        digitalWrite(Zachse.pinENA, LOW);
        digitalWrite(Eachse.pinENA, LOW);
        Serial.println("get M");
    }else if(caLine[0]=='m'){
        digitalWrite(Xachse.pinENA, HIGH);
        digitalWrite(Yachse.pinENA, HIGH);
        digitalWrite(Zachse.pinENA, HIGH);
        digitalWrite(Eachse.pinENA, HIGH);
        Serial.println("get m");
    }else if(caLine[0]=='a'){
        Serial.println("get a");
        setState(GCodeStartHome);
    }else if(caLine[0]=='A'){
        Serial.println("get A");
        setState(GCodeStart);
    }else if(caLine[0]=='S'){
        startTimer(PRESCALE);
        Serial.println("get S");
    }else if(caLine[0]=='s'){
        startTimer(0);
        Serial.println("get s");
    }else if(caLine[0]=='B'){
        Serial.println("get B");
        File root = SD.open("/");
        SD_printDirectory(GFile, root, 0);
    }else if(caLine[0]=='b'){
        Serial.println("get b");
        Serial.print("npP_x:");
        Serial.print(nextPrePos.Xp);
        Serial.print(" pP_x:");
        Serial.println(prePos.Xp);
        Serial.print("npP_y:");
        Serial.print(nextPrePos.Yp);
        Serial.print(" pP_y:");
        Serial.println(prePos.Yp);
        Serial.print("npP_z:");
        Serial.print(nextPrePos.Zp);
        Serial.print(" pP_z:");
        Serial.println(prePos.Zp);
    }else{
        checkSerialAddSteps(caLine[0]);
    }
  }
}
void checkSerialAddSteps(char caLine){
    if(caLine == 'x'){
      nextPrePos.Xp -= 10;
    }else if(caLine == 'X'){
      nextPrePos.Xp += 10;
    }else if(caLine == 'y'){
      nextPrePos.Yp -= 10;
    }else if(caLine == 'Y'){
      nextPrePos.Yp += 10;
    }else if(caLine == 'z'){
      nextPrePos.Zp -= 10;
    }else if(caLine == 'Z'){
      nextPrePos.Zp += 10;
    }else{
        Serial.print("unknown kommand ");
        Serial.println(caLine);
    }
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
  }
  digitalWrite(22, LOW);
}
void addCommand(comParam* newCommand) {
  stepParam newStep;
  cb_push_back(&cbCommand, newCommand);
  newStep.achse = C;
  newStep.ticks = 0;
  cb_push_back(&cbSteps, &newStep);
}
void performStep(StepMotorBig *mot, bool dir) {
  if (dir && useES && mot->pinPlus != 0){
    if(digitalRead(mot->pinPlus)==HIGH){
      return;
    }
  }else if (!dir && useES && mot->pinNull != 0){
    if(digitalRead(mot->pinNull)==HIGH){
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
  switch(newCommand->com){
    case G92:
      Serial.println("do G92");
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

  newParam->com = ComandParser(command_line);
  if (newParam->com == FAIL)
    return -1;

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
        break;
      case 'Y':
        wort++;
        newParam->Y = atof(wort);
        newParam->useY = true;
        break;
      case 'Z':
        wort++;
        newParam->Z = atof(wort);
        newParam->useZ = true;
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
  while(cb->count == cb->capacity) {
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
void doStdTasks(){
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

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
  //processing
  int steps_pmm;//steps pro mm (constant für x y z)
  long act_step;//actueller position der achse in step
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

StepMotorBig Xachse;
StepMotorBig Yachse;
StepMotorBig Zachse;
StepMotorBig Eachse;

sComand lComandList[8] = {
  {G1, "G1"},
  {G9, "G9"},
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

void setup() {
  // put your setup code here, to run once:
  Xachse.act_step = 0;
  Xachse.steps_pmm = 50;
  Xachse.pinENA = 37;//PC0
  Xachse.pinDIR = 39;//PG2
  Xachse.pinPUL = 41;//PG0
  pinMode(Xachse.pinENA, OUTPUT);
  pinMode(Xachse.pinDIR, OUTPUT);
  pinMode(Xachse.pinPUL, OUTPUT);

  Yachse.act_step = 0;
  Yachse.steps_pmm = 50;
  Yachse.pinENA = 43;
  Yachse.pinDIR = 45;
  Yachse.pinPUL = 47;
  pinMode(Yachse.pinENA, OUTPUT);
  pinMode(Yachse.pinDIR, OUTPUT);
  pinMode(Yachse.pinPUL, OUTPUT);

  Zachse.act_step = 0;
  Zachse.steps_pmm = 50;
  Zachse.pinENA = 25;
  Zachse.pinDIR = 27;
  Zachse.pinPUL = 29;
  pinMode(Zachse.pinENA, OUTPUT);
  pinMode(Zachse.pinDIR, OUTPUT);
  pinMode(Zachse.pinPUL, OUTPUT);

  Eachse.act_step = 0;
  Eachse.steps_pmm = 35;
  Eachse.pinENA = 31;
  Eachse.pinDIR = 33;
  Eachse.pinPUL = 35;
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
    checkSerial();

    switch (state) {
      case GCodeStart:
        if (SD_OpenFile(&GFile , "CIRCLE.TXT") == 0)
          setState(GCodeRun);
        break;
      case GCodeRun:
        if (readFromSD(&GFile) == -1)
          setState(GCodeStop);
        //calculate steps until finisched last point
        break;
      case GCodePause:
        break;
      case GCodeStop:
        SD_CloseFile(&GFile);
        break;
      case GCodeCreate:
        break;
      case GCodeStartHome:
        if (SD_OpenFile(&HomeFile , "home.txt") == 0)
          setState(GCodeRunHome);
        break;
      case GCodeRunHome:
        if (readFromSD(&HomeFile) == -1)
          setState(GCodeStopHome);
        else
          //calculate steps until finisched last point
          break;
        break;
      case GCodeStopHome:
        SD_CloseFile(&HomeFile);
        setState(GCodeRun);
        break;
      default:
        Serial.println("unknown State");
    }
  }
}

int readFromSD(File* gFile) {
  comParam newCommand;
  char *newLine;
  newLine = SD_ReadLine(gFile);
  if (newLine[0] == '@')
    return -1;
  LineParser(newLine, &newCommand);
  if (newCommand.com == FAIL) {
    readFromSD(gFile);
  } else if (newCommand.com != G1) {
    addCommand(&newCommand);
    readFromSD(gFile);
  } else {
    nextMove = newCommand;// nur die in use sollten übertragen werden
  }
}
void setState(eGCodeState_t newState) {
  state = newState;
  Serial.print("newState ");
  Serial.println(state);
}
void checkSerial() {
  char caLine[2];
  stepParam newStep;
  comParam newCommand;
  if (Serial.available()) {
    Serial.println("get serial");
    Serial.readBytes(caLine, 2);

    if (caLine[0] >= 'A' and caLine[0] <= 'Z')
      newStep.dir = 1;
    else
      newStep.dir = 0;

    switch (caLine[0]) {
      case 'x':
      case 'X':
        Serial.println("get x");
        newStep.achse = X;
        break;
      case 'y':
      case 'Y':
        Serial.println("get y");
        newStep.achse = Y;
        break;
      case 'z':
      case 'Z':
        Serial.println("get z");
        newStep.achse = Z;
        break;
      case 'a':
        Serial.println("get a");
        newCommand.com = G92;
        newCommand.X = 10;
        addCommand(&newCommand);
        return;
        break;
      case 'A':
        setState(GCodeStart);
        return;
        break;
      case 'B':
        File root = SD.open("/");
        SD_printDirectory(GFile, root, 0);
        return;
        break;
      case 'M':
        digitalWrite(Xachse.pinENA, LOW);
        digitalWrite(Yachse.pinENA, LOW);
        digitalWrite(Zachse.pinENA, LOW);
        digitalWrite(Eachse.pinENA, LOW);
        return;
        break;
      case 'm':
        digitalWrite(Xachse.pinENA, HIGH);
        digitalWrite(Yachse.pinENA, HIGH);
        digitalWrite(Zachse.pinENA, HIGH);
        digitalWrite(Eachse.pinENA, HIGH);
        return;
        break;

      default:
        Serial.print("unknown kommand ");
        Serial.println(caLine[0]);
        return;
    }
    newStep.ticks = T_MAX;
    for (int i = 0; i < 500; i++) {
      while (cb_push_back(&cbSteps, &newStep) == -1) {
        delay(1);
      }
    }
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
  while (cb_push_back(&cbCommand, newCommand) == -1) {
    delay(1);
  }
  newStep.achse = C;
  newStep.ticks = 0;
  while (cb_push_back(&cbSteps, &newStep) == -1) {
    delay(1);
  }
}
void performStep(StepMotorBig *mot, bool dir) {
  digitalWrite(mot->pinPUL, LOW);
  digitalWrite(mot->pinDIR, dir);
  if (dir)
    mot->act_step++;
  else
    mot->act_step--;
  digitalWrite(mot->pinPUL, HIGH);
}
void performCommand(comParam* newCommand) {
  Serial.print("performCommand com ");
  Serial.println(newCommand->com);
}
void startTimer(int prescale = 1) {
  switch (prescale) {
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
  if (cb->count == cb->capacity) {
    return -1;
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
void setBit(volatile uint8_t* B, unsigned char b) {
  //set a bit in a Byte
  *B |= b;
}
void resetBit(volatile uint8_t* B, unsigned char b) {
  //reset a bit in a Byte
  *B &= ~b;
}

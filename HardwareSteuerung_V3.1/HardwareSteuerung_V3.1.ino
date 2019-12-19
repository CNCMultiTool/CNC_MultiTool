#include <PID_v1.h>

struct telegram{
  char comand;
  float value[4];
  char check_byte;
};

union {
  telegram tel;
  char buf[18];
} Buf;

char last_send_buf[18];

struct StepMotorMedi {
  //Settings
  int pin1;
  int pin2;
  int pin3;
  int pin4;
  int pinNull;
  int pinPos;
  double steps_pmm;//steps pro mm

  int motor_step;//0 - 3 gerade anliegender stepper schritt
  //processing
  long act_step;//actueller position der achse in step
  long soll_step;//
  double act_posi;//actuelle position in mm
  double soll_posi;//soll position in mm
  unsigned long time_pstep;
  unsigned long time_next_step;  
};

struct StepMotorBig {
  //Settings
  int pinENA;
  int pinDIR;
  int pinPUL;
  int pinNull;
  int pinPositiv;
  float SwitchID;
  int steps_pmm;//steps pro mm (constant für x y z)

  //processing
  long act_step;//actueller position der achse in step
  long soll_step;//
  double act_posi;//actuelle position in mm
  double soll_posi;//soll position in mm
  unsigned long time_pstep;
  unsigned long time_next_step;  
};

struct StepMotorMedi Wachse;
struct StepMotorBig Xachse;
struct StepMotorBig Yachse;
struct StepMotorBig Zachse;

bool msg_available;

double Speed = 2; //mm pro minute
double dist;
double ges_time;

unsigned long time_now = micros();
unsigned long cycle_time = micros();

bool sendPose = false;

double U1;
double U2;
double R1 = 110;//messwiederstand in Ohm
double R2; //berechnete Ohm des PT100 
double R0 = 100;//R0 des PT1000
double A = 3.9083*pow(10,-3); //constanten für pt100
double B = -5.775*pow(10,-7);
double C = -4.183*pow(10,-4);
double T;
double soll_T;

int temprelai = 27;

//PID
double Output;

double KP = 35;
double KI = 0.2;
double KD = 750;

//PID myPID(&soll_T, &Output, &T, KP, KI, KD,P_ON_M, REVERSE);
PID myPID(&soll_T, &Output, &T, KP, KI, KD,P_ON_E, REVERSE);
unsigned long WindowSize = 5000;
unsigned long windowStartTime;
unsigned long PID_time = millis();

int debug = 0;

void setup() {
  Serial.begin(9600,SERIAL_8N2);//9600
  
  /*TODO
   * EndPins
   */

  Xachse.soll_posi = 0;
  Xachse.act_posi = 0;
  Xachse.soll_step = 0;
  Xachse.act_step = 0;
  Xachse.time_next_step = 0;
  Xachse.time_pstep = 0;
  Xachse.steps_pmm = 100;
  Xachse.pinENA = 37;
  Xachse.pinDIR = 39;
  Xachse.pinPUL = 41;
  Xachse.pinPositiv = 50;
  Xachse.pinNull = 52;
  Xachse.SwitchID = 0;

  Yachse.soll_posi = 0;
  Yachse.act_posi = 0;
  Yachse.soll_step = 0;
  Yachse.act_step = 0;
  Yachse.time_next_step = 0;
  Yachse.time_pstep = 0;
  Yachse.steps_pmm = 100;
  Yachse.pinENA = 43;
  Yachse.pinDIR = 45;
  Yachse.pinPUL = 47;
  Yachse.pinPositiv = 46;
  Yachse.pinNull = 48;
  Yachse.SwitchID = 0;

  Zachse.soll_posi = 0;
  Zachse.act_posi = 0;
  Zachse.soll_step = 0;
  Zachse.act_step = 0;
  Zachse.time_next_step = 0;
  Zachse.time_pstep = 0;
  Zachse.steps_pmm = 100;
  Zachse.pinENA = 49;
  Zachse.pinDIR = 51;
  Zachse.pinPUL = 53;
  Zachse.pinPositiv = 42;
  Zachse.pinNull = 44;
  Zachse.SwitchID = 0;
  
  Wachse.soll_posi = 0;
  Wachse.act_posi = 0;
  Wachse.soll_step = 0;
  Wachse.act_step = 0;
  Wachse.time_next_step = 0;
  Wachse.time_pstep = 0;
  Wachse.motor_step = 0;
  Wachse.steps_pmm = 20;
  Wachse.pin1 = 29;
  Wachse.pin2 = 31;
  Wachse.pin3 = 33;
  Wachse.pin4 = 35;
    
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
  
  pinMode(Wachse.pin1,OUTPUT);
  pinMode(Wachse.pin2,OUTPUT);
  pinMode(Wachse.pin3,OUTPUT);
  pinMode(Wachse.pin4,OUTPUT);

  pinMode(23,INPUT_PULLUP);
  pinMode(temprelai,OUTPUT);

  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetSampleTime(WindowSize);
  myPID.SetMode(AUTOMATIC);
  
}

void loop() {
  time_now = micros();
  //if(digitalRead(23)==LOW){
  //  digitalWrite(temprelai,HIGH);
  //  digitalWrite(temprelai,LOW);
  //  act_equal_soll();
  //}

  checkEndswitches();
  
  if(msg_available){
    recive_msg();
  }
  
  TempControle();

  //motors drive
  treiberBig(Xachse);
  treiberBig(Yachse);
  treiberBig(Zachse);
  treiberMedi(Wachse);
  
  if(Xachse.soll_step == Xachse.act_step){
    if(Yachse.soll_step == Yachse.act_step){
      if(Zachse.soll_step == Zachse.act_step){
        if(Wachse.soll_step == Wachse.act_step){
          if(sendPose == false){
            act_steps_to_act_posi();
            sendactpos();
            sendPose = true;
          }
        }
      }
    }
  }

  //something to do everi 1000 ms
  if(cycle_time<time_now){
    cycle_time = time_now + 1000000;
    sendsetting();
    //sendtimes();
    //sendactpos();
    //sendsollpos();
    //sendsollstep();
    //sendactstep();
  }
}

float checkEndswitches(){
  float newXSwitchID = checkEndswitche(Xachse);
  float newYSwitchID = checkEndswitche(Yachse);
  float newZSwitchID = checkEndswitche(Zachse);
  if(Xachse.SwitchID != newXSwitchID || Yachse.SwitchID != newYSwitchID || Zachse.SwitchID != newZSwitchID){
    send_variabelTestTommand('e',newXSwitchID,newYSwitchID,newZSwitchID,0);
  }
  Xachse.SwitchID = newXSwitchID;
  Yachse.SwitchID = newYSwitchID;
  Zachse.SwitchID = newZSwitchID;
}
  
  
float checkEndswitche(struct StepMotorBig &StepM){
  float switchID;
  if(digitalRead(StepM.pinNull)==HIGH&& StepM.soll_step <= StepM.act_step){
    StepM.soll_step = StepM.act_step;
    switchID = -1;
  }else if(digitalRead(StepM.pinPositiv)==HIGH&& StepM.soll_step >= StepM.act_step){
    StepM.soll_step = StepM.act_step;
    switchID = 1;
  }else{
    switchID = 0;
  }
  return(switchID);
}

void TempControle(){
  PID_time = millis();
  U1 = analogRead(0);
  U2 = 1024-U1;
  R2 = (R1*U2)/U1;
  T = (-A*R0+sqrt(pow(A*R0,2)-4*B*R0*(R0-R2)))/(2*B*R0);

  if(soll_T >= 0){
    myPID.Compute();
    if(PID_time > (windowStartTime + WindowSize)){
      windowStartTime = PID_time;
    } 
      
    if(Output > (PID_time - windowStartTime)){
      digitalWrite(temprelai, HIGH);
    }else{
      digitalWrite(temprelai, LOW);
    }
    debug = Output;
  }

  if(soll_T == -1){
    digitalWrite(temprelai, LOW);
  }
  if(soll_T == -2){
    digitalWrite(temprelai, HIGH);
  }
}

void recive_msg(){
  msg_available = false;
  switch(Buf.tel.comand){
    case 'm'://move
      Xachse.soll_posi = Buf.tel.value[0];
      Yachse.soll_posi = Buf.tel.value[1];
      Zachse.soll_posi = Buf.tel.value[2];
      Wachse.soll_posi = Buf.tel.value[3];
      getMoveParams();
      sendPose = false;
      break;
    case 'n'://set new pose
      Xachse.act_posi = Buf.tel.value[0];
      Yachse.act_posi = Buf.tel.value[1];
      Zachse.act_posi = Buf.tel.value[2];
      Wachse.act_posi = Buf.tel.value[3];
      setPose();
      sendactpos();
      break;
    case 'p'://send pose
      sendactpos();
      break;
    case 's': //set speed
      Speed = Buf.tel.value[0];
      sendsetting();
      break;
    case 't': //set temperatur
      soll_T = Buf.tel.value[0];
      sendsetting();
      break;
    case 'f'://set feed rate of filament
      Wachse.steps_pmm = Buf.tel.value[0];
      sendsetting();
      break;
    case 'c'://send setting
      sendsetting();
      break;
    case 'b'://send stop
      act_equal_soll();
      break;
    case 'r'://PID parameter
      KP = Buf.tel.value[0];
      KI = Buf.tel.value[1];
      KD = Buf.tel.value[2];
      if(Buf.tel.value[3]>0.5){
        myPID.SetTunings(KP, KI, KD,P_ON_E);
      }else{
        myPID.SetTunings(KP, KI, KD,P_ON_M);
      }
    case 'N':
      strncpy(Buf.buf,last_send_buf , 18);
      send_tel();
      break;
    default:
      sendRepeatRequest();
      break;
  }
}

//strops the movement (set soll pos equal act pos)
void act_equal_soll(){
  Xachse.soll_step = Xachse.act_step;
  Yachse.soll_step = Yachse.act_step;
  Zachse.soll_step = Zachse.act_step;
  Wachse.soll_step = Wachse.act_step;
  act_steps_to_act_posi();
  Xachse.soll_posi = Xachse.act_posi;
  Yachse.soll_posi = Yachse.act_posi;
  Zachse.soll_posi = Zachse.act_posi;
  Wachse.soll_posi = Wachse.act_posi;
}
void act_steps_to_act_posi(){
  Xachse.act_posi = (double)Xachse.act_step/Xachse.steps_pmm;
  Yachse.act_posi = (double)Yachse.act_step/Yachse.steps_pmm;
  Zachse.act_posi = (double)Zachse.act_step/Zachse.steps_pmm;
  Wachse.act_posi = (double)Wachse.act_step/Wachse.steps_pmm;
}
void act_posi_to_act_steps(){
  Xachse.act_step = (double)Xachse.act_posi*Xachse.steps_pmm;
  Yachse.act_step = (double)Yachse.act_posi*Yachse.steps_pmm;
  Zachse.act_step = (double)Zachse.act_posi*Zachse.steps_pmm;
  Wachse.act_step = (double)Wachse.act_posi*Wachse.steps_pmm;
}
void setPose(){
  Xachse.soll_posi = Xachse.act_posi;
  Yachse.soll_posi = Yachse.act_posi;
  Zachse.soll_posi = Zachse.act_posi;
  Wachse.soll_posi = Wachse.act_posi;
  act_posi_to_act_steps();
  Xachse.soll_step = Xachse.act_step;
  Yachse.soll_step = Yachse.act_step;
  Zachse.soll_step = Zachse.act_step;
  Wachse.soll_step = Wachse.act_step;
}

//berechnet die  parameter für eine bewegung für eine achse
void getMoveParams(){
  dist = sqrt(pow(Xachse.soll_posi-Xachse.act_posi,2)+pow(Yachse.soll_posi-Yachse.act_posi,2)+pow(Zachse.soll_posi-Zachse.act_posi,2)); //gesamtdistans
  if(dist == 0){
    dist = abs(Wachse.soll_posi-Wachse.act_posi)*10;
  }
  ges_time = (dist/Speed)*1000000;
  
  BigM_move_params(Xachse);
  BigM_move_params(Yachse);
  BigM_move_params(Zachse);
  MediM_move_params(Wachse);  
}

void BigM_move_params(struct StepMotorBig &StepM){
  StepM.soll_step = StepM.soll_posi*StepM.steps_pmm;
  StepM.time_pstep = ges_time/abs(StepM.soll_step-StepM.act_step);
  StepM.time_next_step = micros();
}

void MediM_move_params(struct StepMotorMedi &StepM){
  StepM.soll_step = StepM.soll_posi*StepM.steps_pmm;
  StepM.time_pstep = ges_time/abs(StepM.soll_step-StepM.act_step);
  StepM.time_next_step = micros();
}

///////////////////////////////////fürt einen schrit aus und gibt den actuellen an////////////////////////////7
void treiberBig(struct StepMotorBig &StepM){
  if(time_now >= StepM.time_next_step){
    if(StepM.soll_step>StepM.act_step){
      //one stapp forward
      digitalWrite(StepM.pinENA,LOW);
      digitalWrite(StepM.pinDIR,HIGH);
      digitalWrite(StepM.pinPUL,HIGH);
      digitalWrite(StepM.pinPUL,LOW);
      StepM.time_next_step += StepM.time_pstep;
      StepM.act_step++;
    }else if(StepM.soll_step<StepM.act_step){
      //one stepp back
      digitalWrite(StepM.pinENA,LOW);
      digitalWrite(StepM.pinDIR,LOW);
      digitalWrite(StepM.pinPUL,HIGH);
      digitalWrite(StepM.pinPUL,LOW);
      StepM.time_next_step += StepM.time_pstep;
      StepM.act_step--;
    }else{
      //turn motor off
      digitalWrite(StepM.pinENA,HIGH);
    }
  }
}

void treiberMedi(struct StepMotorMedi &StepM){
  if(time_now >= StepM.time_next_step){
    if(StepM.soll_step>StepM.act_step){
      //one stapp forward
      StepMedi(StepM,1);
      StepM.time_next_step += StepM.time_pstep;
      StepM.act_step++;
    }else if(StepM.soll_step<StepM.act_step){
      //one stepp back
      StepMedi(StepM,-1);
      StepM.time_next_step += StepM.time_pstep;
      StepM.act_step--;
    }else if(time_now >= StepM.time_next_step+1500000){
      //turn motor off
      digitalWrite(StepM.pin1, LOW);
      digitalWrite(StepM.pin2, LOW);
      digitalWrite(StepM.pin3, LOW);
      digitalWrite(StepM.pin4, LOW);
    }
  }
}

void StepMedi(struct StepMotorMedi &StepM,int direct){
  StepM.motor_step += direct;
  if (StepM.motor_step > 3){
    StepM.motor_step = 0;
  }else if (StepM.motor_step < 0){
    StepM.motor_step = 3;
  }
  switch (StepM.motor_step)
  {
    case 0:    // 1010
      digitalWrite(StepM.pin1, HIGH);
      digitalWrite(StepM.pin2, LOW);
      digitalWrite(StepM.pin3, HIGH);
      digitalWrite(StepM.pin4, LOW);
      break;
    case 1:    // 0110
      digitalWrite(StepM.pin1, LOW);
      digitalWrite(StepM.pin2, HIGH);
      digitalWrite(StepM.pin3, HIGH);
      digitalWrite(StepM.pin4, LOW);
      break;
    case 2:    //0101
      digitalWrite(StepM.pin1, LOW);
      digitalWrite(StepM.pin2, HIGH);
      digitalWrite(StepM.pin3, LOW);
      digitalWrite(StepM.pin4, HIGH);
      break;
    case 3:    //1001
      digitalWrite(StepM.pin1, HIGH);
      digitalWrite(StepM.pin2, LOW);
      digitalWrite(StepM.pin3, LOW);
      digitalWrite(StepM.pin4, HIGH);
      break;
  }
}

//Serial Send Funktions///////////////////////////////////////////////////////////////////
void sendsetting(){
  Buf.tel.comand = 'q';
  Buf.tel.value[0] = Output;//Speed;//curent speed  (now-heatStartTime)/1000;
  Buf.tel.value[1] = T;//temperatur
  Buf.tel.value[2] = soll_T;//Output;
  Buf.tel.value[3] = Wachse.steps_pmm;
  send_tel();
}

void sendtimes(){
  Buf.tel.comand = 'w';
  Buf.tel.value[0] = Xachse.time_pstep;
  Buf.tel.value[1] = Yachse.time_pstep;
  Buf.tel.value[2] = Zachse.time_pstep;
  Buf.tel.value[3] = Wachse.time_pstep;
  send_tel();
}

void sendactpos(){
  Buf.tel.comand = 'm';
  Buf.tel.value[0] = Xachse.act_posi;
  Buf.tel.value[1] = Yachse.act_posi;
  Buf.tel.value[2] = Zachse.act_posi;
  Buf.tel.value[3] = Wachse.act_posi;
  send_tel();
}

void sendsollpos(){
  Buf.tel.comand = 'j';
  Buf.tel.value[0] = Xachse.soll_posi;
  Buf.tel.value[1] = Yachse.soll_posi;
  Buf.tel.value[2] = Zachse.soll_posi;
  Buf.tel.value[3] = Wachse.soll_posi;
  send_tel();
}

void sendsollstep(){
  Buf.tel.comand = 'k';
  Buf.tel.value[0] = Xachse.soll_step;
  Buf.tel.value[1] = Yachse.soll_step;
  Buf.tel.value[2] = Zachse.soll_step;
  Buf.tel.value[3] = Wachse.soll_step;
  send_tel();
}

void sendactstep(){
  Buf.tel.comand = 'b';
  Buf.tel.value[0] = Xachse.act_step;
  Buf.tel.value[1] = Yachse.act_step;
  Buf.tel.value[2] = Zachse.act_step;
  Buf.tel.value[3] = Wachse.act_step;
  send_tel();
}

void sendRepeatRequest(){
  Buf.tel.comand = 'N';
  Buf.tel.value[0] = 0;
  Buf.tel.value[1] = 0;
  Buf.tel.value[2] = 0;
  Buf.tel.value[3] = 0;
  send_tel();
}

void send_variabelTestTommand(char C, float val1, float val2, float val3, float val4){
  Buf.tel.comand = C;
  Buf.tel.value[0] = val1;
  Buf.tel.value[1] = val2;
  Buf.tel.value[2] = val3;
  Buf.tel.value[3] = val4;
  send_tel();
}

void serialEvent(){
  char read_buf[1] = {'n'};
  if(Serial.available()){
    while(read_buf[0]!='S'){
      Serial.readBytes(read_buf,1);
    }
    Serial.readBytes(Buf.buf,18);
    if(Buf.buf[17] != calc_checkbyte()){
      sendRepeatRequest();
      msg_available = false;
    }else{
      msg_available = true;
    }
  }
}

void send_tel(){
  char send_buffer[19];
  Buf.buf[17] = calc_checkbyte();
  strncpy(last_send_buf, Buf.buf, 18);
  send_buffer[0] = 'S';
  for(int i=0;i<18;i++){
    send_buffer[i+1] = Buf.buf[i];
  }
  for(int i = 0;i<19;i++){
    Serial.write(send_buffer[i]);
  }
}

char calc_checkbyte(){
  char check_byte = 'S';
  for(int i = 0;i<17;i++){
    check_byte +=  Buf.buf[i];
  }
  return(check_byte);
}

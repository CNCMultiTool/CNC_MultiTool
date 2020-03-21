#include <PID_v1.h>

struct telegram{
  char space;
  char comand;
  float value[4];
  char check_byte;
};

union {
  telegram tel;
  char buf[19];
} Buf,SenBuf;

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
  
  //rampen
  //unsigned long ist_time_pstep;
  unsigned long step_div;
  unsigned long ramp_step; 
};
int acceleration_steps = 20;

struct StepMotorMedi Wachse;
struct StepMotorBig Xachse;
struct StepMotorBig Yachse;
struct StepMotorBig Zachse;

bool msg_available;

char lastSend[19];

double Speed = 50; //mm pro minute
double dist;
double ges_time;

unsigned long time_now = micros();
unsigned long cycle_time = micros();
unsigned long old_time_now = micros();

bool sendPose = true;

bool alavie;

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

bool wait_for_response = false;

void setup() {
  Serial.begin(115200,SERIAL_8E1);//9600
  char dummy[1];
  while(Serial.available())
  {
    Serial.readBytes(dummy,1);
  }
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
  Wachse.steps_pmm = 37;
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
<<<<<<< HEAD
<<<<<<< HEAD

  pinMode(22,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(28,OUTPUT);
=======
=======
>>>>>>> parent of 08a1822... functional fersion with bug
  
>>>>>>> parent of 08a1822... functional fersion with bug
}

void loop() {
  time_now = micros();
<<<<<<< HEAD
  if(time_now<old_time_now){
    digitalWrite(28,HIGH);
  }
//  if(digitalRead(23)==HIGH){
//    digitalWrite(22,LOW);
//    digitalWrite(24,LOW);
//    digitalWrite(26,LOW);
//    digitalWrite(28,LOW);
//  }

  if(msg_available){
    recive_msg();
  }

=======
  //if(digitalRead(23)==HIGH){
  //  digitalWrite(temprelai,HIGH);
  //}
  
  if(msg_available){
    recive_msg();
  }
<<<<<<< HEAD
>>>>>>> parent of 08a1822... functional fersion with bug
=======
>>>>>>> parent of 08a1822... functional fersion with bug
  checkEndswitches();
  TempControle();

  //motors drive
  treiberBig(Xachse);
  treiberBig(Yachse);
  treiberBig(Zachse);
  treiberMedi(Wachse);
<<<<<<< HEAD
<<<<<<< HEAD

=======
  
>>>>>>> parent of 08a1822... functional fersion with bug
=======
  
>>>>>>> parent of 08a1822... functional fersion with bug
  if(Xachse.soll_step == Xachse.act_step){
    if(Yachse.soll_step == Yachse.act_step){
      if(Zachse.soll_step == Zachse.act_step){
        if(Wachse.soll_step == Wachse.act_step){
          if(sendPose == false){
            act_steps_to_act_posi();
            sendconfirmpos();
            sendPose = true;
          }
        }
      }
    }
  }

<<<<<<< HEAD
<<<<<<< HEAD
  //timeout
  if(cycle_time < time_now && wait_for_response)
  {
    wait_for_response = false;
    serieltimeouthandler();
    digitalWrite(22,HIGH);
=======
=======
>>>>>>> parent of 08a1822... functional fersion with bug
  //something to do everi 1000 ms
  if(cycle_time<time_now){
    cycle_time = time_now + 1000000;
    sendsetting();
>>>>>>> parent of 08a1822... functional fersion with bug
  }
  old_time_now = time_now;
}

float checkEndswitches(){
  float newXSwitchID = checkEndswitch(Xachse);
  float newYSwitchID = checkEndswitch(Yachse);
  float newZSwitchID = checkEndswitch(Zachse);
  if(Xachse.SwitchID != newXSwitchID || Yachse.SwitchID != newYSwitchID || Zachse.SwitchID != newZSwitchID){
    Xachse.SwitchID = newXSwitchID;
    Yachse.SwitchID = newYSwitchID;
    Zachse.SwitchID = newZSwitchID;
    sendendswitch();
    sendactposition();
  }
}
float checkEndswitch(struct StepMotorBig &StepM){
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
    case 'i':
      sendsetting();
      sendconfirmpos();
      sendendswitch();
      break;
    case 'm'://move
      Xachse.soll_posi = Buf.tel.value[0];
      Yachse.soll_posi = Buf.tel.value[1];
      Zachse.soll_posi = Buf.tel.value[2];
      Wachse.soll_posi = Buf.tel.value[3];
      getMoveParams();
      sendPose = false;
      sendConfirmAnswer();
      break;
    case 'p'://set new pose
      Xachse.act_posi = Buf.tel.value[0];
      Yachse.act_posi = Buf.tel.value[1];
      Zachse.act_posi = Buf.tel.value[2];
      Wachse.act_posi = Buf.tel.value[3];
      setPose();
      sendactposition();
      break;
    case 't'://send pose
      sendactposition();
      break;
    case 's': //set speed
      Speed = Buf.tel.value[0];
      soll_T = Buf.tel.value[1];
      Wachse.steps_pmm = Buf.tel.value[2];
      sendsetting();
      break;
    case 'r'://send setting (request settings)
      sendsetting();
      if(alavie)
      {
        alavie = false;
        //digitalWrite(28,LOW);
      }
      else
      {
        alavie = true;
        //digitalWrite(28,HIGH);
      }
      break;
    case 'b'://send stop
      act_equal_soll();
      sendconfirmpos();
      break;
    case 'N'://repetrquest error detection
      Serial.write(lastSend,19);
      //digitalWrite(28,HIGH);
      //stop_responstimer();
      break;
    case 'a'://repetrquest error detection
      //stop_responstimer();
      break;
    default:
      //digitalWrite(24,HIGH);
      //stop_responstimer();
      break;
  }
}
void serieltimeouthandler(){
      //digitalWrite(22,HIGH);
      Serial.flush();
      Serial.end();
      Serial.begin(115200,SERIAL_8E1);//9600
      char dummy[1];
      while(Serial.available())
      {
        Serial.readBytes(dummy,1);
      }
      Serial.write(lastSend,19);
      //sendRepeatRequest();      
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
    dist = abs(Wachse.soll_posi-Wachse.act_posi)*15;
  }
  ges_time = (dist/Speed)*1000000;
  
  BigM_move_params(Xachse);
  BigM_move_params(Yachse);
  BigM_move_params(Zachse);
  MediM_move_params(Wachse);  
}
void BigM_move_params(struct StepMotorBig &StepM){
  StepM.soll_step = StepM.soll_posi*StepM.steps_pmm;
  StepM.step_div = abs(StepM.soll_step-StepM.act_step);
  StepM.time_pstep = ges_time/StepM.step_div;
  StepM.time_next_step = micros();
  StepM.ramp_step = 0;
}
void MediM_move_params(struct StepMotorMedi &StepM){
  StepM.soll_step = StepM.soll_posi*StepM.steps_pmm;
  StepM.time_pstep = ges_time/abs(StepM.soll_step-StepM.act_step);
  StepM.time_next_step = micros();
}

unsigned long calc_steptime(struct StepMotorBig &StepM){
  unsigned long curr_step = StepM.step_div-abs(StepM.soll_step-StepM.act_step);
  unsigned long ist_time_pstep = StepM.time_pstep;
  double soll_speed = (1/double(StepM.time_pstep)) * 100;
  double next_speed;
  int state = 0;
  if(StepM.step_div/2<curr_step)//abbremsen
  {
    state = 1;
    if(abs(StepM.step_div-curr_step) < acceleration_steps)
    {
      state = 2;
      next_speed = (soll_speed/double(acceleration_steps))*double((abs(StepM.step_div-curr_step)+1));
      ist_time_pstep = 1/(next_speed/100);
    }
  }
  else    //beschleunigung
  {
    if(StepM.ramp_step < acceleration_steps)
    {
      next_speed = (soll_speed/double(acceleration_steps))*double(acceleration_steps-(acceleration_steps-StepM.ramp_step)+1);
      ist_time_pstep = 1/(next_speed/100);
      StepM.ramp_step += 1;
    }
  }      
  
  if(ist_time_pstep>10000){
    ist_time_pstep=10000;
  }
  //send_debug(ist_time_pstep,next_speed,state,curr_step);
  return(ist_time_pstep);
}
///////////////////////////////////fürt einen schrit aus und gibt den actuellen an////////////////////////////7
void treiberBig(struct StepMotorBig &StepM){
  if(time_now >= StepM.time_next_step){
    if(StepM.soll_step>StepM.act_step){
      //one stapp forward
      digitalWrite(StepM.pinENA,LOW);
      digitalWrite(StepM.pinDIR,HIGH);
      digitalWrite(StepM.pinPUL,LOW);
      digitalWrite(StepM.pinPUL,HIGH);
      //StepM.time_next_step += StepM.time_pstep;
      StepM.time_next_step += calc_steptime(StepM);
      StepM.act_step++;
    }else if(StepM.soll_step<StepM.act_step){
      //one stepp back
      digitalWrite(StepM.pinENA,LOW);
      digitalWrite(StepM.pinDIR,LOW);
      digitalWrite(StepM.pinPUL,LOW);
      digitalWrite(StepM.pinPUL,HIGH);
      //StepM.time_next_step += StepM.time_pstep;
      StepM.time_next_step += calc_steptime(StepM);
      StepM.act_step--;
    }else if(time_now >= StepM.time_next_step+500000){
      //turn motor off
      digitalWrite(StepM.pinENA,HIGH);
    }else{
      //turn motor off
      digitalWrite(StepM.pinPUL,HIGH);
      digitalWrite(StepM.pinDIR,LOW);
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
void send_debug(float a,float b,float c,float d){
  send_variabelTestCommand('d',a,b,c,d);
  start_responstimer();
}

void sendendswitch(){
  send_variabelTestCommand('e',Xachse.SwitchID,Yachse.SwitchID,Zachse.SwitchID,0);
  start_responstimer();
}
void sendactposition(){
  send_variabelTestCommand('m',Xachse.act_posi,Yachse.act_posi,Zachse.act_posi,Wachse.act_posi);
}
void sendconfirmpos(){
  send_variabelTestCommand('c',Xachse.act_posi,Yachse.act_posi,Zachse.act_posi,Wachse.act_posi);
  start_responstimer();
}
void sendRepeatRequest(){
  send_variabelTestCommand('N',0,0,0,0);
  start_responstimer();
}
void sendConfirmAnswer(){
  send_variabelTestCommand('a',0,0,0,0);
}
void sendsetting(){
  send_variabelTestCommand('s',Speed,T,Wachse.steps_pmm,Output);
}
void send_variabelTestCommand(char C, float val1, float val2, float val3, float val4){
  SenBuf.tel.comand = C;
  SenBuf.tel.value[0] = val1;
  SenBuf.tel.value[1] = val2;
  SenBuf.tel.value[2] = val3;
  SenBuf.tel.value[3] = val4;
  send_tel();
}
void start_responstimer(){
  wait_for_response = true;
  cycle_time = time_now + 50000;
}
void stop_responstimer(){
  wait_for_response = false;
}
void serialEvent(){
  char bufS[1];
  char bufBig[18];
  bool wrong_telegram = false;
  stop_responstimer();
  Serial.readBytes(bufS,1);
  while(bufS[0] != 'S'){
    Serial.readBytes(bufS,1);
    //digitalWrite(28,HIGH);
  }
  
  Serial.readBytes(bufBig,18);
  Buf.buf[0] = bufS[0];
  for(int i=0;i<18;i++){
    Buf.buf[i+1] = bufBig[i];
  }
  
  if(Buf.buf[18]!=calc_checkbyte(Buf.buf)){
    //digitalWrite(26,HIGH);
    wrong_telegram = true;
  }
  if(wrong_telegram == true){
    sendRepeatRequest();
    //digitalWrite(22,HIGH);
  }else{
    msg_available = true;
    //digitalWrite(24,HIGH);
  }
}
void send_tel(){
  SenBuf.buf[0] = 'S';
  SenBuf.buf[18] = calc_checkbyte(SenBuf.buf);
  Serial.write(SenBuf.buf,19);
  strncpy(lastSend,SenBuf.buf,19);
}
char calc_checkbyte(char buf[19]){
  char check_byte = 0;
  for(int i = 0;i<18;i++){
    check_byte +=  buf[i];
  }
  return(check_byte);
}

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
  unsigned long step_div; 

  double Se;
  double Vm;
  double Vmin;
  double Bm;
  double Sb;
  double Sv;
}; 

double acc = 0;
double Speed = 30; //mm pro minute
double Speed_use;
double Speed_min = 5; //mm pro minute
double Speed_dif;
double Se;
double Vm;
double dist;
double ges_time;
double tb;
double te;
double tv;

struct StepMotorBig Wachse;
struct StepMotorBig Xachse;
struct StepMotorBig Yachse;
struct StepMotorBig Zachse;

bool msg_available;

char lastSend[19];



unsigned long time_now = micros();
unsigned long cycle_time = micros();
unsigned long cycle_time1 = micros();
unsigned long cycle_time2 = micros();
unsigned long cycle_time3 = micros();
unsigned long old_time_now = micros();

bool sendPose = false;
bool sendPoseInfo = false;

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

//PID
double Output_Bed;
double KP_Bed = 35;
double KI_Bed = 0.2;
double KD_Bed= 750;
double soll_T_Bed = 0;

int temprelai_Bed = 3;

//PID myPID(&soll_T, &Output, &T, KP, KI, KD,P_ON_M, REVERSE);
PID myPID_Bed(&soll_T_Bed, &Output_Bed, &T_Bed, KP_Bed, KI_Bed, KD_Bed,P_ON_E, REVERSE);
unsigned long WindowSize_Bed = 5000;
unsigned long WindowStartSize_Bed = 0;

const unsigned long threshold = 4294967295/2;

int debug = 0;

bool wait_for_response = true;
bool wait_for_Q = true;

int cycle_time_test = 0;
unsigned long  cycle_time_test_start = 0;

void setup() {
  lastSend[0]= '\0';
  Serial.begin(115200,SERIAL_8N2);//9600
  /*TODO
   * EndPins
   */
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
  Xachse.pinPositiv = 50;
  Xachse.pinNull = 52;
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
  Yachse.pinPositiv = 46;
  Yachse.pinNull = 48;
  Yachse.SwitchID = 0;

  Zachse.soll_posi = 0;
  Zachse.act_posi = 0;
  Zachse.soll_step = 0;
  Zachse.act_step = 0;
  Zachse.time_next_step = 0;
  Zachse.time_pstep = 0;
  Zachse.steps_pmm = 50;
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
  Wachse.steps_pmm = 35;
  Wachse.pinENA = 31;
  Wachse.pinDIR = 33;
  Wachse.pinPUL = 35;
    
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
  
  pinMode(Wachse.pinENA,OUTPUT);
  pinMode(Wachse.pinDIR,OUTPUT);
  pinMode(Wachse.pinPUL,OUTPUT);

  pinMode(temprelai,OUTPUT);
  myPID.SetOutputLimits(0, 255);
  myPID.SetSampleTime(WindowSize);
  myPID.SetMode(AUTOMATIC);

  pinMode(temprelai_Bed,OUTPUT);
  myPID_Bed.SetOutputLimits(0, WindowSize_Bed);
  myPID_Bed.SetSampleTime(WindowSize_Bed);
  myPID_Bed.SetMode(AUTOMATIC);

  pinMode(22,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(28,OUTPUT);

  cycle_time = 0;
  cycle_time1 = 0;
  cycle_time2 = 0;
  cycle_time3 = 0;
}
void loop() {
  time_now = micros();
  if(old_time_now>time_now)
  {
    send_debug(999,999,old_time_now,time_now);
    old_time_now = time_now;
  }
  else
  {
    old_time_now = time_now;
  }

//cycletime tester
  if(cycle_time_test>0){
    cycle_time_test++;
  }
  if(cycle_time_test>1000)
  {
    cycle_time_test_start = (time_now-cycle_time_test_start)/1000;
    sendcycletime(0,0,0,cycle_time_test_start);
    cycle_time_test = 0;
  }
  
  if(Serial.available())
  {
    read_Telegram();
  }
  if(msg_available){
    recive_msg();
  }

  checkEndswitches();
  TempControle();
  //motors drive
  treiberBig(Xachse);
  treiberBig(Yachse);
  treiberBig(Zachse);
  treiberBig(Wachse);

  if(Xachse.soll_step == Xachse.act_step){
    if(Yachse.soll_step == Yachse.act_step){
      if(Zachse.soll_step == Zachse.act_step){
        if(Wachse.soll_step == Wachse.act_step){
          if(sendPose == false){
            sendconfirmpos();
            sendPose = true;
          }
          if(sendPoseInfo == false){
            sendactposition();
            sendPoseInfo = true;
          }
        }
      }
    }
  }
    
  if(is_time_over(cycle_time2))
  {
    char bufS[1];
    cycle_time2 = time_now + 50000;
    if(sendPose == true)
      bufS[0] = {'Q'}; //wait for new task
    else
      bufS[0] = {'W'}; //hardware is working 
    Serial.write(bufS,1);
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
    sendendswitch();
    sendactposition();
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

    if(debug == 1)
    {
      sendsettinginfo();
      sendPIDvalues(soll_T,T,Output,0);
    }
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

      sendsettinginfo();
    }
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
    case 'z'://nozzel valus for NTC
      widerstand1 = Buf.tel.value[0];
      bWert = Buf.tel.value[1];
      Rn = Buf.tel.value[2];
      break;
    case 'h'://Bed valus for NTC
      widerstand1 = Buf.tel.value[0];
      bWert = Buf.tel.value[1];
      Rn = Buf.tel.value[2];
      break;
    case 'm'://move to
      Xachse.soll_posi = Buf.tel.value[0];
      Yachse.soll_posi = Buf.tel.value[1];
      Zachse.soll_posi = Buf.tel.value[2];
      Wachse.soll_posi = Buf.tel.value[3];
      getMoveParams();
      sendPose = false;
      //sendConfirmAnswer();
      break;
    case 'j'://move to
      Xachse.soll_posi = Buf.tel.value[0];
      Yachse.soll_posi = Buf.tel.value[1];
      Zachse.soll_posi = Buf.tel.value[2];
      Wachse.soll_posi = Buf.tel.value[3];
      getMoveParams();
      sendPoseInfo = false;
      break;
    case 'p'://set new pose
      Xachse.act_posi = Buf.tel.value[0];
      Yachse.act_posi = Buf.tel.value[1];
      Zachse.act_posi = Buf.tel.value[2];
      Wachse.act_posi = Buf.tel.value[3];
      setPose();
      sendconfirmpos();
      break;
    case 't'://send pose
      sendactposition();
      break;
    case 's': //set speed temperatur and filament
      Speed = Buf.tel.value[0];
      soll_T = Buf.tel.value[1];
      Wachse.steps_pmm = Buf.tel.value[2];
      soll_T_Bed = Buf.tel.value[3];
      sendsetting();
      break;
    case 'w': //set speed temperatur and filament
      Speed = Buf.tel.value[0];
      soll_T = Buf.tel.value[1];
      Wachse.steps_pmm = Buf.tel.value[2];
      soll_T_Bed = Buf.tel.value[3];
      sendsettinginfo();
      break;
    case 'a':
      acc = Buf.tel.value[0];
      Speed_min = Buf.tel.value[1];
      sendAcc(acc,Speed_min,0,0);
      break;
    case 'b'://send stop
      act_equal_soll();
      sendPose = true;
      sendactposition();
      break;
    case 'l'://start cycletime test
      cycle_time_test++;
      cycle_time_test_start = time_now;
      break;
    //case 'r':
      //serieltimeouthandler();
      //break;
    case 'o'://change PID for nozzel
      KP = Buf.tel.value[0];
      KI = Buf.tel.value[1];
      KD = Buf.tel.value[2];
      if(Buf.tel.value[3]>0.5)
        myPID.SetTunings(KP, KI, KD, P_ON_E);
      else
        myPID.SetTunings(KP, KI, KD, P_ON_M);  
      break;
    case 'k'://change PID for Bed
      KP_Bed = Buf.tel.value[0];
      KI_Bed = Buf.tel.value[1];
      KD_Bed = Buf.tel.value[2];
      if(Buf.tel.value[3]>0.5)
        myPID_Bed.SetTunings(KP_Bed, KI_Bed, KD_Bed, P_ON_E);
      else
        myPID_Bed.SetTunings(KP_Bed, KI_Bed, KD_Bed, P_ON_M);  
      break;
    case 'd':
      debug = Buf.tel.value[0];
    default:
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
    dist = abs(Wachse.soll_posi-Wachse.act_posi)*15;
  }
  ges_time = (dist/Speed)*1000000;
  //send_debug(dist,Speed,ges_time,0);
  if(acc!=0)
  {
    //get longest singel dist
    Se = abs(Xachse.soll_posi-Xachse.act_posi);
    if(Se<abs(Yachse.soll_posi-Yachse.act_posi))
      Se = abs(Yachse.soll_posi-Yachse.act_posi);
    if(Se<abs(Zachse.soll_posi-Zachse.act_posi))
      Se = abs(Zachse.soll_posi-Zachse.act_posi);
    //calc max speed

    Speed_dif = Speed - Speed_min;
    if(Speed>sqrt(Se*acc))
      Speed_use = sqrt(Se*acc);
    else
      Speed_use = Speed;
    if(Speed<Speed_min)
      Speed_use = Speed;
      
    tb = Speed_dif/acc;
    te = Se/Speed+tb;
    tv = te - tb;
  }
  BigM_move_params(Xachse);
  BigM_move_params(Yachse);
  BigM_move_params(Zachse);
  BigM_move_params(Wachse);  
}

void BigM_move_params(struct StepMotorBig &StepM){
  StepM.soll_step = StepM.soll_posi*float(StepM.steps_pmm);
  StepM.step_div = abs(StepM.soll_step-StepM.act_step);
  StepM.time_pstep = ges_time/StepM.step_div;
  StepM.time_next_step = time_now;
  if(acc!=0)
  {
    StepM.Vmin = Speed_min;
    StepM.Se = abs(StepM.soll_posi-StepM.act_posi);
    StepM.Vm = StepM.Se/(te-tb);
    StepM.Bm = (StepM.Vm-StepM.Vmin)/tb;
    StepM.Sb = StepM.Bm*pow(tb,2)/2+StepM.Vmin*tb;
    StepM.Sv = StepM.Se-StepM.Sb;
    //sendAcc(acc,StepM.Vmin,StepM.Vm,StepM.Se);
    //sendAcc(acc,StepM.Bm,StepM.Sb,StepM.Sv);
  }
}

unsigned long timeNextStep(struct StepMotorBig &StepM){
  unsigned long t_p_step = StepM.time_pstep;
  double V;
  if(acc == 0)
    t_p_step = StepM.time_pstep;
  else
  {
    
    double S = (abs(StepM.step_div) - abs(StepM.soll_step-StepM.act_step))/StepM.steps_pmm;
    if(S <= StepM.Sb)
      V = StepM.Vmin + ((StepM.Vm-StepM.Vmin)/StepM.Sb)*S;
    else if(S > StepM.Sb && S < StepM.Sv)
      V = StepM.Vm;
    else if(S >= StepM.Sv)
      V = StepM.Vm - ((StepM.Vm-StepM.Vmin)/StepM.Sb)*(S-StepM.Sv);
    t_p_step = 1000000.0/(StepM.steps_pmm*V);
    //sendAcc(acc,V,S,StepM.step_div);
  }
  return t_p_step;
}

///////////////////////////////////fürt einen schrit aus und gibt den actuellen an////////////////////////////7
void treiberBig(struct StepMotorBig &StepM){
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
  act_steps_to_act_posi();
  send_variabelTestCommand('m',Xachse.act_posi,Yachse.act_posi,Zachse.act_posi,Wachse.act_posi);
}
void sendconfirmpos(){
  act_steps_to_act_posi();
  send_variabelTestCommand('c',Xachse.act_posi,Yachse.act_posi,Zachse.act_posi,Wachse.act_posi);
  start_responstimer();
}
void sendConfirmAnswer(){
  send_variabelTestCommand('a',0,0,0,0);
}
void sendsetting(){
  send_variabelTestCommand('s',Speed,T,Wachse.steps_pmm,T_Bed);
}
void sendsettinginfo(){
  send_variabelTestCommand('j',Speed,T,Wachse.steps_pmm,T_Bed);
}
void sendcycletime(float a,float b,float c,float d){
  send_variabelTestCommand('l',a,b,c,d);
  start_responstimer();
}
void sendPIDvalues(float a,float b,float c,float d){
  send_variabelTestCommand('u',a,b,c,d);
  start_responstimer();
}

void sendAcc(float a,float b,float c,float d){
  send_variabelTestCommand('a',a,b,c,d);
  start_responstimer();
}

void send_variabelTestCommand(char C, float val1, float val2, float val3, float val4){
  SenBuf.tel.comand = C;
  SenBuf.tel.value[0] = val1;
  SenBuf.tel.value[1] = val2;
  SenBuf.tel.value[2] = val3;
  SenBuf.tel.value[3] = val4;
  
  send_tel();
  start_responstimer();
}
void start_responstimer(){
  wait_for_response = true;
  cycle_time = time_now + 50000;
}
void stop_responstimer(){
  wait_for_response = false;
}

void sendLast(){
  char bufS[1];
  if(lastSend[0 != '\0'])
  {
    Serial.write(lastSend,19);
  }
  else
  {
    bufS[0] = 'N';
    Serial.write(bufS,1); 
  }
}
void read_Telegram(){
  char bufS[1];
  char bufBig[19];
  for(int i=0;i<25&&Serial.available();i++)
  {
    if(Serial.peek() == 'P')//bestätigung des letzden komandos
    {
      Serial.readBytes(bufS,1);
      bufS[0] = 'P';
      Serial.write(bufS,1);
      return;
    }
    else if(Serial.peek() == 'T')//bestätigung des letzden komandos
    {
      Serial.readBytes(bufS,1);
      lastSend[0]= '\0';
      stop_responstimer();
    }
    else if(Serial.peek() == 'N')//bestätigung des letzden komandos
    {
      Serial.readBytes(bufS,1);
      sendLast();
    }
    else if(Serial.peek() == 'S')//begin eines Telegrams
    {
      if(Serial.available()<19)
        return;   
      Serial.readBytes(Buf.buf,19);
      //check checksumm
      if(Buf.buf[18]!=calc_checkbyte(Buf.buf))//if checksumm wrong ask for reseand
      {
        bufS[0] = 'N';
        Serial.write(bufS,1);
        return;
      }
      bufS[0] = 'T';//if telegram ok send T as respons
      Serial.write(bufS,1);
      msg_available = true;
      return;
    }
    else
    {
      Serial.readBytes(bufS,1);
    }
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

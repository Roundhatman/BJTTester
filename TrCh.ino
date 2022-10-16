#define P1 4
#define P2 5
#define P3 6

#define PinB A0
#define PinC A1

#define ER_UDEF 0
#define ER_LEAK 1
#define ER_DEAD 2
#define TY_NPN  3
#define TY_PNP  4

#define SW_TIME 100
#define END_TEST Serial.println("+--+--+--+--+--+--+--+--+--+");

int P12=0, P13=0, Base=0, C=0, E=0;

void BaseTest(int, int);
int FindBaseNPN(void);
int FindBasePNP(void);
int LeakTest(int);
void hFETest(void);
void GenTest(void);

void setup() {

  // Starting-up
  Serial.begin(9600);
  Serial.println("\nInitializing hardware...");
  delay(100);
  Serial.println("Ready ! \nHardware Interrupt 01 : General Test\nHardware Interrupt 02 : hFE Test\n");
  //attachInterrupt(digitalPinToInterrupt(2), hFETest, RISING);
  //attachInterrupt(digitalPinToInterrupt(3), GenTest, RISING);

}

void loop(){
  if (Serial.available()){
    Serial.read();
    GenTest();
  }
}

void GenTest() {
    if (FindBaseNPN()==TY_NPN){
      if (LeakTest(TY_NPN)==ER_UDEF){
        Serial.println("No leak");
      }
      else Serial.println("Leak !!");
    }
    else{
      Serial.println("This is a PNP or Dead");
      if (FindBasePNP()==TY_PNP){
        if (LeakTest(TY_PNP)==ER_UDEF){
        Serial.println("No leak");
      }
      else Serial.println("Leak !!");
      }
      else Serial.println("Component is not a transistor or transistor is dead or program error");
    }

    END_TEST
}

int FindBaseNPN(){

  int BasePinNPN[3] = {0,0,0};
  Base = 0;
  Serial.println("\n\nAssuming this is a NPN transistor");

  // Base Test NPN P1
  BaseTest(P1,1);
  if (P12==0 && P13==0){
    Serial.println("This is a NPN, Base = P1");
    BasePinNPN[0] = P1;
  }
      
  // Base Test NPN P2
  BaseTest(P2,1);
  if (P12==0 && P13==0){
    Serial.println("This is a NPN, Base = P2");
    BasePinNPN[1] = P2;
  }

  // Base Test NPN P3
  BaseTest(P3,1);
  if (P12==0 && P13==0){
    Serial.println("This is a NPN, Base = P3");
    BasePinNPN[2] = P3;
  }

  if (BasePinNPN[0]==0 && BasePinNPN[1]==0 && BasePinNPN[2]==0) return ER_DEAD;
  if (BasePinNPN[0]>0 && BasePinNPN[1]>1) return ER_DEAD;
  if (BasePinNPN[1]>0 && BasePinNPN[2]>1) return ER_DEAD;
  if (BasePinNPN[2]>0 && BasePinNPN[0]>1) return ER_DEAD;

  for (int i=0; i<3; i++){
    if (BasePinNPN[i]>0) Base = BasePinNPN[i];
  }

  SetPinOuts();
  return TY_NPN;
}

int FindBasePNP(){

  int BasePinPNP[3] = {0,0,0};
  Base = 0;
  Serial.println("\nAssuming this is a PNP transistor");

  // Base Test PNP P1
  BaseTest(P1,0);
  if (P12==0 && P13==0){
    Serial.println("This is a PNP, Base = P1");
    BasePinPNP[0] = P1;
  }
      
  // Base Test PNP P2
  BaseTest(P2,0);
  if (P12==0 && P13==0){
    Serial.println("This is a PNP, Base = P2");
    BasePinPNP[1] = P2;
  }

  // Base Test PNP P3
  BaseTest(P3,0);
  if (P12==0 && P13==0){
    Serial.println("This is a PNP, Base = P3");
    BasePinPNP[2] = P3;
  }

  if (BasePinPNP[0]==0 && BasePinPNP[1]==0 && BasePinPNP[2]==0) return ER_DEAD;
  if (BasePinPNP[0]>0 && BasePinPNP[1]>1) return ER_DEAD;
  if (BasePinPNP[1]>0 && BasePinPNP[2]>1) return ER_DEAD;
  if (BasePinPNP[2]>0 && BasePinPNP[0]>1) return ER_DEAD;

  for (int i=0; i<3; i++){
    if (BasePinPNP[i]>0) Base = BasePinPNP[i];
  }

  SetPinOuts();
  return TY_PNP;
}

void BaseTest(int B, int T_Type){
  P12=0;
  P13=0;

  // Pin Define
  switch (B) {
    case P1:
      C=P2;
      E=P3;
      break;

    case P2:
      C=P1;
      E=P3;
      break;

    case P3:
      C=P1;
      E=P2;
      break;
  }

  if (T_Type==1){
    pinMode(B, INPUT_PULLUP);
    pinMode(C, OUTPUT);
    pinMode(E, INPUT);
    digitalWrite(C, LOW);
    P12 = digitalRead(B);
    delay(SW_TIME);
    
    pinMode(C, INPUT);
    pinMode(E, OUTPUT);
    digitalWrite(E, LOW);
    P13 = digitalRead(B);
    delay(SW_TIME);
  }
  else{
    pinMode(B, OUTPUT);
    digitalWrite(B, LOW);
    
    pinMode(C, INPUT_PULLUP);
    pinMode(E, OUTPUT);
    digitalWrite(E, LOW);
    pinMode(E, INPUT);
    P12 = digitalRead(C);
    delay(SW_TIME);

    pinMode(E, INPUT_PULLUP);
    pinMode(C, OUTPUT);
    digitalWrite(C, LOW);
    pinMode(C, INPUT);
    P13 = digitalRead(E);
    delay(SW_TIME);
  }

}

void SetPinOuts(){
  // Pin Define
  switch (Base) {
    case P1:
      C=P2;
      E=P3;
      break;

    case P2:
      C=P1;
      E=P3;
      break;

    case P3:
      C=P1;
      E=P2;
      break;
  }
}

int LeakTest(int T_Type){

  Serial.print("Runnig leak test...");
  pinMode(Base, OUTPUT);
  digitalWrite(Base, LOW);
  if (T_Type==TY_NPN) digitalWrite(Base, LOW);
  else digitalWrite(Base, HIGH);

  int CE=0, EC=0;
  
  pinMode(C, INPUT_PULLUP);
  pinMode(E, OUTPUT);
  digitalWrite(E, LOW);
  CE = digitalRead(C);
  delay(SW_TIME);

  pinMode(E, INPUT_PULLUP);
  pinMode(C, OUTPUT);
  digitalWrite(C, LOW);
  EC = digitalRead(E);

  if (CE==0 || EC==0) return ER_LEAK;
  return ER_UDEF;

}

void hFETest(){
  pinMode(PinB, INPUT);
  pinMode(PinC, INPUT);

  float Vc=0.0, Vb=0.0, Beta=0.0;

    for (int i=0; i<10; i++){
      Vb = analogRead(PinB);
      Vc = analogRead(PinC);
      Beta += 330.0*(5.0-Vc)/(5.0-Vb);
    }

     Beta /= 10;
    Serial.println("Beta = " + String(Beta) + "\n");
}

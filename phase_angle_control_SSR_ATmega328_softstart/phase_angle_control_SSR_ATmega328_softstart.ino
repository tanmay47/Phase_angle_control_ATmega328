#define AC_LOAD 13
volatile int diff;
int val;
long temp;
volatile int rea,rea1,rea2,stab1,stab2,inc;
volatile int update;
void setup()
{
  cli();
  TCCR1A = 0;
  TCCR1B = 0b00000011; //CS10,CS11)  64 pre   2500 diff 
  TCNT1  = 0;
  OCR1A = 1600;// = (16*10^6) * (100*10^-6) / 1 (must be <65536) 100us
  update = 1600;
  TIMSK1 = 0b00000000;
  stab2=1020;
  stab1=20;
  rea = 0;
  rea1 = 0;
  rea2 = 0;
  
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  attachInterrupt(0, zero_crosss_int, RISING);
  sei();
  pinMode(AC_LOAD, OUTPUT);
}
void zero_crosss_int()
{
  TIMSK1 = 0b00000000;
  PORTB = 0b00000000;
  diff = TCNT1;
  TCNT1=0;
  OCR1A = update;
  
  
  if ( rea > stab2)
  {
    PORTB = 0b00100000;
    //stab1 = 22;
    stab2 = 1019;
    TCNT1=0;
  }
  else if(rea >  stab1)
  {
    stab1 = 20;
    stab2 = 1021;
    TCNT1=0;
    TIMSK1 = 0b00000010;
  }
  else
  {stab1 = 22;}
  
  
  
  if ( PINB & 0b00000010 )  //9 - softstart
  {
    if(rea<rea1)
    {rea++;}
    else //if(rea>rea1)
    {rea--;}
  }
  else
  {rea = rea1;}
  TIMSK1 = 0b00000010;
}

void loop()
{
  rea2 = analogRead(A5);
  if( PINC & 0b00000001 ) //A0 - 4-20mA
  {
    rea2 = rea2 - 205;
    rea2 = rea2 * 5;
    rea1 = rea2 / 4;
  }
  else
    {rea1 = rea2;}
  val = (1024 - rea) ;
  val = val / 8;
  temp = (diff/8)+1;
  temp = val * temp;
  val = temp /16;
  if(val < 50)
  {update = 50;}
  else if(diff - val < 50 )
  {update = diff + 200;}
  else
  {update = val ;}
}

ISR(TIMER1_COMPA_vect){
  TIMSK1 = 0b00000000;
  PORTB = 0b00100000;
}

#define SSR 13
#define zerocross 2
#define softstart 9

volatile unsigned int diff,update,lowerlim,higherlim;
unsigned int val;
unsigned long val1;

void setup()
{
  Serial.begin(9600);
  cli();
  TCCR1A = 0;          //using timer1
  TCCR1B = 0b00000011; //64 prescalar   diff=2500 
  TCNT1  = 0;          //init
  OCR1A = 1600;        // = (16*10^6) * (100*10^-6) / 1 (must be <65536) 100us
  TIMSK1 = 0b00000000; //interrupts masked
  
  pinMode(zerocross,INPUT);
  pinMode(softstart,INPUT);
  pinMode(SSR, OUTPUT);
  attachInterrupt(0, zero_crosss_int, RISING);
  sei();
}

void zero_crosss_int()
{
  TIMSK1 = 0b00000000;
  PORTB = 0b00000000;   //SSR OFF
  diff=2500;//TCNT1;
  TCNT1=0;
  //OCR1A = update;
  //TIMSK1 = 0b00000010;  //interrupts unmasked
  
  if(update > higherlim) //OFF
  {
    higherlim=2430; //dec
    lowerlim=25; //inc
  }
  else if(update > lowerlim) //scale
  {
    higherlim=2480; //inc
    lowerlim=15; //dec
    OCR1A = update;
    TIMSK1 = 0b00000010;  //interrupts unmasked
  }
  else //ON
  {
    PORTB = 0b00100000;    //SSR ON
    higherlim=2480; //dec
    lowerlim=25; //inc
  }
}


void loop()
{
  val = ( 1024-analogRead(A5) );
  val1=0;
  val1 = ( ((unsigned long)val) * (unsigned long)diff );
  Serial.println("====");
  Serial.println(diff);
  Serial.println(val);
  Serial.println(val1);
  val = val1 >> 10;
  
  update = val;
  Serial.println(update);
  /*
  if(val > higherlim) //ON
  {
    update = diff-20;
    higherlim=1000; //dec
    lowerlim=25; //inc
  }
  else if(val > lowerlim) //scale
  {
    update = val;
    higherlim=1010; //inc
    lowerlim=15; //dec
  }
  else //OFF
  {
    update = 20;
    higherlim=1000; //dec
    lowerlim=25; //inc
  }
*/
}

ISR(TIMER1_COMPA_vect)
{
  TIMSK1 = 0b00000000;   //interrupts masked
  PORTB = 0b00100000;    //SSR On
}

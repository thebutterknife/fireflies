#include <Adafruit_NeoPixel.h>
#include <Adafruit_AW9523.h>

#define MAX_RAND 10000  // max 10 seconds until next 
#define MIN_RAND 5000   // min 5 seconds until next blink

// create a pixel strand with 1 pixel on PIN_NEOPIXEL

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);

class FireFly {
  public:
    FireFly( Adafruit_AW9523 aw_in, byte pin_number, byte* patt, int*times)
    {
      aw = aw_in;
      x = times;
      pattern = patt;
      pin_num = pin_number;
      end_time = 1;
      first_time = 0;
      is_active = false;
      
    }
    void update_me( int now_time)
    {
      int delta_time = now_time - first_time;
      byte now_val;
      
      if (is_active)
      {        
        if (delta_time > *next_time ) // move the pointers to the next value 
        {
          next_time++;
          last_time++;
          next_val++;
          last_val++;
        }
        if( *next_time == 0)  // blink sequence over, return and wait
        {
          is_active = false;  // set a random time to next start.  cheaper than calling rand every time on a waiting firefly
          end_time = random( MIN_RAND, MAX_RAND ) + now_time;
          now_val = 0;
        }else{
          now_val = map( delta_time, *last_time, *next_time, *last_val, *next_val); 
        }       
        
        aw.analogWrite( pin_num, now_val);

      }else if (now_time >= end_time) {
        last_time = x;
        next_time = x+1;
        last_val  = pattern;
        next_val  = pattern+1;
        first_time = now_time;
        end_time   = 0;
        is_active  = true;        
      }
      return;
    }
  private:
    Adafruit_AW9523 aw;
    int end_time;
    int first_time;
    byte* pattern;
    byte pin_num;
    int* x;
    bool is_active;
    int* last_time;
    int* next_time;
    byte* last_val;
    byte* next_val;    
};

//0
byte p_potomaca[] = {0, 140, 255, 255,  30,  0,    0, 155, 255, 255, 30,  7,    0,   0};
int  t_potomaca[] = {0, 20,   30, 120, 170, 200, 800, 820, 830, 920, 960, 990, 1010, 0};
//1
byte p_caerulucens[]  = { 0, 255, 0,    0  };
int  t_caerulucens[]  = { 0, 500, 1000, 0  };
//2
byte p_pensylvanica[] = { 0, 255, 255,  30,   0, 128, 255,  200,    0, 0};
int  t_pensylvanica[] = { 0,  20, 120, 140, 240, 260, 300, 1800, 2600, 0}; 
//3
byte p_versicolor[]  = { 0,200,255,200,  0,   0, 200, 255, 200,  0,   0,  180, 235, 180,  0 ,  0,   90, 130,  90,  0,   0,  50, 90, 50, 0  ,  0 , 30, 60, 30,  10,    1,  0,  0}; 
int  t_versicolor[]  = { 0, 30, 50, 70,100, 110, 140, 160, 180, 210, 220, 250, 270, 290, 320, 330, 360, 380, 400, 430, 440, 470, 490, 510, 540, 550, 580, 600, 620, 650, 1800,0};
//4
byte p_fairchildi[] = { 0,180,255,100,  0,   0,  200,255,200,  0,  0 };
int  t_fairchildi[] = { 0, 40,60, 90,  100, 800, 840,860,890, 900, 0 };
//5
byte p_evanescens[]  = { 0, 255, 0,    0  };
int  t_evanescens[]  = { 0, 500, 1000, 0  };
//6
byte p_ceratus[]  = { 0, 30,  0,   0,  30,   0,    0 ,  150,  160,  0, 0};
int  t_ceratus[]  = { 0, 50, 100, 500, 550, 600, 1000, 1050, 1300 , 1400, 0};
//7
byte p_test[]  = { 0,    255, 0};
int  t_test[]  = { 0, 25500, 0};
//8
byte p_time[]  = { 0, 255,  255, 0};
int  t_time[]  = { 0, 950, 1000, 0};

int*  ts[] = { t_potomaca, t_caerulucens, t_pensylvanica, t_versicolor, t_fairchildi, t_evanescens, t_ceratus, t_test, t_time};
byte* ps[] = { p_potomaca, p_caerulucens, p_pensylvanica, p_versicolor, p_fairchildi, p_evanescens, p_ceratus, p_test, p_time};

Adafruit_AW9523 aw1;
FireFly* f[16];
int x = 0;
byte y[] = {0,1,2,3,5,7,10,15,22,31,50,78,110,155,190,255};
 
void setup() {
  
  int pat_sel;

  randomSeed( analogRead(0) );
  
  if (! aw1.begin(0x58)) {
  Serial.println("AW9523 not found? Check wiring!");
  while (1) delay(10);  // halt forever
  }

  for( int i = 0; i < 16; i++)
  {
    aw1.pinMode( i, AW9523_LED_MODE);
  }

  // quick check that things were initialized.
  aw1.analogWrite(0, 255);
  delay( 500 );
  aw1.analogWrite(0, 0);
  
  for( int i = 0; i < 16; i++)
  {
    aw1.analogWrite(i, 0);
    pat_sel = random(0,7);
    //pat_sel = 3;//(i/2)%7;
    f[i] = new FireFly( aw1, i, ps[ pat_sel ], ts[ pat_sel ]);
  }
}

void loop() {
   
    for( int i = 0; i < 16; i++)
    {
      //aw1.analogWrite(i, y[i]);
      f[i]->update_me( millis());
    }
    x += 16;
    delay(1);
}

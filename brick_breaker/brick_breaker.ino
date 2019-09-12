#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//port
//--LED Matrix--
//-Row-
//Kiri
#define R1   PORTL&(0x01<<0)
#define R2  PORTL&(0x01<<4)
#define R3  PORTC&(0x01<<3)
#define R4  PORTL&(0x01<<2)
#define R5  PORTL&(0x01<<7)
#define R6  PORTC&(0x01<<1)
#define R7  PORTG&(0x01<<1)
#define R8  PORTD&(0x01<<7)
//Kanan
#define R9  PORTC&(0x01<<4)
#define R10 PORTC&(0x01<<0)
#define R11 PORTC&(0x01<<5)
#define R12 PORTC&(0x01<<2)
#define R13 PORTA&(0x01<<4)
#define R14 PORTC&(0x01<<7)
#define R15 PORTA&(0x01<<2)
#define R16 PORTA&(0x01<<6)
//-Collumn
#define C1  PORTL&(0x01<<1)
#define C2  PORTA&(0x01<<3)
#define C3  PORTA&(0x01<<1)
#define C4  PORTB&(0x01<<1)
#define C5  PORTA&(0x01<<0)
#define C6  PORTB&(0x01<<3)
#define C7  PORTL&(0x01<<3)
#define C8  PORTL&(0x01<<5)
//--Switch--
#define switch1 PINB&(0x01<<0)//kanan
#define switch2 PINB&(0x01<<2)//kiri
#define switch3 PINL&(0x01<<1)//start
#define switch4 PINL&(0x01<<3)//con

//--Buzzer
#define buzz PORTC&(0x01<<6)
//variabel

//batas posisi

int start_x=0;
int end_x=15;
int start_y=0;
int end_y=7;
//posisi bola
int ball_x=9;
int ball_y=6;//posisi awal bola
int ball_dir=0;//arah gerak bola
//posisi bar
int bar_x=9;//letak titik tengah bar 3titik, min 1 max 14
int bar_y=7;//bar_y tidak boleh berubah

//titik pada layar
uint16_t screen[8];
//counter
int brick=48;//jumlah brick yang ada
int count=0;//permainan selesai jika count mencapai brick
//enable
int en_row=0;
//posisi
int row,
  col;
//kondisi
int start=0,
  con=0;
//fungsi
void init_int(void);
void init_brick(uint16_t pos[8]);
void init_player(uint16_t pos[8]);
int take_bit(int b,int i);
void ball_move(int x, int y, int dir);
int ball_limit(int x,int y,int dir);
void clash(int x, int y, int dir,uint16_t screen[8]);
void buzz_clash(int x, int y, int dir,uint16_t screen[8]);
void end_screen(void);
void light_led(int en_row);




ISR(Timer1_OVF_vect)
{
  Serial.begin(9600);
   Serial.println(switch1);
  if(con==0)
  {
    if(start==0)
    {
      
      //gerak ke kanan atas
      ball_move(ball_x, ball_y, ball_dir);
      //pantulan bola
      clash(ball_x,ball_y,ball_dir, screen);
//buzz_clash(ball_x,ball_y,ball_dir,screen);
    //batasan bola
      ball_dir=ball_limit(ball_x,ball_y,ball_dir);
      if(switch1==0)//gerak bar ke kanan
      {
        if(bar_x<=14)
        bar_x+=1;
        if(ball_dir==0)
        {
          ball_dir=1;
        }
      }
      else if(switch2==0)//gerak bar ke kiri
      {
        if(bar_x>=1)
        bar_x-=1;
        if(ball_dir==0)
        {
          ball_dir=2;
        }
      }
      if(switch3==0)//pause
      {
        start=0;
      }
    }
    else
    {
      if(switch4==0)//start
      {
        start=1;
      }
    }
  }
  else
  {
    if(switch4==0)//play again
    {
      con=0;
      init_brick(screen);
      init_player(screen);
    }
  }
  //set ulang timer
  
  TCNT1=3036;
  TIFR1 = (1<<TOV1);
  
}
// ISR(INT0_vect)//gerakkan bar ke kanan
// {
  // if(start==1)
  // {
    // if(bar_x<=14)
    // {
      // bar_x+=1;
    // }
  // }
// }
// ISR(INT1_vect)//gerakkan bar ke kiri
// {
  // if(start==1)
  // {
    // if(bar_x>=1)
    // {
      // bar_x-=1;
    // }
  // }
// }
// ISR(INT2_vect)//Start button??
// {
  // if(start==0)
  // {
    // start=1;
  // }
  // else if(start==1)
  // {
    // start=0;
  // }
// }
int main(void)
{ 
  DDRA=0xFF;
  DDRB=0x0B;
  DDRC=0xFF;
  DDRD=0x80;
  DDRL=0xF5;
  
  PORTB=0x05;
  PORTL=0x0A;
  Serial.begin(9600);
  init_int();
  init_brick(screen);
  init_player(screen);
  while(1)
  {
  
  
  //display 1khz
    if(en_row==0)
    {
      //aktifkan enable r1
      PORTL^=(1<<PL1)|(1<<PL5);
     
    }
    else if(en_row==1)
    {
      //aktifkan enable r2
      PORTL^=(1<<PL1);
      PORTA^=(1<<PA3);
   
    
    }
    else if(en_row==2)
    {
      //aktifkan enable r3
       
      PORTA^=(1<<PA1)|(1<<PA3);
      
    
    }
    else if(en_row==3)
    {
      //aktifkan enable r4
       
      PORTA^=(1<<PA1);
      PORTB^=(1<<PB1);
    
    }
    else if(en_row==4)
    {
      //aktifkan enable r5
     
      PORTA^=(1<<PA0);
      PORTB^=(1<<PB1);
    
    }
    else if(en_row==5)
    {
      //aktifkan enable r6
       
      PORTA^=(1<<PA0);
      PORTB^=(1<<PB3);
    
    }
    else if(en_row==6)
    {
      //aktifkan enable r7
     PORTL^=(1<<PL3);
      PORTB^=(1<<PB3);
    
    }
    else if(en_row==7)
    {
      //aktifkan enable r8
     PORTL^=(1<<PL3)|(1<<PL5);
      
     
    }
     
    screen[7]=0x0007<<(bar_x-1);
    screen[ball_y]=0x0001<<(ball_x);
    //letak brick
    light_led(en_row);
    //perubahan en_row
    en_row+=1;
    if(en_row==8)
    {
      en_row=0;
    }
    if((count==brick)||(ball_y==7))
    {
      con=1;
      end_screen();
    }
    //frekuensi scanning
    _delay_ms(6);
  }
}








//Fungsi
//fungsi inisialisasi interrupt
void init_int(void)
{
  //inisialisasi timer
  cli();
  TCCR1A=0;
  TCCR1B=0;
  TIMSK1 |= (1<<TOIE1);
  TCCR1B |= (1<<CS12);//prescaller 256
  //waktu per interrupt
  TCNT1=3036;
  sei();
  //aktifkan interrupt
  
  //aktifkan interrupt eksternal (switch)
//  EICRA=0b00111111;//falling edge INT2 INT1 INT 0 mengaktifkan interrupt
//  EIMSK=0b00000111;// Mengaktifkan enable INT2 INT1 INT0
//  EIFR=0b00000111;//insialisasi flag INT2 INT1 INT0
}
//Fungsi untuk inisialisasi brick
void init_brick(uint16_t pos[8])
{
  pos[0]=0xFFFF;
  pos[1]=0xFFFF;
  pos[2]=0xFFFF;
}
//fungsi inisialisasi bar dan bola
void init_player(uint16_t pos[8])
{
  pos[6]=0x0001<<(ball_x);//inisialisasi bola
  pos[7]=0x0004;//inisialisasi bar
  ball_dir=0;
}
int take_bit(int b, int i)
{
  int temp;
  int r;
  int c;
  r=b;
  c=i;
  if(b>7)
  {
     r=b-8;
     c=i+8;
   }
  temp=(screen[r]&(1<<c))&&1;
  return(temp);  
}
void ball_move(int x, int y, int dir)
{
  
  if(dir==1)
  {
    ball_x+=1;
    ball_y-=1;
  }
  //gerak ke kiri atas
  else if(dir==2)
  {
    ball_x-=1;
    ball_y-=1;
  }
  //gerak ke kanan bawah
  else if(dir==3)
  {
    ball_x+=1;
    ball_y+=1;
  }
  //gerak ke kiri bawah
  else if(dir==4)
  {
    ball_x-=1;
    ball_y+=1;
  }
}
int ball_limit(int x, int y, int dir)
{
  int temp;
  temp=dir;
  if(x==end_x)
  {
    temp+=1;
  }
  else if(y==end_y)
  {
    temp+=2;
  }
  else if(x==start_x)
  {
    temp-=1;
  }
  else if(y==start_y)
  {
    temp-=1;
  }
  return(temp);
}
void clash(int x, int y, int dir,uint16_t screen[8])
{
  
  if(dir!=0)
  {
    if(((screen[y+1]&(0x0001<<x))==1)&&(dir>2))//kondisi menabrak bar atau brick dibawah
    {
      dir-=2;
      if(y!=6)//kondisi menabrak brick
      {
        screen[y+1]&=~(0x0001)<<x;
        count+=1;
      }
      
       
    }
    if((screen[y-1]&(0x0001<<x))==1)//kondisi menabrak brick di atas
    {
      dir+=2;
      screen[y-1]&=~(0x0001)<<x;
      count+=1;
    }
    if((screen[y]&(0x0001<<(x+1)))==1)//kondisi menabrak brick di kanan
    {
      dir+=1;
      if(y!=7)
      {
        screen[y]&=~(0x0001)<<(x+1);
        count+=1;
      }
    }
    if((screen[y]&(0x0001<<(x-1)))==1)//kondisi menabrak brick di kiri
    {
      dir-=1;
      if(y!=7)
      {
        screen[y]&=~(0x0001)<<(x-1);
        count+=1;
      }
    }
  }
}
void buzz_clash(int x, int y, int dir,uint16_t screen[8])
{
  if(dir!=0)
  {
    if(((screen[y+1]&(0x0001<<x))==1)&&(dir>2))//kondisi menabrak bar atau brick dibawah
    {
     
      if(y!=6)//kondisi menabrak brick
      {
        PORTC=(1<<PC6);
      }
      
       
    }
    if((screen[y-1]&(0x0001<<x))==1)//kondisi menabrak brick di atas
    {
      PORTC=(1<<PC6);
    }
    if((screen[y]&(0x0001<<(x+1)))==1)//kondisi menabrak brick di kanan
    {
      
      if(y!=7)
      {
       PORTC=(1<<PC6);
      }
    }
    if((screen[y]&(0x0001<<(x-1)))==1)//kondisi menabrak brick di kiri
    {
      
      if(y!=7)
      {
        PORTC=(1<<PC6);
      }
    }
  }
}
void end_screen(void)
{
  if(count==brick)
  {
    //WIN
    screen[0]=0x0000;
    screen[1]=0x0000;
    screen[2]=0x1554;
    screen[3]=0x1D54;
    screen[4]=0x1528;
    screen[5]=0x0000;
    screen[6]=0x0000;
    screen[7]=0x0000;
  }
  else
  {
       screen[0]=0x0000;
      screen[1]=0x0000;
      screen[2]=0x0000;
      screen[3]=0x0000;
      screen[4]=0x0000;
      screen[5]=0x0000;
      screen[6]=0x0000;
      screen[7]=0x0000;
   }
}
void light_led(int en_row)
{
  PORTA^=((-take_bit(14,en_row)^PORTA)&(1<<PA2))|((-take_bit(12,en_row)^PORTA)&(1<<PA4))|((-take_bit(15,en_row)^PORTA)&(1<<PA6));
  PORTC^=((-take_bit(9,en_row)^PORTC)&(1<<PC0))|((-take_bit(5,en_row)^PORTC)&(1<<PC1))|((-take_bit(11,en_row)^PORTC)&(1<<PC2))|((-take_bit(2,en_row)^PORTC)&(1<<PC3))|((-take_bit(8,en_row)^PORTC)&(1<<PC4))|((-take_bit(10,en_row)^PORTC)&(1<<PC5))|((-take_bit(13,en_row)^PORTC)&(1<<PC7));
  PORTD^=((-take_bit(7,en_row)^PORTD)&(1<<PD7));
  PORTG^=((-take_bit(6,en_row)^PORTG)&(1<<PG1));
  PORTL^=((-take_bit(0,en_row)^PORTL)&(1<<PL0))|((-take_bit(3,en_row)^PORTL)&(1<<PL2))|((-take_bit(1,en_row)^PORTL)&(1<<PL4))|((-take_bit(4,en_row)^PORTL)&(1<<PL7));
}


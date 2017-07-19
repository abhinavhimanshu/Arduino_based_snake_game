/*Clock and latch pins for vertical and horizontal shift registers*/
#define clockpinV 10
#define clockpinH 11
#define latchpinH 4
#define datapinH  6
#define datapinV  7
#define latchpinV 8

volatile bool clockwise = LOW;
volatile bool anticlockwise = LOW;

uint8_t xball=2,yball=1,dir=0; //cordinates of ball and direction of snake head
bool matrix[8][8];//Details of matrix 8X8
uint8_t snakex[8],snakey[8];
int len = 3;//intial length of snake 
byte ch;    
int speed =100;//speed of movment 
bool hit=false; // if hit true GAME OVER

//Clockwise turn of snake, called by an interrupt
void clock()
{
  delay(5);
  clockwise=HIGH;
  anticlockwise=LOW;
}

//Anticlock turn of snake, called by an interrupt
void anticlock()
{
  delay(5);
  clockwise=LOW;
  anticlockwise=HIGH;
}


void tobyte(uint8_t k)
 {
   ch=0b00000000;
   uint8_t i;
   for(i=0;i<8;i++)
     ch=ch|(matrix[i][k]<<i);
 }



//Movment of snake body corresponding to direction of turn
void move()
 {
   switch(dir)
    {
      case 0:
      snakex[0]=snakex[0];
      snakey[0]=snakey[0]-1;
      break; 
      case 1:
       snakex[0]=snakex[0]-1;
       snakey[0]=snakey[0];
       break;
      case 2:
       snakex[0]=snakex[0]+1;
       snakey[0]=snakey[0];
       break;
      case 3:
       snakex[0]=snakex[0];
       snakey[0]=snakey[0]+1;
       break; 
    }
 }

//This function sets the direction of Snake
void direction()
{
  uint8_t i;
 
  if(hit==false || len==8)
   { matrix[snakex[len-1]][snakey[len-1]]=false;hit = false;}
  else
  {
   hit=false;
   len++;
  }
   for(i=len-1;i>0;i--)//shifting snake's body
   {
    snakex[i]=snakex[i-1];
    snakey[i]=snakey[i-1];
    matrix[snakex[i]][snakey[i]]=true;
  }
  
  if(clockwise==HIGH && anticlockwise==LOW)// change dir according to old dir and turn taken
  {
    clockwise=LOW;
    anticlockwise=LOW;
    switch (dir)
        {
          case 0 : //OLDTOP
          dir = 2;//new right
          break;
          case 1 ://OLD LEFt
          dir = 0;//new top
          break;
          case 2 ://OLD RIGHT
          dir = 3;//new bottom
          break;
          case 3 ://OLD BOTTOM
          dir = 1;//new left
          break;
        }
 
    }
  else if(anticlockwise==HIGH && clockwise==LOW) // change dir according to old dit and turn taken
    {
       clockwise=LOW;
       anticlockwise=LOW;
       switch (dir)
       {
          case 0 : //OLDTOP
          dir = 1;//newleft
          break;
          case 1 ://OLDleft
          dir = 3;//bottom
          break;
          case 2 ://OLDRIGHT
          dir = 0;//new top
          break;
          case 3 ://OLDBOTTOM
          dir = 2;//new right
           break;
      }
    
    }
     move();// make movments
     
    matrix[snakex[0]][snakey[0]]=true;
}



// get random position of ball
void ball()
{
   uint8_t i;
   if(xball==snakex[0] && yball==snakey[0])
   { 
    xball=rand()%8;
    yball=rand()%8;
    hit=true;
    bool flag=false;
    for(i=0;i<len;i++)
     {
      if(xball==snakex[i] && yball==snakey[i] )
      { flag=true; break;}
     }
     if(flag==false)
     return;
     else
     ball();
   } 
 }



//check if snake hits the boundaries
 int check()
 {
   uint8_t i;
   if(snakex[0]>7 || snakex[0]<0||snakey[0]>7 || snakey[0]<0)
     return 1;
   for(i=1;i<len;i++)
     if(snakex[i]==snakex[0] && snakey[i]==snakey[0])  
     return 1;
   return 0;  
 }


void setup()
{
  uint8_t i,j;
  attachInterrupt(INT0,clock, RISING); // attach user input as interrupts to improve the response
  attachInterrupt(INT1,anticlock,RISING); //These functions are automatically called when pins get a rising signal
  pinMode(clockpinH,OUTPUT);//Horizontal latches for shift register to control dot matrix
  pinMode(clockpinV,OUTPUT);//Vertical clock for shift register to control dot matrix
  pinMode(latchpinH,OUTPUT);//Horizontal latches for shift register to control dot matrix
  pinMode(datapinH,OUTPUT);//Horizontal data for shift register to control dot matrix
  pinMode(latchpinV,OUTPUT);//Vertical latches for shift register to control dot matrix
  pinMode(datapinV,OUTPUT);//Vertical data for shift register to control dot matrix
  digitalWrite(latchpinH,HIGH);
  digitalWrite(latchpinV,HIGH);
  for(i=0;i<8;i++)
  for(j=0;j<8;j++)
  matrix[i][j]=false;
  snakex[0]=3;
  snakex[1]=4;
  snakex[2]=5;
  snakey[0]=3;
  snakey[1]=3;
  snakey[2]=3;
   for(i=0;i<3;i++)
   matrix[snakex[i]][snakey[i]]=true;
}

//each frame of game is present in each refresh call
void refresh()
{
   uint8_t i,j;
   
  for(j=0;j<30;j++) 
  {
     for(i=0;i<8;i++)
       {
          digitalWrite(latchpinV,LOW);
          shiftOut(datapinV,clockpinV,MSBFIRST,~(0b00000001<<i));
          digitalWrite(latchpinV,HIGH);
          tobyte(i); 
          digitalWrite(latchpinH,LOW);
          shiftOut(datapinH,clockpinH,MSBFIRST,ch);
          digitalWrite(latchpinH,HIGH);
          delay(2);
          digitalWrite(latchpinH,LOW);
          shiftOut(datapinH,clockpinH,MSBFIRST,0b00000000);
          digitalWrite(latchpinH,HIGH);
        }
   }
}




//Run all this  in loop
void loop()
{
   ball();
   matrix[xball][yball]=true;
   if(check())
    {while(1) refresh();}
   
    refresh();  
    direction();
}

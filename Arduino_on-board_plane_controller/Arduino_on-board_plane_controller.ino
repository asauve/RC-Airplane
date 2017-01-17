
//one packet arrives every 25 ms
#include <Servo.h>

class airplane
{
  public:
  bool connected=false;
  bool armed=false;
  float throttle=0; //(scale of -1 to 1)
  float leftFlap=0; //(scale of -1 to 1)
  float rightFlap=0; //(scale of -1 to 1)
  int last; //time last packet was received (millis)
  int sincelast; //elapsed time since last packet (millis)
Servo esc;
Servo left;
Servo right;

/////////////////////////////////////////////////////////////////////////////////////
///      class functions
/////////////////////////////////////////////////////////////////////////////////////

  float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
  {
   return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
  }

  
  void readSerial(void){
    char start=Serial.read();
      switch (start){
      case'!':
      {
        throttle=Serial.parseFloat();
        break;  
      }
      case '@': //leftFlap
      {
        leftFlap=Serial.parseFloat();
        break;
      }
      case '#': //rightFlap
      {
        rightFlap=Serial.parseFloat();
        break;
      }
    }//end of switch/case
  }//end of readSerial



void sweepFlaps(void){
  
  for (float x=0; x<=1.0; x+=0.001){
    leftFlap=x;
    rightFlap=x;
    outputData();
    delay(1);
   }    
   for (float x=1; x>=-1.0; x-=0.001){
    leftFlap=x;
    rightFlap=x;
    outputData();
    delay(1);
   }    
   for (float x=-1.0; x<=0.0; x+=0.001){
    leftFlap=x;
    rightFlap=x;
    outputData();
    delay(1);
   }    
  }//end of sweepFlaps
  

  void outputData(void){
    if (throttle<0){throttle=0;} //only fly forwards
    left.write(mapfloat(leftFlap, -1.0, 1.0, 25.0, 90.0));    
    esc.writeMicroseconds(mapfloat(throttle, 0, 1, 1148, 1832)); //send the correct pulse width to the esc.
    right.write(mapfloat(rightFlap, -1.0, 1.0, 155.0, 90.0));
  }//end of outputData
};//end of airplane class declaration


airplane airplane;//global airplane object, named airplane

void setup()
{
  airplane.esc.attach(9);// Electric Speed Controller data line
  airplane.left.attach(10);// Left servo data line
  airplane.right.attach(11);//Right servo data line
  pinMode(1,OUTPUT); //TX(0)
  pinMode(0,INPUT);  //RX(0)
  Serial.begin(9600);

//initialize variables
airplane.throttle=0.0;
airplane.leftFlap=0.0;
airplane.rightFlap=0.0;


airplane.sweepFlaps();

}//end of setup



void loop()
{
  while (Serial.available() == 0){//waiting for a message
      airplane.sincelast=millis()-airplane.last;
      if (airplane.sincelast>4000){
        //four seconds since last transmission: stop motor
        airplane.throttle=0;
        airplane.outputData();
      }else if (airplane.sincelast>55){
        //turn on warning LED: around 2 packets were dropped
      }
  }
  
//happens when a packet is received
airplane.readSerial(); //interpret packet
airplane.outputData();//send out the corresponding signals to esc/servo/LED
airplane.last=millis(); //record the time of the last sucessful transmission
airplane.connected=true;
}//end of loop

  float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
  {
   return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
  }




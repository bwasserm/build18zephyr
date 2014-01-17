#include <SPI.h>
#include <PacketLink.h>

// Pin Assignment Defines
#define HOIST_SERVO_PIN  9
#define LEFT_MOTOR_PIN   4
#define RIGHT_MOTOR_PIN  5
#define MIC_L_PIN       A0
#define MIC_C_PIN       A1
#define MIC_R_PIN       A2

// Hoist positions
#define HOIST_UP 0
#define HOIST_DOWN 1
#define HOIST_RAISING 2
#define HOIST_LOWERING 3

#define GO_RIGHT 30
#define GO_LEFT -30

// Globals
int target = 0;
int hoist_position = HOIST_UP;
PacketLink link;

// SPI globals
volatile byte spi_val = 0;
volatile boolean spi_available;
byte angle; //angle in degrees that the sound source is at
boolean hoist_is_lowered;

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  spi_val = SPDR;  // grab byte from SPI Data Register
  
  spi_available = true;
}  // end of interrupt routine SPI_STC_vect
boolean check_for_obstacles() 
{
 return false; 
}
void turn_right()
{
  
}

void turn_left()
{
  
}
void move_forward()
{
  if (!check_for_obstacles()) {
    
  }
  //TODO
  
}
void stay_in_place()
{
  
}
boolean lower_hoist()
{
  return hoist_is_lowered;
}
boolean raise_hoist()
{
  return hoist_is_lowered;
}

void  setup(){
  
  // Setup SPI Slave
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  
  // get ready for an interrupt 
  spi_available = false;

  // now turn on interrupts
  SPI.attachInterrupt();
  
  link.set_id(BLIMP_ADDR);
  
  //init some globals
  hoist_is_lowered = false;

  Serial.begin(BAUD);
}

void loop(){
  
   byte command = CMD_NULL;
   command = link.check_packets();
   byte sender = 0;
   switch(command){
    case(CMD_NULL):
    case(CMD_PING):
    case(CMD_ECHO):
    case(CMD_BCN_START):
    case(CMD_BCN_STOP):
    case(CMD_BCN_BEEP):
      break;
    case(CMD_BCN_REQ):
      sender = link.get_last_sender();
      //if no target, start target, make sure other remote is off
      if(target == 0) {
        target = sender;
        if(sender == REM1_ADDR) {
          link.send_packet(REM1_ADDR,CMD_BCN_START);
          //don't need this if we stop when we hoist and ignore conflicting targets
       //   link.send_packet(REM2_ADDR,CMD_BCN_STOP); 
        } else {
          link.send_packet(REM2_ADDR,CMD_BCN_START);
        //  link.send_packet(REM1_ADDR,CMD_BCN_STOP);
        }
      }
      //otherwise, either already targeting remote or targeting the other remote, so do nothing
      break;
    case(CMD_HOIST):
      link.send_packet(target,CMD_BCN_STOP);
      if(hoist_is_lowered) {
        hoist_is_lowered = lower_hoist();
      }
      else {
        hoist_is_lowered = raise_hoist();
      }
      // Nothing needs to be done for these
      break;
    default:
      // Nothing to do. Maybe flash LED to indicate error?
      break;
  }
  
  //beaconing stuff
  if(target > 0){
      // Check SPI buffer, get angle
    if(spi_available){
      angle = spi_val;
      Serial.println(spi_val);
      spi_available = false;
      // this is just placeholding atm
      if(angle < GO_RIGHT) {
         turn_right();
      } 
      else if(angle > GO_LEFT) {
         turn_left();
      }
    }  // end of flag set
    move_forward();
  } 
  else {
    stay_in_place();
  }

  
}

#include <SPI.h>
#include <PacketLink.h>

// Pin Assignment Defines
#define LEFT_MOTOR_PIN A0
#define RIGHT_MOTOR_PIN A1
#define HOIST_POS_PIN A2
#define HOIST_NEG_PIN A3

#define PROX_L A5
#define PROX_C A6
#define PROX_R A7

// Hoist positions
#define HOIST_UP 0
#define HOIST_DOWN 1
#define HOIST_RAISING 2
#define HOIST_LOWERING 3

#define GO_RIGHT 30
#define GO_LEFT -30

// time for hoist travel, in ms
#define HOIST_TIME 5000
// distance before turning
#define PROX_THRESH 768

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
boolean check_for_obstacles(int direction) 
{
  return(PROX_THRESH < analogRead(direction));
}
void turn_right()
{
  digitalWrite(LEFT_MOTOR_PIN, 1);
  digitalWrite(RIGHT_MOTOR_PIN, 0);
}

void turn_left()
{
  digitalWrite(LEFT_MOTOR_PIN, 0);
  digitalWrite(RIGHT_MOTOR_PIN, 1);
}
void move_forward()
{
  if (!check_for_obstacles(PROX_C)) {
    if(check_for_obstacles(PROX_L)) {
      turn_right();
    } else if(check_for_obstacles(PROX_R)) {
      turn_left();
    }
  } else {
    digitalWrite(LEFT_MOTOR_PIN, 1);
    digitalWrite(RIGHT_MOTOR_PIN, 1);
  }
}

void stay_in_place()
{
  digitalWrite(LEFT_MOTOR_PIN, 0);
  digitalWrite(RIGHT_MOTOR_PIN, 0);
}

boolean lower_hoist()
{
  digitalWrite(HOIST_POS_PIN, 0);
  digitalWrite(HOIST_NEG_PIN, 1);
  hoist_position = HOIST_LOWERING;
  delay(HOIST_TIME);
  // stop the hoist
  digitalWrite(HOIST_POS_PIN, 0);
  digitalWrite(HOIST_NEG_PIN, 0);
  hoist_position = HOIST_DOWN;
  return true; // the hoist is lowered
}
boolean raise_hoist()
{
  digitalWrite(HOIST_POS_PIN, 1);
  digitalWrite(HOIST_NEG_PIN, 0);
  hoist_position = HOIST_RAISING;
  delay(HOIST_TIME);
  // stop the hoist
  digitalWrite(HOIST_POS_PIN, 0);
  digitalWrite(HOIST_NEG_PIN, 0);
  hoist_position = HOIST_UP;
  return false; // the hoist is not lowered
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
  
  // set up motor pins
  pinMode(HOIST_UP, OUTPUT);
  pinMode(HOIST_DOWN, OUTPUT);
  pinMode(HOIST_POS_PIN, OUTPUT);
  pinMode(HOIST_NEG_PIN, OUTPUT);

  // set up prox pins
  pinMode(PROX_L, INPUT);
  pinMode(PROX_C, INPUT);
  pinMode(PROX_R, INPUT);

  //init some globals
  hoist_is_lowered = false;
  hoist_position = HOIST_UP;

  Serial.begin(BAUD);
}

void loop(){
  
   byte command = CMD_NULL;
   command = link.check_packets();
   byte sender = 0;
   switch(command){
    case(CMD_NULL):
     break;
    case(CMD_BCN_START):
      target = 0; //reset automatic targeting when changed to manual
      move_forward();
      break;
    case(CMD_BCN_STOP):
      stay_in_place();
      break;
    case(CMD_MANUAL_LEFT):
      turn_left();
      break;
    case(CMD_MANUAL_RIGHT):
      turn_right();
      break;
    case(CMD_BCN_REQ):
    case(CMD_BCN_DEP):
      sender = link.get_last_sender();
      //if no target, start target, make sure other remote is off
      if(target == 0) {
        //if request, send start instruction to sender, if deploy, send it to the other remote
        if((sender == REM1_ADDR && command == CMD_BCN_REQ) || (sender == REM2_ADDR && command == CMD_BCN_DEP)) {
          link.send_packet(REM1_ADDR,CMD_BCN_START);
          target = REM1_ADDR;
        } else {
          link.send_packet(REM2_ADDR,CMD_BCN_START);
          target = REM2_ADDR;
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
        target = 0; //reset target, person has presumably received or added a message
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

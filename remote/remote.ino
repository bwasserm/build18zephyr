#include <PacketLink.h>

//change this for the different remotes!
#define MY_ADDR REM1_ADDR

// Pin Assignment Defines
#define BEACON_LED_PIN  10
#define READY_LED_PIN   11
//location of each button in button array
#define MANUAL 0
#define LEFT_BEACON 1
#define RIGHT_DEPLOY 2
#define HOIST 3

#define NUM_BUTTONS 4

//this is just in case I mixed up the buttons, so I don't have to retype it everywhere
#define UP LOW
#define DOWN HIGH

#define DEBOUNCE_DELAY 10


// Globals
boolean beaconing = false;
boolean manual_mode = false;
PacketLink link;

//array for button pin defines
byte buttons[] = {2,3,4,5};
int button_state[] = {UP, UP, UP, UP};

/* Beacons to blimp
 * Sends an audio signal to the blimp, flashes Tracking LED, sends RF packet to confirm
 * beaconing to other nodes (so only one beacons at a time)
 */
/*void beacon(){
  // Don't beacon if beaconing is disabled (duh)
  if(beaconing == false){
    return;
  }
}
*/
/* Enable the beaconing function
 */
void enable_beaconing(){
  beaconing = true;
  digitalWrite(BEACON_LED_PIN, HIGH);
}

/* Disable the beaconing function
 */
void disable_beaconing(){
  beaconing = false;
  digitalWrite(BEACON_LED_PIN, LOW);
}


void read_debounce()
{
  static int last_state[NUM_BUTTONS];
  static long last_debounce_time;
  int index;

  if (millis() < last_debounce_time) {
     // we wrapped around, lets just try again
     last_debounce_time = millis();
  }
  if(millis() - last_debounce_time < DEBOUNCE_DELAY) {
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  last_debounce_time = millis();
  
  for (index = 0; index < NUM_BUTTONS; index++) {
       button_state[index] = digitalRead(buttons[index]);   // read the button
     }
    last_state[index] = button_state[index];   // keep a running tally of the buttons
}



/* Checks the buttons. Returns an appropriate command
 * @return The command to activate based on the pressed buttons
 */
byte check_buttons(){
  read_debounce();
  byte command = CMD_NULL;
  //if manual button, switch between modes
  if(button_state[MANUAL] == DOWN) {
   if(manual_mode) {
    manual_mode = false;
    return CMD_BCN_STOP;
   } 
   else {
    manual_mode = true;
    return CMD_BCN_START; 
   }
  }
  //if left/beacon button, go left or summon blimp depending on mode
  else if(button_state[LEFT_BEACON] == DOWN) {
    //if manual mode, go left
    if(manual_mode){
       return CMD_MANUAL_LEFT;
    } 
    //in automatic mode, acts as a beacon
    else {
     return CMD_BCN_REQ;
    }
  }
  //if right/deploy button, go right or deploy blimp depending on mode
  else if(button_state[RIGHT_DEPLOY] == DOWN) {
    if(manual_mode) {
      return CMD_MANUAL_RIGHT;
    }
    else {
      return CMD_BCN_DEP;
    }
  }
  //if hoist, raise or lower hoist
  else if(button_state[HOIST] == DOWN) {
    return CMD_HOIST; 
  }
  
  return command;
  
}

void setup(){
 
  // Setup pins
  pinMode(buttons[MANUAL], INPUT);
  pinMode(buttons[LEFT_BEACON], INPUT);
  pinMode(buttons[RIGHT_DEPLOY], INPUT);
  pinMode(buttons[HOIST], INPUT);

  pinMode(BEACON_LED_PIN, OUTPUT);
  pinMode(READY_LED_PIN, OUTPUT);
  //figure the ready pin should always be on, or maybe always on when networking is working
  digitalWrite(READY_LED_PIN, HIGH);
  
  link.set_id(MY_ADDR);
  
  
}

void loop(){ 
  byte command = CMD_NULL;
  command = link.check_packets();
  switch(command){
    //if no command, turn off ready
    case(CMD_NULL):
    case(CMD_BCN_DEP):
    case(CMD_BCN_REQ):
    case(CMD_MANUAL_LEFT):
    case(CMD_MANUAL_RIGHT):
    case(CMD_HOIST):
      // Nothing needs to be done for these
      //maybe blink an led to show receiving messages?
      break;
    case(CMD_BCN_START):
      enable_beaconing();
      break;
    case(CMD_BCN_STOP):
      disable_beaconing();
      break;
    default:
      // Nothing to do. Maybe flash LED to indicate error?
      break;
  }
  command = check_buttons();
  switch(command){
    case(CMD_NULL):
      // Nothing pressed, nothing to do
      break;
    case(CMD_HOIST):
      // The hoist button was pressed, send the packet
      link.send_packet(BLIMP_ADDR, CMD_HOIST);
      break;
    case(CMD_BCN_REQ):
      link.send_packet(BLIMP_ADDR, CMD_BCN_REQ);
      break;
    case(CMD_BCN_DEP):
      link.send_packet(BLIMP_ADDR, CMD_BCN_DEP);
      break;
      //here BCN_START and BCN_STOP tell the blimp to start and stop manual control
    case(CMD_BCN_START):
      link.send_packet(BLIMP_ADDR, CMD_BCN_START);  
      break;
    case(CMD_BCN_STOP):
      link.send_packet(BLIMP_ADDR, CMD_BCN_STOP);
      break;
      //manually go right or left
    case(CMD_MANUAL_RIGHT):
      link.send_packet(BLIMP_ADDR, CMD_MANUAL_RIGHT);
      break;
    case(CMD_MANUAL_LEFT):
      link.send_packet(BLIMP_ADDR, CMD_MANUAL_LEFT);
      break;
    default:
      // Do something on error?
      break;
  }
//  beacon();
}

#include <PacketLink.h>

// Pin Assignment Defines
#define SUMMON_BTN_PIN   2
#define DEPLOY_BTN_PIN   3
#define HOIST_BTN_PIN    4
#define SPEAKER_PIN      9
#define BEACON_LED_PIN  10
#define READY_LED_PIN   11


// Globals
boolean beaconing = false;
PacketLink link;

/* Beacons to blimp
 * Sends an audio signal to the blimp, flashes Tracking LED, sends RF packet to confirm
 * beaconing to other nodes (so only one beacons at a time)
 */
void beacon(){
  // Don't beacon if beaconing is disabled (duh)
  if(beaconing == false){
    return;
  }
}

/* Enable the beaconing function
 */
void enable_beaconing(){
  beaconing = true;
}

/* Disable the beaconing function
 */
void disable_beaconing(){
  beaconing = false;
}

/* Checks the buttons. Returns an appropriate command
 * @return The command to activate based on the pressed buttons
 */
byte check_buttons(){
  byte command = CMD_NULL;
  
  return command;
}

void setup(){
 
  // Setup pins
  pinMode(SUMMON_BTN_PIN, INPUT);
  pinMode(DEPLOY_BTN_PIN, INPUT);
  pinMode(HOIST_BTN_PIN, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(BEACON_LED_PIN, OUTPUT);
  pinMode(READY_LED_PIN, OUTPUT);
}

void loop(){ 
  byte command = CMD_NULL;
  command = link.check_packets();
  switch(command){
    case(CMD_NULL):
    case(CMD_PING):
    case(CMD_ECHO):
    case(CMD_BCN_REQ):
    case(CMD_HOIST):
      // Nothing needs to be done for these
      break;
    case(CMD_BCN_START):
      enable_beaconing();
      break;
    case(CMD_BCN_STOP):
      disable_beaconing();
      break;
    case(CMD_BCN_BEEP):
      // If someone else is beaconing, stop so multiple nodes aren't beaconing simultaneously
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
      link.send_packet(BROAD_ADDR, CMD_BCN_REQ);
      break;
    default:
      // Do something on error?
      break;
  }
  beacon();
}


// XBee Defines
#define CHANNEL 0x0D
#define BAUD 9600

// Pin Assignment Defines
#define SUMMON_BTN_PIN   2
#define DEPLOY_BTN_PIN   3
#define HOIST_BTN_PIN    4
#define SPEAKER_PIN      9
#define BEACON_LED_PIN  10
#define READY_LED_PIN   11

// Packet defines
/* Packet structure
  [0] To Node
  [1] From Node
  [2] Command
  [3] Null Terminator
  
  Node addresses:
  0: Blimp
  1: Remote 1
  2: Remote 2
  3: Remote 3
  255: Broadcast
  
  Commands (for both packets and buttons):
  0: None
  1: Ping/Test
  2: Ping Reply
  3: Request to beacon
  4: Begin beaconing
  5: Stop beaconing
  6: Beacon Beep (packet to say who is currently beaconing)
  10: Activate hoist
  
  */
#define PACKET_LEN 4
#define BLIMP_ADDR 0
#define BROAD_ADDR 255
#define CMD_NULL 0
#define CMD_PING 1
#define CMD_ECHO 2
#define CMD_BCN_REQ 3
#define CMD_BCN_START 4
#define CMD_BCN_STOP 5
#define CMD_BCN_BEEP 6
#define CMD_HOIST 10

// Globals
byte my_id = 0;
boolean beaconing = false;

/* Set the ID of the current remote
 * Use some method (button press, dip switches, EEPROM, etc) to get an ID from hardware
 * Set XBee ID, send messages from ID, and filter received messages
 * Modifies the global variable my_id;
 */
void set_id(){
  my_id = 0;
}

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

/* Sends a wireless packet
 */
void send_packet(byte target, byte command){
  char buffer[PACKET_LEN] = {target, my_id, command, 0};
  Serial.print(buffer);
}

/* Checks the RX buffer for any new bytes, and return when a complete packet has been read
 * @return A command value from the packet
 */
byte check_packets(){
  static char buffer[PACKET_LEN] = {0, 0, 0, 0};
  static int bytes_read = 0;
  byte command;
  
  return command;
}

/* Checks the buttons. Returns an appropriate command
 * @return The command to activate based on the pressed buttons
 */
byte check_buttons(){
  byte command = 0;
  return command;
}

void setup(){
  Serial.begin(BAUD);
  
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
  command = check_packets();
  switch(command){
    case(CMD_NULL):
      break;
  }
  command = check_buttons();
  switch(command){
  }
  beacon();
}

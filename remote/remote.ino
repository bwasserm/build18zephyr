
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
/* Packet structure (one byte bitfield)
  [7] 0 (So the result is a 7-bit ASCII character)
  [6-5] To Addr
  [4-3] From Addr
  [2-0] Command
  
  Node addresses:
  0: Blimp
  1: Remote 1
  2: Remote 2
  3: Remote 3
  If To Addr == From Addr, treat as broadcast message
  
  Commands (for both packets and buttons):
  0: None
  1: Ping/Test
  2: Ping Reply
  3: Request to beacon
  4: Begin beaconing
  5: Stop beaconing
  6: Beacon Beep (packet to say who is currently beaconing)
  7: Activate hoist
  
  */
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
  char packet = '\0';
  
  if(target == BROAD_ADDR){
    target = my_id;
  }
  
  packet = ((target &  0x3) << 5) |
           ((my_id  &  0x3) << 3) |
           (command &  0x7);
  Serial.write(packet);
}

/* Checks the RX buffer for any new bytes, and return when a complete packet has been read
 * @return A command value from the packet
 */
byte check_packets(){
  byte packet;
  int bytes_read = 0;
  int to_addr = -1;
  int from_addr = -1;
  int command = 0;
  
  // Check for new byte
  if(Serial.available() > 0){
    // Read next byte in buffer
    bytes_read = Serial.readBytes((char *)&packet, 1);
    
    // Parse the packet
    to_addr =   (packet >> 5) & 0x3;
    from_addr = (packet >> 3) & 0x3;
    command =   packet & 0x7;
    
    // Only accept packets addressed to the current node, or broadcast
    if(to_addr == my_id || to_addr == from_addr){
      // Reply to pings here, since the source address isn't returned
      if(command == CMD_PING){
        send_packet(from_addr, CMD_ECHO);
      }
      
      // Return the received command
      return command;
    }
  }
  
  // There wasn't a command received, so return the null command
  return CMD_NULL;
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
      send_packet(BLIMP_ADDR, CMD_HOIST);
      break;
    case(CMD_BCN_REQ):
      send_packet(BROAD_ADDR, CMD_BCN_REQ);
      break;
    default:
      // Do something on error?
      break;
  }
  beacon();
}

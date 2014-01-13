// XBee Defines
#define CHANNEL 0x0D
#define BAUD 9600
// The blimp is ID 0
#define MY_ID 0

// Pin Assignment Defines
#define HOIST_SERVO_PIN  9
#define LEFT_MOTOR_PIN  10
#define RIGHT_MOTOR_PIN 11
#define MIC_L_PIN       A0
#define MIC_C_PIN       A1
#define MIC_R_PIN       A2

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
#define CMD_HOIST 7

// Hoist positions
#define HOIST_UP 0
#define HOIST_DOWN 1
#define HOIST_RAISING 2
#define HOIST_LOWERING 3

// Globals
int target = 0;
int hoist_position = HOIST_UP;

/* Sends a wireless packet
 */
void send_packet(byte target, byte command){
  char packet = '\0';
  
  if(target == BROAD_ADDR){
    target = MY_ID;
  }
  
  packet = ((target &  0x3) << 5) |
           ((MY_ID  &  0x3) << 3) |
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
    if(to_addr == MY_ID || to_addr == from_addr){
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

void  setup(){
}

void loop(){
}

#include "Arduino.h"
#include "PacketLink.h"

PacketLink::PacketLink(){
	
	// Set channel
	// TODO
}

/* Returns the last id that sent a packet
 */
byte PacketLink::get_last_sender(){

  return last_sender;
}

void PacketLink::set_id(byte id){
	// ID can only be two bits long
	my_id = id & 0x3;
        last_sender = id & 0x3;
}

/* Sends a wireless packet
 */
void PacketLink::send_packet(byte target, byte command){
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
byte PacketLink::check_packets(){
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

      // Update the node that transmitted last
      last_sender = from_addr;

      // Reply to pings here, since the source address isn't returned
      //    if(command == CMD_PING){
      //   send_packet(from_addr, CMD_NULL);
      // }
      last_sender = (packet >> 3) & 0x3;
      // Return the received command
      return command;
    }
  }
  
  // There wasn't a command received, so return the null command
  return CMD_NULL;
}

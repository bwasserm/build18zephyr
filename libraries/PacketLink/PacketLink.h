/* PacketLink.h
 * All packet operations for Project Zephyr, for Build18 2014
 */
 
#ifndef PACKET_LINK_H
#define PACKET_LINK_H
 
#include "Arduino.h"

// XBee Defines
#define CHANNEL 0x0D
#define BAUD 9600

// Packet Defines
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
#define BLIMP_ADDR 1
#define REM1_ADDR 2
#define REM2_ADDR 3
#define BROAD_ADDR 255
#define CMD_NULL 0
#define CMD_PING 1
#define CMD_ECHO 2
#define CMD_BCN_REQ 3
#define CMD_BCN_START 4
#define CMD_BCN_STOP 5
#define CMD_BCN_BEEP 6
#define CMD_HOIST 7
// THERE CAN BE ONLY EIGHT
 
class PacketLink{
	public:
		byte my_id;
		PacketLink();
		void set_id(byte id);
		void send_packet(byte target, byte command);
		byte check_packets();
	private:
		
};
 
#endif // PACKET_LINK_H
 

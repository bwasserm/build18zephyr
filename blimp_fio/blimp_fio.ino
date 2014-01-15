#include <SPI.h>
#include <PacketLink.h>

// The blimp is ID 0
#define MY_ID 0

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

// Globals
int target = 0;
int hoist_position = HOIST_UP;
PacketLink link();

// SPI globals
byte spi_val = 0;
volatile boolean spi_available;

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  spi_val = SPDR;  // grab byte from SPI Data Register
  
  spi_available = true;
}  // end of interrupt routine SPI_STC_vect


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

}

void loop(){
  // Check SPI buffer
  if(spi_available){
    Serial.println(spi_val);
    spi_available = false;
  }  // end of flag set
}

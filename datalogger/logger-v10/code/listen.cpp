#include <RFM69.h>
#include <RFM69registers.h>
#include <listen.h>
#include <SPI.h>

static volatile uint8_t receivedSize = 0;
static RFM69* pRadio;
static uint8_t maxSize;
static byte* buffer;


static void irq() 
{
	if( receivedSize != 0 ) return;
	
	noInterrupts();
	
    pRadio->select();
    SPI.transfer(REG_FIFO & 0x7F);
    uint8_t len = SPI.transfer(0);
    uint8_t node = SPI.transfer(0);
	if( len-- > 1 ) {
		len = len > maxSize ? maxSize : len; // precaution
		for( uint8_t i = 0; i < len; i++ ) {
			uint8_t b = SPI.transfer( 0 );
			buffer[i] = b;
		}
	}
	
	pRadio->unselect();
	interrupts();
	
	receivedSize = len;
}


void RFM69Listener::startListening( RFM69& radio, void* buf, uint8_t size )
{
	maxSize = size;
	buffer = (byte*)buf;

	pRadio = &radio;
	
	receivedSize = 0;
	detachInterrupt( RF69_IRQ_NUM );
	attachInterrupt( RF69_IRQ_NUM, irq, RISING);

	radio.writeReg( REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01 );
	radio.writeReg( REG_NODEADRS, radio.getAddress() );
	
	radio.writeReg( REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_OFF | RF_PACKET2_AES_OFF );

	radio.writeReg( REG_LISTEN1, RF_LISTEN1_RESOL_RX_64 | RF_LISTEN1_RESOL_IDLE_262000 |
					RF_LISTEN1_CRITERIA_RSSI | RF_LISTEN1_END_10 );
	radio.writeReg( REG_LISTEN2, 10 );
	radio.writeReg( REG_LISTEN3, 8 );
    radio.writeReg( REG_PACKETCONFIG1,
					RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON |
					RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODE );
    radio.writeReg( REG_RSSITHRESH, 200 );
	radio.writeReg( REG_RXTIMEOUT2, 75 );
	radio.writeReg( REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_STANDBY );
	radio.writeReg( REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_ON  );
}

uint8_t RFM69Listener::receivedBytes()
{
	return receivedSize;
}

void RFM69Listener::clearBuffer()
{
	receivedSize = 0;
}


void RFM69Listener::endListening( RFM69& radio )
{
 	radio.writeReg( REG_RXTIMEOUT2, 0 );
	radio.writeReg( REG_OPMODE, RF_OPMODE_LISTENABORT | RF_OPMODE_STANDBY );
	radio.writeReg( REG_OPMODE, RF_OPMODE_STANDBY );
	radio.writeReg( REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_STANDBY );
	radio.setMode( RF69_MODE_STANDBY );
}

void RFM69Listener::send( RFM69& radio, uint8_t targetNode, void* buffer, uint8_t size )
{
	detachInterrupt( RF69_IRQ_NUM );
    radio.writeReg( REG_PACKETCONFIG1,
					RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON |
					RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODE );
	radio.encrypt( 0 );
	
	uint32_t time = millis();
	radio.setMode( RF69_MODE_TX );
	while( millis() - time < 3000 ) {

		noInterrupts();
		// write to FIFO
		radio.select();
		SPI.transfer(REG_FIFO | 0x80);
		SPI.transfer( size + 1);
		SPI.transfer( targetNode );

		for (uint8_t i = 0; i < size; i++)
		  	SPI.transfer(((uint8_t*) buffer)[i]);
		radio.unselect();
		interrupts();
		delayMicroseconds( 5000 );
		
		
	}
	radio.setMode( RF69_MODE_STANDBY );
}





class RFM69;

class RFM69Listener
{
public:
	static void startListening( RFM69& radio, void* buffer, uint8_t size );
	static void endListening( RFM69& radio );
	static uint8_t receivedBytes();
	static void clearBuffer();
	static void send( RFM69& radio, uint8_t targetNode, void* buffer, uint8_t size );
};



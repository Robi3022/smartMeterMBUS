
#ifndef ENCRYPT_H_
#define ENCRYPT_H_



#include <stdint.h>
#include <vector>
#include "mbedtls/gcm.h"


namespace MBus
{
	class DlmsMeter
	{
		public:
			DlmsMeter(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);
			int processData();

		private:
			const std::vector<uint8_t>& receiveBuffer; // Stores the packet currently being received
			const std::vector<uint8_t>& key; // Stores the decryption key
			mbedtls_gcm_context aes = {0}; // AES context used for decryption

			uint16_t swap_uint16(uint16_t val);
			uint32_t swap_uint32(uint32_t val);
			uint16_t getTabIndexByObisCode(uint8_t* code, uint8_t len);
			int getMBusPayload(std::vector<uint8_t> & mbusPayload);
			int decryptData(std::vector<uint8_t> &mbusPayload, uint8_t* &plaintext, uint16_t &messageLength );
			int printOutData(uint8_t* plaintext, uint16_t messageLength);
			int printDoubleLong(uint8_t* plaintext, uint8_t index);
			int printLong(uint8_t* plaintext, uint8_t index);
			int printString(uint8_t* plaintext, uint8_t index);
	};
}



#endif

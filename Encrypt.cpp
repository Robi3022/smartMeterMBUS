
#include "Encrypt.h"

#include <cstring>
#include <iostream>
#include <vector>
#include "MBusDefines.h"
#include "ObisCodes.h"



namespace MBus
{

	DlmsMeter::DlmsMeter(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) : receiveBuffer(data), key(key) {
		/* nothing */
	}

	int DlmsMeter::processData()
	{

		if(!this->receiveBuffer.empty())
		{
			// read payload from MBUS Frame
			std::vector<uint8_t> mbusPayload;
			if (getMBusPayload(mbusPayload) != 0)
			{
				return -1;
			}

			/* decrypt header */
			uint8_t* plaintext = NULL;
			uint16_t messageLength = 0;
			if (decryptData(mbusPayload, plaintext, messageLength) != 0)
			{
				return -2;
			}
			if (plaintext == NULL || messageLength == 0)
			{
				return -3;
			}

			if (printOutData(plaintext, messageLength) != 0)
			{
				return -4;
			}

		}
		return 0;
	}


	int DlmsMeter::printOutData(uint8_t* plaintext, uint16_t messageLength)
	{
		uint8_t* endPtr = plaintext + messageLength;
		bool first = true;

		printf("\"meterData\": {\r\n");

		int currentPosition = DECODER_START_OFFSET;
		do
		{
			if(plaintext[currentPosition + OBIS_TYPE_OFFSET] != DataType::OctetString)
			{
				printf("Error: OBIS: Unsupported OBIS header type");
				return -1;
			}

			uint8_t obisCodeLength = plaintext[currentPosition + OBIS_LENGTH_OFFSET];

			if(obisCodeLength != 0x06)
			{
				printf("OBIS: Unsupported OBIS header length");
				return -1;
			}

			uint8_t obisCode[obisCodeLength];
			memcpy(&obisCode[0], &plaintext[currentPosition + OBIS_CODE_OFFSET], obisCodeLength); // Copy OBIS code to array

			currentPosition += obisCodeLength + 2; // Advance past code, position and type


			uint16_t index = getTabIndexByObisCode(&obisCode[0], obisCodeLength);

			// Compare A C and D against code table entries
			if(index >= DECODE_TAB_LEN)
			{
				printf("OBIS: Unsupported OBIS code");
			}
			else
			{
				uint8_t dataType = plaintext[currentPosition];
				currentPosition++; // Advance past data type

				uint8_t dataLength = 0x00;


				if (!first)
				{
					printf(",\r\n");
				}
				first = false;

				switch(dataType)
				{
					case DataType::DoubleLongUnsigned:	dataLength = printDoubleLong(&plaintext[currentPosition], index);  break;
					case DataType::LongUnsigned: dataLength = printLong(&plaintext[currentPosition], index); break;
					case DataType::OctetString: dataLength = printString(&plaintext[currentPosition], index); break;
					default:	printf("OBIS: Unsupported OBIS data type"); return -1;
				}
				currentPosition += dataLength; 	// Skip data length
				currentPosition += 2; 			// Skip break after data
			}

			if(plaintext[currentPosition] == 0x0F) // There is still additional data for this type, skip it
				currentPosition += 6; // Skip additional data and additional break; this will jump out of bounds on last frame
		}
		while (currentPosition <= messageLength); // Loop until arrived at end


		printf("\r\n}");
		return 0;
	}


	int DlmsMeter::printDoubleLong(uint8_t* plaintext, uint8_t index)
	{
		uint32_t uint32Value;
		float floatValue;

		memcpy(&uint32Value, plaintext, 4); // Copy bytes to integer
		uint32Value = swap_uint32(uint32Value); // Swap bytes
		floatValue = uint32Value; // Ignore decimal digits for now

		printf("\t\"%s\": %f", decodeSettings[index].strOut, floatValue);

		return 4;
	}

	int DlmsMeter::printLong(uint8_t* plaintext, uint8_t index)
	{
		uint16_t uint16Value;
		float floatValue;

		memcpy(&uint16Value, plaintext, 2); // Copy bytes to integer
		uint16Value = swap_uint16(uint16Value); // Swap bytes

		if(plaintext[5] == Accuracy::SingleDigit)
			floatValue = uint16Value / 10.0; // Divide by 10 to get decimal places
		else if(plaintext[5] == Accuracy::DoubleDigit)
			floatValue = uint16Value / 100.0; // Divide by 100 to get decimal places
		else
			floatValue = uint16Value; // No decimal places


		printf("\t\"%s\": %f", decodeSettings[index].strOut, floatValue);
		return 2;
	}


	int DlmsMeter::printString(uint8_t* plaintext, uint8_t index)
	{
		int dataLength = 0;
		uint16_t uint16Value;
		if (decodeSettings[index].pType == printType::ptTime)
		{
			dataLength = plaintext[0];
			plaintext++; // Advance past string length

			char timestamp[21]; // 0000-00-00T00:00:00Z

			uint16_t year;
			uint8_t month;
			uint8_t day;

			uint8_t hour;
			uint8_t minute;
			uint8_t second;

			memcpy(&uint16Value, plaintext, 2);
			plaintext += 2;
			year = swap_uint16(uint16Value);

			memcpy(&month, plaintext, 1);
			plaintext++;
			memcpy(&day, plaintext, 1);
			plaintext++;

			memcpy(&hour, plaintext, 1);
			plaintext++;
			memcpy(&minute, plaintext, 1);
			plaintext++;
			memcpy(&second, plaintext, 1);
			plaintext++;

			sprintf(timestamp, "%04u-%02u-%02uT%02u:%02u:%02uZ", year, month, day, hour, minute, second);
			printf("\t\"%s\": \"%s\"", decodeSettings[index].strOut, timestamp);

		}
		else
		{
			dataLength = plaintext[0];
			plaintext++;
			char text[100] = {0};
			memcpy(text, plaintext, dataLength);
			printf("\t\"%s\": \"%s\"", decodeSettings[index].strOut, text);

		}
		return dataLength + 1;
	}



	int DlmsMeter::getMBusPayload(std::vector<uint8_t> & mbusPayload)
	{
		uint16_t frameOffset = 0; // Offset is used if the M-Bus message is split into multiple frames

		while(true)
		{
			// Check start bytes
			if(this->receiveBuffer[frameOffset + MBUS_START1_OFFSET] != 0x68 || this->receiveBuffer[frameOffset + MBUS_START2_OFFSET] != 0x68)
			{
				printf("Error: MBUS: Start bytes do not match \n");
				return -1;
			}

			// Both length bytes must be identical
			if(this->receiveBuffer[frameOffset + MBUS_LENGTH1_OFFSET] != this->receiveBuffer[frameOffset + MBUS_LENGTH2_OFFSET])
			{
				printf("Error: MBUS: Length bytes do not match \n");
				return -1;
			}

			uint8_t frameLength = this->receiveBuffer[frameOffset + MBUS_LENGTH1_OFFSET]; // Get length of this frame

			// Check if received data is enough for the given frame length
			if(this->receiveBuffer.size() - frameOffset < frameLength + 3)
			{
				printf("Error: MBUS: Frame too big for received data \n");
				return -1;
			}

			if(this->receiveBuffer[frameOffset + frameLength + MBUS_HEADER_INTRO_LENGTH + MBUS_FOOTER_LENGTH - 1] != 0x16)
			{
				printf("Error: MBUS: Invalid stop byte \n");
				return -1;
			}

			mbusPayload.insert(mbusPayload.end(), &this->receiveBuffer[frameOffset + MBUS_FULL_HEADER_LENGTH], &this->receiveBuffer[frameOffset + MBUS_HEADER_INTRO_LENGTH + frameLength]);

			frameOffset += MBUS_HEADER_INTRO_LENGTH + frameLength + MBUS_FOOTER_LENGTH;

			if(frameOffset >= this->receiveBuffer.size()) // No more data to read, exit loop
			{
				break;
			}
		}
		return 0;
	}

	int DlmsMeter::decryptData(std::vector<uint8_t> &mbusPayload, uint8_t* &plaintext, uint16_t &messageLength )
	{

		// Verify and parse DLMS header
		if(mbusPayload.size() < 20) // If the payload is too short we need to abort
		{
			printf("DLMS: Payload too short \n");
			return -1;
		}

		if(mbusPayload[DLMS_CIPHER_OFFSET] != 0xDB) // Only general-glo-ciphering is supported (0xDB)
		{
			printf("DLMS: Unsupported cipher \n");
			return -1;
		}

		uint8_t systitleLength = mbusPayload[DLMS_SYST_OFFSET];

		if(systitleLength != 0x08) // Only system titles with length of 8 are supported
		{
			printf("Error: DLMS: Unsupported system title length \n");
			return -1;
		}

		messageLength = mbusPayload[DLMS_LENGTH_OFFSET];
		int headerOffset = 0;

		if(messageLength == 0x82)
		{
			memcpy(&messageLength, &mbusPayload[DLMS_LENGTH_OFFSET + 1], 2);
			messageLength = swap_uint16(messageLength);

			headerOffset = DLMS_HEADER_EXT_OFFSET; // Header is now 2 bytes longer due to length > 127
		}
		messageLength -= DLMS_LENGTH_CORRECTION; // Correct message length due to part of header being included in length

		if(mbusPayload.size() - DLMS_HEADER_LENGTH - headerOffset != messageLength)
		{
			printf("Error: DLMS: Message has invalid length");
			return -1;
		}

		if(mbusPayload[headerOffset + DLMS_SECBYTE_OFFSET] != 0x21) // Only certain security suite is supported (0x21)
		{
			printf("DLMS: Unsupported security control byte");
			return -1;
		}

		// Decryption

		uint8_t iv[12]; // Reserve space for the IV, always 12 bytes
		// Copy system title to IV (System title is before length; no header offset needed!)
		// Add 1 to the offset in order to skip the system title length byte
		memcpy(&iv[0], &mbusPayload[DLMS_SYST_OFFSET + 1], systitleLength);
		memcpy(&iv[8], &mbusPayload[headerOffset + DLMS_FRAMECOUNTER_OFFSET], DLMS_FRAMECOUNTER_LENGTH); // Copy frame counter to IV

		plaintext = (uint8_t*)malloc(messageLength);
		if (plaintext == NULL)
		{
			printf("Memory alloc error \n");
			return -1;
		}

		mbedtls_gcm_init(&this->aes);
		mbedtls_gcm_setkey(&this->aes, MBEDTLS_CIPHER_ID_AES, this->key.data(), this->key.size() * 8);

		mbedtls_gcm_auth_decrypt(&this->aes, messageLength, iv, sizeof(iv), NULL, 0, NULL, 0, &mbusPayload[headerOffset + DLMS_PAYLOAD_OFFSET], plaintext);

		mbedtls_gcm_free(&this->aes);

		if(plaintext[0] != 0x0F || plaintext[5] != 0x0C)
		{
			printf("Error: OBIS: Packet was decrypted but data is invalid");
			return -1;
		}

		return 0;

	}


	uint16_t DlmsMeter::getTabIndexByObisCode(uint8_t* code, uint8_t len)
	{
		for (uint16_t i = 0U; i < DECODE_TAB_LEN; i++)
		{
			if(memcmp(&code[0], &decodeSettings[i].obisCode, OBIS_CODE_LEN) == 0)
			{
				return i;
			}
		}
		return DECODE_TAB_LEN;
	}


	uint16_t DlmsMeter::swap_uint16(uint16_t val)
	{
		return (val << 8) | (val >> 8);
	}

	uint32_t DlmsMeter::swap_uint32(uint32_t val)
	{
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
		return (val << 16) | (val >> 16);
	}


}

#include <iostream>
#include <cstdio>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif
#include "mbedtls/gcm.h"
#ifdef __cplusplus
}
#endif



#include "Encrypt.h"

using namespace MBus;



class MBusPackage
{
	public:
		MBusPackage(const std::vector<uint8_t>* dataIn);

		const std::vector<uint8_t>* getNextPackage();
		int getMsgReadCnt();

	private:
		const std::vector<uint8_t>& data;
		const uint8_t* pEnd = NULL;
};



MBusPackage::MBusPackage(const std::vector<uint8_t>* dataIn) : data(*dataIn) {
	/* nothing */
};

int MBusPackage::getMsgReadCnt()
{
	if (pEnd != NULL)
	{
		return (int)((int64_t)pEnd - (int64_t)&data[0]);
	}
	return 0;
}

const std::vector<uint8_t>* MBusPackage::getNextPackage()
{
	uint32_t len = data.size();
	const uint8_t* start = NULL;
	const uint8_t* end = NULL;
	for (uint32_t i = 0U; i < (len - 3U); i++)
	{
		if (data[i] == data[i + 3u] && data[i] == 0x68U && data[i + 1u] == 0xFA && (i + data[i+1] + 5U) <= len)
		{
			if (data[i + data[i+1] + 5U] == 0x16)
			{
				if (start == NULL)
				{
					start = &data[i];
				}
			}
		}


		if (data[i] == data[i + 3u] && data[i] == 0x68U && data[i + 1u] != 0xFA && (i + data[i+1] + 5U) <= len)
		{
			if (data[i + data[i+1] + 5U] == 0x16 && start != NULL && end == NULL)
			{
				end = &data[i + data[i+1] + 5U];
			}
		}
	}

	if (start != NULL && end != NULL)
	{
		// valid String found
		std::vector<uint8_t>* s = new std::vector<uint8_t>(start, end+1);
		pEnd = end+1;
		return s;
	}

	return NULL;
}



static std::vector<uint8_t>* HexToBytes(char* hex) {
	std::vector<uint8_t>* bytes = new std::vector<uint8_t>;
	for (unsigned int i = 0; i < strlen(hex); i += 2) {
		uint8_t byte;
		sscanf(hex + i, "%2hhx", &byte);
		bytes->push_back(byte);
	}
	return bytes;
}



int main(int argc, char **argv)
{
	/* check if call argument has been given */
	int retCode = 0;
	if (argc >= 3 && strlen(argv[1]) > 2 && strlen(argv[2]) > 2)
	{

		MBusPackage data(HexToBytes(argv[1]));

		const std::vector<uint8_t>* d = data.getNextPackage();
		if (d == NULL)
		{
			printf("Erro: No MBUS Package found\r\n");
			return -1;
		}

		const std::vector<uint8_t>* k = HexToBytes(argv[2]);
		DlmsMeter out(*d, *k);
		retCode = out.loop();

		// positive ret-code, no error
		if (retCode >= 0)
		{
			printf(",\r\n\"BytesRead\": %u\r\n", data.getMsgReadCnt());
		}

	}

	return 0;
}

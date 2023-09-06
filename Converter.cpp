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


#include "MBusPackage.h"
#include "Encrypt.h"

using namespace MBus;


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
		retCode = out.processData();

		// positive ret-code, no error
		if (retCode >= 0)
		{
			printf(",\r\n\"BytesRead\": %u\r\n", data.getMsgReadCnt());
		}

	}

	return 0;
}

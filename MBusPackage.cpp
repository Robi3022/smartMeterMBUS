#include "MBusPackage.h"


namespace MBus {



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


};

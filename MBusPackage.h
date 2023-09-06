/*
 * MBusPackage.h
 *
 *  Created on: 05.09.2023
 *      Author: robert
 */

#ifndef MBUSPACKAGE_H_
#define MBUSPACKAGE_H_

#include "MBusPackage.h"
#include <stdint.h>
#include <vector>
#include <cstring>


namespace MBus
{

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

}

#endif /* MBUSPACKAGE_H_ */

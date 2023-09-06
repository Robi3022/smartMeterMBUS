

#ifndef OBISCODES_H_
#define OBISCODES_H_



#define DECODER_START_OFFSET  	20 		// Offset for start of OBIS decoding, skip header, timestamp and break block
#define OBIS_TYPE_OFFSET 		0
#define OBIS_LENGTH_OFFSET 		1
#define OBIS_CODE_OFFSET 		2
#define OBIS_CODE_LEN 			6



enum DataType
{
    NullData = 0x00,
    Boolean = 0x03,
    BitString = 0x04,
    DoubleLong = 0x05,
    DoubleLongUnsigned = 0x06,
    OctetString = 0x09,
    VisibleString = 0x0A,
    Utf8String = 0x0C,
    BinaryCodedDecimal = 0x0D,
    Integer = 0x0F,
    Long = 0x10,
    Unsigned = 0x11,
    LongUnsigned = 0x12,
    Long64 = 0x14,
    Long64Unsigned = 0x15,
    Enum = 0x16,
    Float32 = 0x17,
    Float64 = 0x18,
    DateTime = 0x19,
    Date = 0x1A,
    Time = 0x1B,

    Array = 0x01,
    Structure = 0x02,
    CompactArray = 0x13
};


enum Accuracy
{
    SingleDigit = 0xFF,
    DoubleDigit = 0xFE
};

typedef enum printType
{
	ptDigit,
	ptTime,
	ptString,
} printType_e;

typedef struct
{
	uint8_t obisCode[OBIS_CODE_LEN];
	const char* strOut;
	printType_e pType;
} decodeData_t;


/*
 * Decode Data table
 */
const decodeData_t decodeSettings[] = {
		{{0x00,	0x00, 0x01, 0x00, 0x00, 0xFF},  "Timestamp"					,ptTime},
		{{0x00, 0x00, 0x60, 0x01, 0x00, 0xFF},  "Serial-Number"				,ptString},
		{{0x00, 0x00, 0x2A, 0x00, 0x00, 0xFF},  "Device-Name" 				,ptString},
		{{0x01, 0x00, 0x20, 0x07, 0x00, 0xFF},  "VoltageL1"					,ptDigit},
		{{0x01, 0x00, 0x34, 0x07, 0x00, 0xFF},  "VoltageL2"					,ptDigit},
		{{0x01, 0x00, 0x48, 0x07, 0x00, 0xFF},  "VoltageL3"					,ptDigit},
		{{0x01, 0x00, 0x1F, 0x07, 0x00, 0xFF},  "CurrentL1"					,ptDigit},
		{{0x01, 0x00, 0x33, 0x07, 0x00, 0xFF},  "CurrentL2"		 			,ptDigit},
		{{0x01, 0x00, 0x47, 0x07, 0x00, 0xFF},  "CurrentL3"		 			,ptDigit},
		{{0x01, 0x00, 0x01, 0x07, 0x00, 0xFF},  "ActivePowerPlus" 			,ptDigit},
		{{0x01, 0x00, 0x02, 0x07, 0x00, 0xFF},  "ActivePowerMinus"			,ptDigit},
		{{0x01, 0x00, 0x01, 0x08, 0x00, 0xFF},  "ActiveEnergyPlus"			,ptDigit},
		{{0x01, 0x00, 0x02, 0x08, 0x00, 0xFF},  "ActiveEnergyMinus"			,ptDigit},
		{{0x01, 0x00, 0x03, 0x08, 0x00, 0xFF},  "ReactiveEnergyPlus"		,ptDigit},
		{{0x01, 0x00, 0x04, 0x08, 0x00, 0xFF},  "ReactiveEnergyMinus"		,ptDigit}
};
#define DECODE_TAB_LEN 		(sizeof(decodeSettings) / sizeof(decodeData_t))


#endif



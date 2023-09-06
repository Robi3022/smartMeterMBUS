
#ifndef MBUSDEFINES_H_
#define MBUSDEFINES_H_


#define MBUS_START1_OFFSET 			0 // Offset of first start byte
#define MBUS_LENGTH1_OFFSET 		1 // Offset of first length byte
#define MBUS_LENGTH2_OFFSET 		2 // Offset of (duplicated) second length byte
#define MBUS_START2_OFFSET 			3 // Offset of (duplicated) second start byte
#define MBUS_HEADER_INTRO_LENGTH	4 // Header length for the intro (0x68, length, length, 0x68)
#define MBUS_FULL_HEADER_LENGTH		9 // Total header length
#define MBUS_FOOTER_LENGTH			2 // Footer after frame
#define MBUS_MAX_FRAME_LENGTH		250 // Maximum size of frame


#define DLMS_HEADER_LENGTH			16 // Length of the header (total message length <= 127)
#define DLMS_HEADER_EXT_OFFSET		2 // Length to offset when header is extended length (total message length > 127)
#define DLMS_CIPHER_OFFSET			0 // Offset at which used cipher suite is stored
#define DLMS_SYST_OFFSET			1 // Offset at which length of system title is stored
#define DLMS_LENGTH_OFFSET			10 // Offset at which message length is stored
#define DLMS_LENGTH_CORRECTION		5 // Part of the header is included in the DLMS length field and needs to be removed
#define DLMS_SECBYTE_OFFSET			11 // Offset of the security byte
#define DLMS_FRAMECOUNTER_OFFSET	12 // Offset of the frame counter
#define DLMS_FRAMECOUNTER_LENGTH	4 // Length of the frame counter (always 4)
#define DLMS_PAYLOAD_OFFSET			16 // Offset at which the encrypted payload begins






#endif

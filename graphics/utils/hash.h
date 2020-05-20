#include "types/types.h"

namespace hash{

	inline uint8 crc8(uint8* data, uint32 length){
		uint8 crc = 0xff;
		const uint8 poly = 0xe5;

		for(uint32 i = 0; i < length; ++i){
			uint8 val = (crc ^ data[i]) & 0xff;
			for(int b = 0; b < 8; ++b)
				val = val & 1 ? (val >> 1) ^ poly : val >> 1;
			crc = crc ^ val;
		}
		crc = crc ^ 0xff;
		return crc;
	}

	inline uint16 crc16(uint8* data, uint32 length){
		uint16 crc = 0xffff;
		const uint16 poly = 0xd405;

		for(uint32 i = 0; i < length; ++i){
			uint16 val = (crc ^ data[i]) & 0xff;
			for(int b = 0; b < 8; ++b)
				val = val & 1 ? (val >> 1) ^ poly : val >> 1;
			crc = (crc >> 8) ^ val;
		}
		crc = crc ^ 0xffff;
		return crc;
	}

	inline uint32 crc32(uint8* data, uint32 length){
		uint32 crc = 0xffffffff;
		const uint32 poly = 0xedb88320;

		for(uint32 i = 0; i < length; ++i){
			uint32 val = (crc ^ data[i]) & 0xff;
			for(int b = 0; b < 8; ++b)
				val = val & 1 ? (val >> 1) ^ poly : val >> 1;
			crc = (crc >> 8) ^ val;
		}
		crc = crc ^ 0xffffffff;
		return crc;
	}

	inline uint64 crc64(uint8* data, uint64 length){
		uint64 crc = 0xffffffffffffffff;
		const uint64 poly = 0xc96c5795d7870f42;

		for(uint64 i = 0; i < length; ++i){
			uint64 val = (crc ^ data[i]) & 0xff;
			for(int b = 0; b < 8; ++b)
				val = val & 1 ? (val >> 1) ^ poly : val >> 1;
			crc = (crc >> 8) ^ val;
		}
		crc = crc ^ 0xffffffffffffffff;
		return crc;
	}

};
#pragma once
#include <cstdint>
#include <QByteArray>
#include <QVector>

class C_crc16
{
		static uint16_t st_MakeCRC16_xxxstream(uint16_t& wcrc,const void* buf,uint32_t dwdLen,const uint16_t wPolynom16);

public:
		static void st_MakeCRC16_initStream(uint16_t& wcrc);
		static void st_MakeCRC16_add_Stream(uint16_t& wcrc,const QByteArray& vec,const uint16_t wPolynom16=st_MakeCRC16_defPoly1021);
		static void st_MakeCRC16_add_Stream(uint16_t& wcrc,const QVector<uint8_t>& vec,const uint16_t wPolynom16=st_MakeCRC16_defPoly1021);
		static void st_MakeCRC16_add_Stream(uint16_t& wcrc,const std::string& str,const uint16_t wPolynom16=st_MakeCRC16_defPoly1021);
		static void st_MakeCRC16_add_Stream(uint16_t& wcrc,const void* buf,uint32_t dwdLen,const uint16_t wPolynom16=st_MakeCRC16_defPoly1021);

		enum{st_MakeCRC16_defPoly1021=0x1021};

		static uint16_t st_MakeCRC16_singleCalc(const QByteArray& vec);
		static uint16_t st_MakeCRC16_singleCalc(const QVector<uint8_t>& vec);
		static uint16_t st_MakeCRC16_singleCalc(const std::string& str);
		static uint16_t st_MakeCRC16_singleCalc(const void* buf,uint32_t dwdLen);
};


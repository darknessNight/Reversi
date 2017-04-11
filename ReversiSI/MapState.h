#pragma once
#include <memory>

namespace SI::Reversi
{
	class MapState
	{
	public:
		static const auto rowsCount = 8u;
		static const auto colsCount = 8u;
	private:
		static const auto bitsPerField = 2u;
		static const auto bitsPerByte = 8u;
		static const auto bytesCount = rowsCount*colsCount*bitsPerField/bitsPerByte;
		unsigned char bytes[bytesCount];
	public:
		enum State
		{
			Empty,
			Player1,
			Player2,
			Unknow
		};
	public:
		explicit MapState(char bytes[bytesCount])
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		explicit MapState(int* bytes)
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		MapState(const MapState& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapState(const MapState&& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapState()
		{
			memset(bytes, 0, bytesCount);
		}
		MapState& operator=(const MapState& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapState& operator=(const MapState&& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}

		State GetFieldState(unsigned x, unsigned y)
		{
			auto flatIndex=x*rowsCount + y;
			auto byteIndex = flatIndex*bitsPerField / bitsPerByte;
			auto inByteIndex = flatIndex*bitsPerField - byteIndex*bitsPerByte;
			auto shift = (bitsPerByte - inByteIndex - bitsPerField);

			return (State)((bytes[byteIndex] >> shift) & 0b11);
		}

		void SetFieldState(unsigned x, unsigned y, State newState)
		{
			auto flatIndex = x*rowsCount + y;
			auto byteIndex = flatIndex*bitsPerField / bitsPerByte;
			auto inByteIndex = flatIndex*bitsPerField - byteIndex*bitsPerByte;
			auto shift = (bitsPerByte - inByteIndex - bitsPerField);

			auto mask = (~0b11)<<shift;
			auto value = ((int)newState) << shift;
			bytes[byteIndex] = bytes[byteIndex] & mask;
			bytes[byteIndex] = bytes[byteIndex] | value;
		}
	};
}
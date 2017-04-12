#pragma once
#include <memory>

namespace SI::Reversi
{
	class MapStateMemoryOptimized
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
		{					//in case we want mooooore optimization and fucking around with bit operations	
			Empty,			// = 0b00
			Player1,		// = 0b10
			Player2,		// = 0b11
			Unknow			// = 0b01
		};
	public:
		explicit MapStateMemoryOptimized(char bytes[bytesCount])
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		explicit MapStateMemoryOptimized(int* bytes)
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		MapStateMemoryOptimized(const MapStateMemoryOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapStateMemoryOptimized(const MapStateMemoryOptimized&& other) noexcept
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapStateMemoryOptimized()
		{
			memset(bytes, 0, bytesCount);
		}
		MapStateMemoryOptimized& operator=(const MapStateMemoryOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}
		MapStateMemoryOptimized& operator=(const MapStateMemoryOptimized&& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}

		State GetFieldState(unsigned x, unsigned y)const 
		{
			auto flatIndex=x*rowsCount + y;
			auto byteIndex = flatIndex*bitsPerField / bitsPerByte;
			auto inByteIndex = flatIndex*bitsPerField - byteIndex*bitsPerByte;
			auto shift = (bitsPerByte - inByteIndex - bitsPerField);

			return static_cast<State>(bytes[byteIndex] >> shift & 0b11);
		}

		void SetFieldState(unsigned x, unsigned y, State newState)
		{
			auto flatIndex = x*rowsCount + y;
			auto byteIndex = flatIndex*bitsPerField / bitsPerByte;
			auto inByteIndex = flatIndex*bitsPerField - byteIndex*bitsPerByte;
			auto shift = (bitsPerByte - inByteIndex - bitsPerField);

			auto mask = ~0b11<<shift;
			auto value = static_cast<int>(newState) << shift;
			bytes[byteIndex] = bytes[byteIndex] & mask;
			bytes[byteIndex] = bytes[byteIndex] | value;
		}
	};


	class MapStateProcessingOptimized
	{
	public:
		static const auto rowsCount = MapStateMemoryOptimized::rowsCount;
		static const auto colsCount = MapStateMemoryOptimized::colsCount;
	private:
		static const auto bytesCount = rowsCount*colsCount;
		unsigned char bytes[bytesCount];
	public:
		typedef MapStateMemoryOptimized::State State;
	public:
		explicit MapStateProcessingOptimized(char bytes[bytesCount])
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		explicit MapStateProcessingOptimized(int* bytes)
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		MapStateProcessingOptimized(const MapStateMemoryOptimized & memoryOther)
		{
			for(auto i=0;i<rowsCount;i++)
			{
				for (auto j = 0; j<colsCount; j++)
				{
					SetFieldState(i, j, memoryOther.GetFieldState(i, j));
				}
			}
		}
		MapStateProcessingOptimized(const MapStateProcessingOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapStateProcessingOptimized(const MapStateProcessingOptimized&& other) noexcept
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		MapStateProcessingOptimized()
		{
			memset(bytes, 0, bytesCount);
		}
		MapStateProcessingOptimized& operator=(const MapStateProcessingOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}
		MapStateProcessingOptimized& operator=(const MapStateProcessingOptimized&& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}

		State GetFieldState(unsigned x, unsigned y)const
		{
			auto flatIndex = x*rowsCount + y;

			return static_cast<State>(bytes[flatIndex]);
		}

		void SetFieldState(unsigned x, unsigned y, State newState)
		{
			auto flatIndex = x*rowsCount + y;
			bytes[flatIndex] = newState;
		}
	};

#ifdef _MEMORY_OPTIMIZED
	typedef MapStateMemoryOptimized MapState;
#else
	typedef MapStateProcessingOptimized MapState;
#endif
}
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
		{
			Empty,
			Player1,
			Player2,
			Unknown
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
		
		bool operator==(const MapStateMemoryOptimized& other)
		{
			for (int i = 0; i < bytesCount; i++)
			{
				if (bytes[i] != other.bytes[i])
					return false;
			}
			return true;
		}

		bool operator!=(const MapStateMemoryOptimized& other)
		{
			return !operator==(other);
		}
	};


	class MapStateProcessingOptimized
	{
	public:
		virtual ~MapStateProcessingOptimized() = default;
		static const auto rowsCount = MapStateMemoryOptimized::rowsCount;
		static const auto colsCount = MapStateMemoryOptimized::colsCount;
	protected:
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

		explicit MapStateProcessingOptimized(const MapStateMemoryOptimized & memoryOther)
		{
			for(auto i=0;i<rowsCount;i++)
			{
				for (auto j = 0; j<colsCount; j++)
				{
					MapStateProcessingOptimized::SetFieldState(i, j, memoryOther.GetFieldState(i, j));
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
		
		bool operator==(const MapStateProcessingOptimized& other)
		{
			for (int i = 0; i < bytesCount; i++)
			{
				if (bytes[i] != other.bytes[i])
					return false;
			}
			return true;
		}

		bool operator!=(const MapStateProcessingOptimized& other)
		{
			return !operator==(other);
		}

		virtual State GetFieldState(unsigned x, unsigned y)const
		{
			auto flatIndex = GetFlatIndex(x,y);
			if (bytes[flatIndex] > State::Unknown)
				return State::Unknown;
			return static_cast<State>(bytes[flatIndex]);
		}

		virtual void SetFieldState(unsigned x, unsigned y, State newState)
		{
			auto flatIndex = GetFlatIndex(x, y);
			bytes[flatIndex] = newState;
		}

	protected:
		static unsigned GetFlatIndex(unsigned x, unsigned y){
			return x*rowsCount + y;
		}
	};

#ifdef _MEMORY_OPTIMIZED
	typedef MapStateMemoryOptimized MapState;
#else
	typedef MapStateProcessingOptimized MapState;
#endif
}
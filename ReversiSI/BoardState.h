#pragma once
#include <memory>

namespace SI::Reversi
{
	class BoardStateMemoryOptimized
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
		explicit BoardStateMemoryOptimized(char bytes[bytesCount])
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		explicit BoardStateMemoryOptimized(int* bytes)
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		BoardStateMemoryOptimized(const BoardStateMemoryOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		BoardStateMemoryOptimized(const BoardStateMemoryOptimized&& other) noexcept
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		BoardStateMemoryOptimized()
		{
			memset(bytes, 0, bytesCount);
		}
		BoardStateMemoryOptimized& operator=(const BoardStateMemoryOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}
		BoardStateMemoryOptimized& operator=(const BoardStateMemoryOptimized&& other)
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
		
		bool operator==(const BoardStateMemoryOptimized& other) const
		{
			for (int i = 0; i < bytesCount; i++)
			{
				if (bytes[i] != other.bytes[i])
					return false;
			}
			return true;
		}

		bool operator!=(const BoardStateMemoryOptimized& other) const
		{
			return !operator==(other);
		}
	};


	class BoardStateProcessingOptimized
	{
	public:
		virtual ~BoardStateProcessingOptimized() = default;
		static const auto rowsCount = BoardStateMemoryOptimized::rowsCount;
		static const auto colsCount = BoardStateMemoryOptimized::colsCount;
	protected:
		static const auto bytesCount = rowsCount*colsCount;
		unsigned char bytes[bytesCount];
	public:
		typedef BoardStateMemoryOptimized::State FieldState;
	public:
		explicit BoardStateProcessingOptimized(char bytes[bytesCount])
		{
			memcpy(this->bytes, bytes, bytesCount);
		}
		explicit BoardStateProcessingOptimized(int* bytes)
		{
			memcpy(this->bytes, bytes, bytesCount);
		}

		explicit BoardStateProcessingOptimized(const BoardStateMemoryOptimized & memoryOther)
		{
			for(auto i=0;i<rowsCount;i++)
			{
				for (auto j = 0; j<colsCount; j++)
				{
					BoardStateProcessingOptimized::SetFieldState(i, j, memoryOther.GetFieldState(i, j));
				}
			}
		}
		BoardStateProcessingOptimized(const BoardStateProcessingOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		BoardStateProcessingOptimized(const BoardStateProcessingOptimized&& other) noexcept
		{
			memcpy(this->bytes, other.bytes, bytesCount);
		}
		BoardStateProcessingOptimized()
		{
			memset(bytes, 0, bytesCount);
		}
		BoardStateProcessingOptimized& operator=(const BoardStateProcessingOptimized& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}
		BoardStateProcessingOptimized& operator=(const BoardStateProcessingOptimized&& other)
		{
			memcpy(this->bytes, other.bytes, bytesCount);
			return *this;
		}
		
		bool operator==(const BoardStateProcessingOptimized& other) const
		{
			for (int i = 0; i < bytesCount; i++)
			{
				if (bytes[i] != other.bytes[i])
					return false;
			}
			return true;
		}

		bool operator!=(const BoardStateProcessingOptimized& other) const
		{
			return !operator==(other);
		}

		virtual FieldState GetFieldState(unsigned x, unsigned y)const
		{
			auto flatIndex = GetFlatIndex(x,y);
			if (bytes[flatIndex] > FieldState::Unknown)
				return FieldState::Unknown;
			return static_cast<FieldState>(bytes[flatIndex]);
		}

		virtual void SetFieldState(unsigned x, unsigned y, FieldState newState)
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
	typedef BoardStateMemoryOptimized BoardState;
#else
	typedef BoardStateProcessingOptimized BoardState;
#endif
}
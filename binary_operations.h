#ifndef BINARY_OPERATIONS_H_
#define BINARY_OPERATIONS_H_ 1

#include <cstdint>
#include <type_traits>
#include <cstddef>

#include "binary_templates.h"

#include "result_codes.h"
//#include "modules/core/preprocessor.h"
//#include "modules/core/literals.h"
//#include "modules/crossplatform/intrin.h"
//#include "modules/crossplatform/aligned.h"

#if __cplusplus >= 201703L
	#define __REGISTER
#else
	#define __REGISTER register
#endif

namespace math
{
	//************************************
	//Get	a single bit at the position,
	//Set	a single bit at the position
	//Clear a single bit at the position
	//************************************
	template<typename Type = uint8_t, typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>	inline	Type			GetBit				( const	Type* mass, size_t bit_position ); 
																												inline	void			SetBit				( uint8_t* mass, size_t bit_position, uint8_t bit_value ); 
																												inline	void			ClearBit			( uint8_t* mass, size_t bit_position );
	//************************************
	//Get a word of type "Type" at the position,
	//Set a word of type "Type" at the position
	//************************************
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	Type			GetWord				( const uint8_t* mass, size_t mass_bitpos );  
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	Type			GetWord				( const uint8_t* mass, size_t mass_bitpos, size_t word_bitlen ); 
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	void			SetWord				( uint8_t* mass, size_t bit_position, Type word );
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	void			SetWord				( uint8_t* mass, size_t bit_position, Type word, size_t word_bitlen );


	//************************************
	//Mask
	//************************************
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	Type			GetMask				( size_t word_bitlen );


	//************************************
	//Расчет четности количества установленных бит
	//************************************
	template<typename Type>																						inline	uint32_t		Parity				( Type value );
	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	uint32_t		Parity				( const uint8_t* mass, size_t mass_bitpos, size_t word_bitlen ); 
	
	
	//************************************
	// Расчет количества установленных бит
	//************************************
	template<typename Type>																						inline	uint8_t			CountOnBits			( Type value );


	//************************************
	// Переворачивает биты в слове. Если длина слова указана, то биты переварачиваются в слове длиной word_btlen
	//************************************
	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	Type			ReverseBitInWord	( Type word ); 
	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	Type			ReverseBitInWord	( Type word, uint32_t word_btlen );


	//************************************
	// Переворачивает биты в каждом байте в слове.
	//************************************
	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type = 0>				inline	Type			ReverseByteInWord	( Type word );


	//************************************
	// Перестанавливает байты для перехода от big endian к little endian
	// Changes the order of bytes from little endian to big endian and back
	//************************************
	template<typename Type>																						inline	Type			ByteSwap			( Type word );
}

namespace math
{
	#pragma region Mask
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	inline	Type	GetMask( size_t	word_bitlen )
	{
		const	typename std::make_unsigned<Type>::type	mask[] = 
		{	
			#define _SHR( v )	BinaryMask<typename std::make_unsigned<Type>::type, v>::value

			#define	_SHR8(v)	_SHR( v + 0 ),		_SHR( v + 1 ),		_SHR( v + 2 ),		_SHR( v + 3 ),		_SHR( v + 4 ),		_SHR( v + 5 ),		_SHR( v + 6 ),		_SHR( v + 7 )		
			#define	_SHR64(v)	_SHR8( v + 0 ),		_SHR8( v + 8 ),		_SHR8( v + 16 ),	_SHR8( v + 24 ),	_SHR8( v + 32 ),	_SHR8( v + 40 ),	_SHR8( v + 48 ),	_SHR8( v + 56 )

					_SHR64( 0 ),
					BinaryMaskOfOnes<Type>::value

			#undef	_SHR
			#undef	_SHR8
			#undef	_SHR64
		};

		return	static_cast<Type>( mask[word_bitlen] );
	}
	#pragma endregion


	#pragma region Bit
	template<typename Type, typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	inline Type GetBit( const Type* mass , size_t bit_position )
	{
		return	(mass[ bit_position / (sizeof(Type) * CHAR_BIT) ] >> (bit_position & (sizeof(Type) * CHAR_BIT - 1) )) & 1;
	}

	inline void SetBit( uint8_t * mass , size_t bit_position , uint8_t bit_value )
	{
		mass[bit_position / 8] = ( mass[bit_position / 8] & ( ~( 1 << bit_position % 8 ) ) ) ^ ( bit_value << bit_position % 8 );
	}

	inline void ClearBit( uint8_t* mass, size_t bit_position )
	{
		const	size_t	word_len	= sizeof( uint8_t ) * CHAR_BIT;
		mass[bit_position / word_len]	&= ~((uint8_t)((uint8_t) 1) << (bit_position & (word_len - 1)));
	}
	#pragma endregion


	#pragma region Word
	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type >
	inline Type GetWord( const uint8_t * src , size_t bit_pos )
	{
		const size_t nbits = sizeof(Type)* 8;
		const size_t shift = bit_pos % nbits;
		return  ( ( ((Type*)src)[bit_pos / nbits + 1] << ( nbits - shift ) ) &~GetMask<Type>(nbits-shift)  ) | (((Type*)src)[bit_pos / nbits] >> shift );
	}

	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>	
	inline Type GetWord	( const uint8_t* mass, size_t mass_bitpos, size_t word_bitlen )
	{
		return	GetWord<Type>( mass, mass_bitpos ) & (((Type)(-1)) >> (sizeof(Type) * CHAR_BIT - word_bitlen));
	}

	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type >
	inline void SetWord( uint8_t * mass , size_t bit_pos , Type word )
	{
		size_t pos           = bit_pos / ( sizeof( Type )*8 );
		size_t cur_word_bpos = bit_pos % ( sizeof( Type )*8 );
		((Type*)mass)[pos]    = ( ( ((Type*)mass)[pos]     &  GetMask<Type>(cur_word_bpos) ) | (   word                      << cur_word_bpos ) );
		((Type*)mass)[pos+ 1] = ( ( ((Type*)mass)[pos+1]   & ~GetMask<Type>(cur_word_bpos) ) | ( ( word >> ( sizeof( Type )*8 - cur_word_bpos ) ) &GetMask<Type>(cur_word_bpos) ) );
	}

	template <typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	inline void SetWord	( uint8_t* mass, size_t bit_position, Type word, size_t word_bitlen )
	{
		__REGISTER		const	size_t		type_bitlen	= sizeof(Type) * CHAR_BIT;
		__REGISTER				size_t		pos_byte	= bit_position / CHAR_BIT;
		__REGISTER				size_t		pos_shift	= bit_position & (CHAR_BIT - 1);
								Type*		ptr			= (Type*) &mass[pos_byte];

		if ( (pos_shift + word_bitlen) <= type_bitlen )
		{
			__REGISTER	Type	mask			= GetMask<Type>( word_bitlen );
								word			= word & mask;

								ptr[0]			= ptr[0] & (~(mask << pos_shift));
								ptr[0]			= ptr[0] ^ (word << pos_shift);
		}
		else
		{
			__REGISTER	size_t	word_a_bitlen	= type_bitlen - pos_shift;
			__REGISTER	Type	mask_a			= GetMask<Type>( word_a_bitlen );
			__REGISTER	Type	word_a			= word & mask_a;

								ptr[0]			= ptr[0] & (~(mask_a << pos_shift));
								ptr[0]			= ptr[0] ^ (word_a << pos_shift);


			__REGISTER	size_t	word_b_bitlen	= word_bitlen - word_a_bitlen;
			__REGISTER	Type	mask_b			= GetMask<Type>( word_b_bitlen );
			__REGISTER	Type	word_b			= (word >> word_a_bitlen) & mask_b;

								ptr[1]			= ptr[1] & (~mask_b);
								ptr[1]			= ptr[1] ^ word_b;
		}
		return	;
	}
	#pragma endregion


	#pragma region Parity
    namespace parity
    {
        static	const	uint32_t	parity_table_256[256] = 
        {
            #   define P2(n) n, (n)^1, (n)^1, n
            #   define P4(n) P2(n), P2((n)^1), P2((n)^1), P2(n)
            #   define P6(n) P4(n), P4((n)^1), P4((n)^1), P4(n)

            P6(0), P6(1), P6(1), P6(0)

			#	undef P2
			#	undef P4
			#	undef P6
        };
    }
	template<>
	inline	uint32_t	Parity<uint8_t>( uint8_t v )
	{
		return parity::parity_table_256[v & 0xff];
	}

	template<>
	inline	uint32_t	Parity<uint16_t>( uint16_t v )
	{
        v ^= v >> 8;

		return	parity::parity_table_256[v & 0xff];
	}

	template<>
	inline	uint32_t	Parity<uint32_t>( uint32_t v )
	{
		v ^= v >> 16;
		v ^= v >> 8;
		v ^= v >> 4;
		v &= 0xf;

		return (0x6996 >> v) & 1;

//         v ^= v >> 16;
// 
//         return	parity::parity_table_256[v & 0xff] ^ parity::parity_table_256[(v >> 8) & 0xff];
	}

	template<>
	inline	uint32_t	Parity<uint64_t>( uint64_t v )
	{
		uint32_t	dw = ((uint32_t)v) ^ (uint32_t)(v >> 32);
					dw = ( dw & 0xFFFF ) ^ ( dw >> 16 );
					dw = ( dw & 0x00FF ) ^ ( dw >> 8 );

		return	parity::parity_table_256[dw & 0xff];
	}

	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	uint32_t	Parity( const uint8_t* mass, size_t mass_bitpos, size_t word_bitlen )
	{
		uint32_t	result	= 0;

		for (size_t i = 0; i < word_bitlen / (sizeof(Type) * CHAR_BIT); i++)
		{
			result			^= math::Parity<Type>( math::GetWord<Type>( mass, mass_bitpos ) );
			mass_bitpos		+= sizeof(Type) * CHAR_BIT;
		}

		if ( word_bitlen - word_bitlen / (sizeof(Type) * CHAR_BIT) * (sizeof(Type) * CHAR_BIT)  )
			result			^= math::Parity<Type>( math::GetWord<Type>( mass, mass_bitpos, word_bitlen - word_bitlen / (sizeof(Type) * CHAR_BIT) * (sizeof(Type) * CHAR_BIT) ) );

		return	result;
	}
	#pragma endregion
		

//	#pragma region CountOnBits
//	ALIGNED_(16)	static const uint8_t g_onbits_table[] = {
//		0x00, 0x01, 0x01, 0x02, 0x01, 0x02, 0x02, 0x03, 0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 
//		0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 
//		0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 
//		0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 
//		0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 
//		0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 
//		0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 
//		0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07, 
//		0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 
//		0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 
//		0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 
//		0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07, 
//		0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 
//		0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07, 
//		0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07, 
//		0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07, 0x05, 0x06, 0x06, 0x07, 0x06, 0x07, 0x07, 0x08
//	};
//
//	template<>
//	inline	uint8_t	CountOnBits<uint8_t>	( uint8_t Data )
//	{
//		return g_onbits_table[Data];
//	}
//
//	template<>
//	inline	uint8_t	CountOnBits<uint16_t>	( uint16_t Data )
//	{
//		return g_onbits_table[Data & 0xFF] + g_onbits_table[Data >> 8];
//	}
//
//	template<>
//	inline	uint8_t	CountOnBits<uint32_t>	( uint32_t v )
//	{
//#if defined(__arm__) || defined(__aarch64__) || ( ( __SIZEOF_POINTER__ != 8 ) && !defined( _WIN64) )
//		v	= v - ((v >> 1) & 0x55555555);						// reuse input as temporary
//		v	= (v & 0x33333333) + ((v >> 2) & 0x33333333);		// temp
//
//		return	((v + (v >> 4) & 0x0F0F0F0F) * 0x01010101) >> 24;
//#else
//		return	static_cast<uint8_t>( _mm_popcnt_u32( v ) );
//#endif
//	}
//
//	template<>
//	inline	uint8_t	CountOnBits<uint64_t>	( uint64_t Data )
//	{
//		return CountOnBits<uint32_t>((uint32_t)Data) + CountOnBits<uint32_t>((uint32_t)(Data >> 32));
//	}
//	#pragma endregion


	#pragma region ReverseBitInWord
	template<>
	uint8_t		inline		ReverseBitInWord<uint8_t>	( uint8_t word )
	{
		#ifdef WIN32
			return	static_cast<uint8_t>(((word * 0x0802LU & 0x22110LU) | (word * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
		#else
			return	static_cast<uint8_t>(((word * 0x80200802ui64) & 0x0884422110ui64) * 0x0101010101ui64 >> 32);
		#endif
	}

	template<>
	uint64_t	inline		ReverseBitInWord<uint64_t>	( uint64_t word )
	{
		word = ((word & 0x5555555555555555) <<  1 | (word & 0xAAAAAAAAAAAAAAAA) >>  1);
		word = ((word & 0x3333333333333333) <<  2 | (word & 0xCCCCCCCCCCCCCCCC) >>  2);
		word = ((word & 0x0F0F0F0F0F0F0F0F) <<  4 | (word & 0xF0F0F0F0F0F0F0F0) >>  4);
		word = ((word & 0x00FF00FF00FF00FF) <<  8 | (word & 0xFF00FF00FF00FF00) >>  8);
		word = ((word & 0x0000FFFF0000FFFF) << 16 | (word & 0xFFFF0000FFFF0000) >> 16);
		word = (word << 32 | word >> 32);

		return	word;
	}

	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	Type		inline		ReverseBitInWord				( Type word )
	{
		Type	word_res	= 0;

		for (size_t i = 0; i < sizeof(Type); i++)
			word_res	^= static_cast<Type>( ReverseBitInWord<uint8_t>( (word >> (CHAR_BIT * i)) & 0xFF ) ) << (CHAR_BIT * (sizeof(Type) - 1 - i));

		return	word_res;
	}

	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	Type		inline		ReverseBitInWord				( Type word, uint32_t word_bitlen )
	{
		if ( word_bitlen == 0 )		return	(Type) 0;

		_DBG_CONDITION( word_bitlen <= sizeof(Type) * CHAR_BIT );

		return	ReverseBitInWord<Type>( word ) >> (sizeof(Type) * CHAR_BIT - word_bitlen);
	}

	template<typename Type,typename std::enable_if<std::is_unsigned<Type>::value,Type>::type>
	Type			inline		ReverseByteInWord	( Type word )
	{
		Type	word_res	= {};

		for (size_t i = 0; i < sizeof(word); ++i)
		{
			word_res	^= ReverseBitInWord<uint8_t>( (word >> (i * CHAR_BIT)) & 0xFF ) << (i * CHAR_BIT);
		}

		return	word_res;
	}
	#pragma endregion


	#pragma region ByteSwap
	template<>
	uint16_t		inline		ByteSwap<uint16_t>			( uint16_t val )
	{
		return (val << 8) | (val >> 8 );
	}

	template<>
	int16_t			inline		ByteSwap<int16_t>			(int16_t val)
	{
		return (val << 8) | ((val >> 8) & 0xFF);
	}

	template<>
	uint32_t		inline		ByteSwap<uint32_t>			( uint32_t val )
	{
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
		return (val << 16) | (val >> 16);
	}

	template<>
	int32_t			inline		ByteSwap<int32_t>			(int32_t val)
	{
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
		return (val << 16) | ((val >> 16) & 0xFFFF);
	}

	template<>
	uint64_t		inline		ByteSwap<uint64_t>			( uint64_t val )
	{
		val = ((val << 8) & 0xFF00FF00FF00FF00ui64) | ((val >> 8) & 0x00FF00FF00FF00FFui64);
		val = ((val << 16) & 0xFFFF0000FFFF0000ui64) | ((val >> 16) & 0x0000FFFF0000FFFFui64);
		return (val << 32) | (val >> 32);
	}

	template<>
	int64_t			inline		ByteSwap<int64_t>			(int64_t val)
	{
		val = ((val << 8) & 0xFF00FF00FF00FF00ui64) | ((val >> 8) & 0x00FF00FF00FF00FFui64);
		val = ((val << 16) & 0xFFFF0000FFFF0000ui64) | ((val >> 16) & 0x0000FFFF0000FFFFui64);
		return (val << 32) | ((val >> 32) & 0xFFFFFFFFui64);
	}
	#pragma endregion
}
#endif //BINARY_OPERATIONS_H_

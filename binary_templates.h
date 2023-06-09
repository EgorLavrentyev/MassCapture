#ifndef MODULES_MATH_DISRETE_BINARY_TEMPLATES_H
#define MODULES_MATH_DISRETE_BINARY_TEMPLATES_H	1

#include <cstdint>
#include <type_traits>
#include <climits>


namespace math
{
	template<typename Type>													struct BitLength;

	template<typename Type>													struct BinaryMaskOfOnes;
	template<typename Type, uint32_t word_bitlen>							struct BinaryMask;
}


// implementation
namespace	math
{
	template<typename Type>
	struct	BitLength
	{
		enum: uint32_t
		{	
			value	= sizeof(Type) * CHAR_BIT
		};
	};



	template<typename Type>
	struct BinaryMaskOfOnes
	{
		using	UnsignedType	= typename std::make_unsigned<Type>::type;

		enum: UnsignedType
		{	
			value	= static_cast<UnsignedType>( ~static_cast<UnsignedType>( 0 ) )
		};
	};



	template<bool is_zero, bool is_out_of_rage, uint32_t word_bitlen, typename Type>
	struct BinaryMaskAssist;

	template<uint32_t word_bitlen, typename Type>
	struct BinaryMaskAssist<true, false, word_bitlen, Type>
	{
		enum: Type
		{
			value	= static_cast<typename std::make_unsigned<Type>::type>( 0 )
		};
	};

	template<uint32_t word_bitlen, typename Type>
	struct BinaryMaskAssist<true, true, word_bitlen, Type>
	{
		enum: Type
		{
			value	= static_cast<typename std::make_unsigned<Type>::type>( 0 )
		};
	};

	template<uint32_t word_bitlen, typename Type>
	struct BinaryMaskAssist<false, true, word_bitlen, Type>
	{
		enum: Type
		{	
			value	= BinaryMaskOfOnes<Type>::value >> (BitLength<Type>::value - word_bitlen)
		};
	};

	template<uint32_t word_bitlen, typename Type>
	struct BinaryMaskAssist<false, false, word_bitlen, Type>
	{
		enum: Type
		{
			value	= BinaryMaskOfOnes<Type>::value
		};
	};



	template<typename Type, uint32_t word_bitlen>
	struct BinaryMask
	{
		enum: Type
		{
			value	= static_cast<typename std::make_unsigned<Type>::type>( BinaryMaskAssist<(word_bitlen == 0), (word_bitlen < BitLength<Type>::value), word_bitlen, Type>::value )
		};
	};
}

#endif

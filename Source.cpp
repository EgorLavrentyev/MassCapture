#include <iostream>
#include <iomanip>
#include <windows.h>
#include <cstring>
#include <chrono>
#include <algorithm>
#include "result_codes.h"
#include "binary_operations.h"
#include <cstdint>
#include <stdexcept>

#define MODULES_MATH_DISRETE_BINARY_FUNCTIONS_H	1

#define SRC_SIZE 100000000
#define DST_SIZE 100000000

//#define SRC_SIZE 5
//#define DST_SIZE 5

void fill_mass(unsigned char mass[], size_t size) {
	for (unsigned i = 0; i < size; i++) {
		mass[i] = rand() % 255;
	}
}

void print_bytes(uint8_t* a, int size) {

	for (int k = 0; k < size; k++) {
		for (int i = 0; i < 8; i++)
			if (a[size - k - 1] & 1 << (8 - i - 1)) {
				std::cout << "1";
			}
			else
				std::cout << "0";

		std::cout << " ";
	}
	std::cout << std::endl;
}


void print_bytes_color(uint8_t* a, int size, size_t pos_bit, size_t size_bit, int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (size_t k = 0; k < size; k++) {
		for (size_t i = 0; i < 8; i++) {
			size_t pos = (size - k - 1) * 8 + 8 - i - 1;
			if (pos >= pos_bit && pos < pos_bit + size_bit){
				SetConsoleTextAttribute(hConsole, color);
			}
			else {
				SetConsoleTextAttribute(hConsole, 15);
			}

			if (a[size - k - 1] & 1 << (8 - i - 1)) {
				std::cout << "1";
			}
			else
				std::cout << "0";
		}
		std::cout << " ";
	}
	std::cout << std::endl;
	SetConsoleTextAttribute(hConsole, 15);
}

bool MassCapture_primitive(const uint8_t* src_mass, size_t src_pos_bit, size_t size_bit, uint8_t* dst_mass, size_t dst_pos_bit) {
	if (src_mass == nullptr || dst_mass == nullptr || size_bit == 0 || src_pos_bit + size_bit - 1 > SRC_SIZE * 8 || dst_pos_bit + size_bit - 1 > DST_SIZE * 8) {
		return false;
	}

	size_t src_pos_byte = src_pos_bit / 8;
	size_t src_pos_in_byte = src_pos_bit % 8;
	size_t dst_pos_byte = dst_pos_bit / 8;
	size_t dst_pos_in_byte = dst_pos_bit % 8;

	while (size_bit) {
		uint8_t src_bit = src_mass[src_pos_byte] & (1 << src_pos_in_byte);
		dst_mass[dst_pos_byte] &= ~(1 << dst_pos_in_byte);
		dst_mass[dst_pos_byte] |= (src_bit >> src_pos_in_byte) << dst_pos_in_byte;

		src_pos_in_byte++;
		dst_pos_in_byte++;

		if (dst_pos_in_byte == 8) {
			dst_pos_byte++;
			dst_pos_in_byte = 0;
		}
		if (src_pos_in_byte == 8) {
			src_pos_byte++;
			src_pos_in_byte = 0;
		}
		size_bit--;
	}

	return true;
}

//bool MassCapture(const uint8_t* src_mass, size_t src_pos_bit, size_t size_bit, uint8_t* dst_mass, size_t dst_pos_bit) {
//	if (src_mass == nullptr || dst_mass == nullptr || size_bit == 0 || src_pos_bit + size_bit - 1 > SRC_SIZE * 8 || dst_pos_bit + size_bit - 1 > DST_SIZE * 8) {
//		return false;
//	}
//
//	// ��������� �������
//	size_t src_pos_byte = src_pos_bit / 8;
//	size_t src_pos_in_byte = src_pos_bit % 8;
//	size_t dst_pos_byte = dst_pos_bit / 8;
//	size_t dst_pos_in_byte = dst_pos_bit % 8;
//
//	size_t bits_left = size_bit;
//	while (bits_left > 0) {
//		// ���������� ���, ������� ����� ����������� � ������� �����
//		size_t bits_to_copy = min(bits_left, 8 - src_pos_in_byte);
//		bits_to_copy = min(bits_to_copy, 8 - dst_pos_in_byte);
//
//		// �����, ������� ������������� ������ ���� � �������� ����� � 1
//		uint8_t mask = ((1 << bits_to_copy) - 1) << src_pos_in_byte;
//
//		// ����������� ������ �����
//		uint8_t bits = (src_mass[src_pos_byte] & mask) >> src_pos_in_byte;
//		bits <<= dst_pos_in_byte;
//		mask = ((1 << bits_to_copy) - 1) << dst_pos_in_byte;
//		dst_mass[dst_pos_byte] &= ~mask;
//		dst_mass[dst_pos_byte] |= bits;
//
//		// ����������� ��������� � �������
//		bits_left -= bits_to_copy;
//		src_pos_in_byte += bits_to_copy;
//		dst_pos_in_byte += bits_to_copy;
//
//		// ���� ��������� ����� ����� ��������� �������, ������� � ���������� �����
//		if (src_pos_in_byte == 8) {
//			src_pos_byte++;
//			src_pos_in_byte = 0;
//		}
//
//		// ���� ��������� ����� ����� �������� �������, ������� � ���������� �����
//		if (dst_pos_in_byte == 8) {
//			dst_pos_byte++;
//			dst_pos_in_byte = 0;
//		}
//	}
//	return true;
//}

bool MassCaptureOptimized(const uint8_t* src_mass, size_t src_pos_bit, size_t size_bit, uint8_t* dst_mass, size_t dst_pos_bit) {
	if (src_mass == nullptr || dst_mass == nullptr || size_bit == 0 || src_pos_bit + size_bit - 1 > SRC_SIZE * 8 || dst_pos_bit + size_bit - 1 > DST_SIZE * 8) {
		return false;
	}

	// ��������� �������
	size_t src_pos_byte = src_pos_bit / 8;
	size_t src_pos_in_byte = src_pos_bit % 8;
	size_t dst_pos_byte = dst_pos_bit / 8;
	size_t dst_pos_in_byte = dst_pos_bit % 8;

	size_t bits_left = size_bit;
	while (bits_left > 0) {
		// ���������� ���, ������� ����� ����������� � ������� �����
		size_t bits_to_copy = min(bits_left, 8 - src_pos_in_byte);
		bits_to_copy = min(bits_to_copy, 8 - dst_pos_in_byte);

		//���� ����� ����������� ���� �������
		if (bits_to_copy == 8) {
			dst_mass[dst_pos_byte] = src_mass[src_pos_byte];
			bits_left -= bits_to_copy;
			src_pos_byte++;
			src_pos_in_byte = 0;
			dst_pos_byte++;
			dst_pos_in_byte = 0;
		}
		else {
			// �����, ������� ������������� ������ ���� � �������� ����� � 1
			uint8_t mask = ((1 << bits_to_copy) - 1) << src_pos_in_byte;

			// ����������� ������ �����
			uint8_t bits = (src_mass[src_pos_byte] & mask) >> src_pos_in_byte;
			bits <<= dst_pos_in_byte;
			mask = ((1 << bits_to_copy) - 1) << dst_pos_in_byte;
			dst_mass[dst_pos_byte] &= ~mask;
			dst_mass[dst_pos_byte] |= bits;

			// ����������� ��������� � �������
			bits_left -= bits_to_copy;
			src_pos_in_byte += bits_to_copy;
			dst_pos_in_byte += bits_to_copy;

			// ���� ��������� ����� ����� ��������� �������, ������� � ���������� �����
			if (src_pos_in_byte == 8) {
				src_pos_byte++;
				src_pos_in_byte = 0;
			}

			// ���� ��������� ����� ����� �������� �������, ������� � ���������� �����
			if (dst_pos_in_byte == 8) {
				dst_pos_byte++;
				dst_pos_in_byte = 0;
			}
		}
	}
	return true;
}

bool MassCapture_A(const uint8_t* src_mass, size_t src_pos_bit, size_t size_bit, uint8_t* dst_mass, size_t dst_pos_bit) {
	if (src_mass == nullptr || dst_mass == nullptr || size_bit == 0 || src_pos_bit + size_bit - 1 > SRC_SIZE * 8 || dst_pos_bit + size_bit - 1 > DST_SIZE * 8) {
		return false;
	}

	// ��������� �������
	size_t src_pos_byte = src_pos_bit / 8;
	size_t src_pos_in_byte = src_pos_bit % 8;
	size_t dst_pos_byte = dst_pos_bit / 8;
	size_t dst_pos_in_byte = dst_pos_bit % 8;
	size_t src_end_pos_in_byte = (src_pos_bit + size_bit) % 8;
	int flag1 = src_pos_in_byte == 0 ? 0 : 1;
	int flag2 = src_end_pos_in_byte == 0 ? 0 : 1;
	size_t full_bytes_left = (size_bit - (8 - src_pos_in_byte) * flag1 - src_end_pos_in_byte * flag2) / 8;

	// ������� 1-� �������� ���� �� src, ���� �� ����
	size_t bits_left = min(size_bit, 8 - src_pos_in_byte);
	while (bits_left % 8)
	{
		size_t bits_to_copy = min(bits_left, 8 - dst_pos_in_byte);
		uint8_t mask = ((1 << bits_to_copy) - 1) << src_pos_in_byte;
		uint8_t bits = (src_mass[src_pos_byte] & mask) >> src_pos_in_byte;
		bits <<= dst_pos_in_byte;
		mask = ((1 << bits_to_copy) - 1) << dst_pos_in_byte;
		dst_mass[dst_pos_byte] &= ~mask;
		dst_mass[dst_pos_byte] |= bits;

		bits_left -= bits_to_copy;
		src_pos_in_byte += bits_to_copy;
		dst_pos_in_byte += bits_to_copy;

		if (dst_pos_in_byte == 8) {
			dst_pos_byte++;
			dst_pos_in_byte = 0;
		}
		if (src_pos_in_byte == 8) {
			src_pos_byte++;
			src_pos_in_byte = 0;
		}
	}

	// ������� ������ ����� �� src
	uint8_t mask = (0xFF << dst_pos_in_byte);
	uint8_t mask1 = 0xFF << (8 - dst_pos_in_byte);
	for (; full_bytes_left; full_bytes_left--) {
		uint8_t src_byte = src_mass[src_pos_byte++];
		dst_mass[dst_pos_byte] &= ~mask;
		dst_mass[dst_pos_byte] |= (src_byte & ~mask1) << dst_pos_in_byte;

		dst_mass[++dst_pos_byte] &= mask;
		dst_mass[dst_pos_byte] |= (src_byte & mask1) >> (8 - dst_pos_in_byte);
	}

	// ������� �����
	bits_left = src_end_pos_in_byte * flag2;
	src_pos_in_byte = 0;

	while (bits_left % 8) {
		size_t bits_to_copy = min(bits_left, 8 - dst_pos_in_byte);
		uint8_t mask = ((1 << bits_to_copy) - 1) << src_pos_in_byte;
		uint8_t bits = (src_mass[src_pos_byte] & mask) >> src_pos_in_byte;
		bits <<= dst_pos_in_byte;
		mask = ((1 << bits_to_copy) - 1) << dst_pos_in_byte;
		dst_mass[dst_pos_byte] &= ~mask;
		dst_mass[dst_pos_byte] |= bits;

		bits_left -= bits_to_copy;
		src_pos_in_byte += bits_to_copy;
		dst_pos_in_byte += bits_to_copy;

		if (dst_pos_in_byte == 8) {
			dst_pos_byte++;
			dst_pos_in_byte = 0;
		}
	}
		
	return true;
}

bool MassCapture_B(const uint8_t* src_mass, size_t src_pos_bit, size_t size_bit, uint8_t* dst_mass, size_t dst_pos_bit) {
	if (src_mass == nullptr || dst_mass == nullptr || size_bit == 0 || src_pos_bit + size_bit - 1 > SRC_SIZE * 8 || dst_pos_bit + size_bit - 1 > DST_SIZE * 8) {
		return false;
	}

	// ��������� �������
	size_t src_pos_byte = src_pos_bit / 8;
	size_t src_pos_in_byte = src_pos_bit % 8;
	size_t dst_pos_byte = dst_pos_bit / 8;
	size_t dst_pos_in_byte = dst_pos_bit % 8;
	size_t dst_end_pos_in_byte = (dst_pos_bit + size_bit) % 8;
	int flag1 = dst_pos_in_byte == 0 ? 0 : 1;
	int flag2 = dst_end_pos_in_byte == 0 ? 0 : 1;
	size_t full_bytes_left = (size_bit - (8 - dst_pos_in_byte) * flag1 - dst_end_pos_in_byte * flag2) / 8;

	// ������� � 1-� �������� ���� dst, ���� �� ����
	size_t bits_left = min(size_bit, 8 - dst_pos_in_byte);
	while (bits_left % 8)
	{
		size_t bits_to_copy = min(bits_left, 8 - src_pos_in_byte);
		uint8_t mask = ((1 << bits_to_copy) - 1) << src_pos_in_byte;
		uint8_t bits = (src_mass[src_pos_byte] & mask) >> src_pos_in_byte;
		bits <<= dst_pos_in_byte;
		mask = ((1 << bits_to_copy) - 1) << dst_pos_in_byte;
		dst_mass[dst_pos_byte] &= ~mask;
		dst_mass[dst_pos_byte] |= bits;

		bits_left -= bits_to_copy;
		src_pos_in_byte += bits_to_copy;
		dst_pos_in_byte += bits_to_copy;

		if (dst_pos_in_byte == 8) {
			dst_pos_byte++;
			dst_pos_in_byte = 0;
		}
		if (src_pos_in_byte == 8) {
			src_pos_byte++;
			src_pos_in_byte = 0;
		}
	}

	// ������� � ������ ����� dst
	uint8_t mask = (0xFF << src_pos_in_byte);
	uint8_t mask1 = 0xFF << (8 - dst_pos_in_byte);
	
	for (; full_bytes_left; full_bytes_left--) {
		uint8_t byte = 0;
		byte |= (src_mass[src_pos_byte++] & mask) >> src_pos_in_byte;
		byte |= (src_mass[src_pos_byte] & (~mask)) << (8 - src_pos_in_byte);
		dst_mass[dst_pos_byte++] = byte;
	}

	// ������� � �����
	bits_left = dst_end_pos_in_byte * flag2;
	dst_pos_in_byte = 0;

	while (bits_left % 8) {
		size_t bits_to_copy = min(bits_left, 8 - src_pos_in_byte);
		uint8_t mask = ((1 << bits_to_copy) - 1) << src_pos_in_byte;
		uint8_t bits = (src_mass[src_pos_byte] & mask) >> src_pos_in_byte;
		bits <<= dst_pos_in_byte;
		mask = ((1 << bits_to_copy) - 1) << dst_pos_in_byte;
		dst_mass[dst_pos_byte] &= ~mask;
		dst_mass[dst_pos_byte] |= bits;

		bits_left -= bits_to_copy;
		src_pos_in_byte += bits_to_copy;
		dst_pos_in_byte += bits_to_copy;

		if (src_pos_in_byte == 8) {
			src_pos_byte++;
			src_pos_in_byte = 0;
		}
	}

	return true;
}

namespace math
{
	//************************************
	//	Capture a series of "bitlen" bits from "src" at the position
	//	and store them in "dst" at the position 
	//************************************
	bool MassCapture(const uint8_t* src, size_t src_bitpos, size_t bitlen, uint8_t* dst, size_t dst_bitpos);


}


// implementation
namespace	math
{

	namespace	details
	{
		template<typename Base>
		bool MassCapture(const uint8_t* src, size_t src_bitpos, size_t bitlen, uint8_t* dst, size_t dst_bitpos)
		{
			if (src == nullptr || dst == nullptr)	return Result::e_pointer;
			if (bitlen == 0)						return Result::noerror;

			/*constexpr*/  size_t base_size = sizeof(Base) * CHAR_BIT;

			// if ( bitlen is short )
			size_t bits_written = 0;
			size_t dst_start_byte_pos = dst_bitpos / 8;
			size_t dst_end_byte_pos = (dst_bitpos + bitlen - 1) / 8;

			if (bitlen + dst_bitpos % base_size < base_size)
			{
				uint8_t start_byte_mask = GetMask<uint8_t>(dst_bitpos % 8);
				if (dst_start_byte_pos == dst_end_byte_pos)
				{
					uint8_t	end_byte_mask = GetMask<uint8_t>((dst_bitpos + bitlen) % 8);
					if ((dst_bitpos + bitlen) % 8 == 0) end_byte_mask = 0xFF;

					if ((src_bitpos + CHAR_BIT) < (src_bitpos + bitlen + src_bitpos % CHAR_BIT))
					{
						dst[dst_bitpos / CHAR_BIT] = dst[dst_bitpos / CHAR_BIT] & (start_byte_mask | ~end_byte_mask) | ((GetWord<uint8_t>(src, src_bitpos) << (dst_bitpos % 8)) & end_byte_mask);
					}
					else
					{
						dst[dst_bitpos / CHAR_BIT] = (dst[dst_bitpos / CHAR_BIT] & (start_byte_mask | ~end_byte_mask)) | ((src[src_bitpos / CHAR_BIT] >> (src_bitpos % CHAR_BIT) << (dst_bitpos % 8)) & end_byte_mask);
					}

					return true;
				}

				dst[dst_start_byte_pos] = dst[dst_start_byte_pos] & start_byte_mask | (GetWord<uint8_t>(src, src_bitpos) << dst_bitpos % 8);
				bits_written += 8 - dst_bitpos % 8;
			}
			else
			{
				const	Base* wsrc = reinterpret_cast<const Base*>(src);
				Base* wdst = reinterpret_cast<Base*>(dst);

				//stage 1
				//Make dst_bitpos % base_size equal to 0, that is, align it
				if (dst_bitpos % base_size != 0) // alignment
				{
					wdst[dst_bitpos / base_size] = wdst[dst_bitpos / base_size] & (GetMask<Base>(dst_bitpos % base_size)) | (GetWord<Base>(src, src_bitpos) << dst_bitpos % base_size);

					bits_written = base_size - dst_bitpos % base_size;
				}

				//precomputation for stage 2
				size_t left_shift = base_size - (src_bitpos + bits_written) % base_size;
				size_t right_shift = (src_bitpos + bits_written) % base_size;
				wsrc += (src_bitpos + bits_written) / base_size;
				wdst += (dst_bitpos + bits_written) / base_size;

				size_t nwords = 0;
				if (bitlen >= base_size + bits_written)
				{
					nwords = (bitlen - bits_written) / base_size;
					bits_written += base_size * nwords; // will be written after stage 2
				}

				//stage 2, central part
				//copying word by word
				const Base shift_mask = ~GetMask<Base>(left_shift);
				for (size_t i = 0; i < nwords; i++)
				{
					*wdst = (*(wsrc + 1) << left_shift) & shift_mask | (*wsrc >> right_shift);
					wsrc++;
					wdst++;
				}
			}

			//stage 3
			// tail
			dst_start_byte_pos = (dst_bitpos + bits_written) / 8;

			size_t nbytes = 1 + dst_end_byte_pos - dst_start_byte_pos;
			for (size_t i = 0; i + 1 < nbytes; ++i)
			{
				dst[dst_start_byte_pos + i] = GetWord<uint8_t>(src, src_bitpos + bits_written + i * 8);
			}

			if (nbytes > 0)
			{
				const	size_t	bit_pos = src_bitpos + bits_written + (nbytes - 1) * CHAR_BIT;
				const	size_t	end_byte_shift = (dst_bitpos + bitlen) % 8;
				uint8_t	end_byte_mask = GetMask<uint8_t>(end_byte_shift);

				if (end_byte_shift == 0) end_byte_mask = 0xFF;

				if ((bit_pos + CHAR_BIT) < (src_bitpos + bitlen + bit_pos % CHAR_BIT))
				{
					dst[dst_end_byte_pos] = (dst[dst_end_byte_pos] & ~end_byte_mask) | (GetWord<uint8_t>(src, bit_pos) & end_byte_mask);
				}
				else
				{
					dst[dst_end_byte_pos] = (dst[dst_end_byte_pos] & ~end_byte_mask) | ((src[bit_pos / CHAR_BIT] >> (bit_pos % CHAR_BIT)) & end_byte_mask);
				}
			}

			return true;
		}
	} //end namespace

#pragma region MassCapture
	bool MassCapture(const uint8_t* src, size_t src_bitpos, size_t bitlen, uint8_t* dst, size_t dst_bitpos)
	{
#ifdef WIN32
		return details::MassCapture<uint32_t>(src, src_bitpos, bitlen, dst, dst_bitpos);
#else
		return details::MassCapture<uint64_t>(src, src_bitpos, bitlen, dst, dst_bitpos);
#endif
	}
#pragma endregion

	template<typename Type>
	inline	uint32_t		MassCountOnBits(const Type* src, size_t src_len)
	{
		if (src == nullptr)			return	0;
		if (src_len == 0)				return	0;

		uint32_t	err_cnt = 0;

		for (size_t i = 0; i < src_len; ++i)
		{
			err_cnt += static_cast<uint32_t>(math::CountOnBits(src[i]));
		}

		return	err_cnt;
	}
}


bool get_time(void) {
	uint8_t* src_mass = new uint8_t[SRC_SIZE];
	uint8_t* dst_mass = new uint8_t[DST_SIZE];
	uint8_t* dst_mass1 = new uint8_t[DST_SIZE];
	fill_mass(src_mass, SRC_SIZE);
	fill_mass(dst_mass, DST_SIZE);
	

	size_t num_bits = 99999990;
	size_t iter = 10;
	size_t size_bit = SRC_SIZE / 3;
	std::memcpy(dst_mass1, dst_mass, num_bits / 8);
	bool result = 0;
	bool are_equal = std::equal(dst_mass, dst_mass + num_bits / 8, dst_mass1);
	result = MassCapture_primitive(src_mass, 0, num_bits, dst_mass1, 0);

	auto begin = std::chrono::high_resolution_clock::now();
	/*for (int i = 0; i < iter; i++) {
		result = MassCapture(src_mass, 0, num_bits, dst_mass, 0);
	}*/

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	//std::cout << "Slow_func = " << double (iter * num_bits / (8 * 1024 * 1024)) /(elapsed_ms.count() / 1000.0) << " MB/s\n";

	/*begin = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iter; i++) {
		result = MassCapture_A(src_mass, 9, num_bits, dst_mass, 0);
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	//are_equal = std::equal(dst_mass, dst_mass + num_bits / 8, dst_mass1);
	std::cout << "Func_A (1 read, 2 writes) = " << double(iter * num_bits / (8 * 1024 * 1024)) / (elapsed_ms.count() / 1000.0) << " MB/s\n";
	//std::cout << are_equal << std::endl;*/

	begin = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iter; i++) {
		result = MassCapture_B(src_mass, 8, num_bits, dst_mass, 0);
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	//are_equal = std::equal(dst_mass, dst_mass + num_bits / 8, dst_mass1);
	std::cout << "Func_B (2 reads, 1 write) = " << double(iter * num_bits / (8 * 1024 * 1024)) / (elapsed_ms.count() / 1000.0) << " MB/s\n";
	//std::cout << are_equal << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iter; i++) {
		result = math::MassCapture(src_mass, 8, num_bits, dst_mass, 0);
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	//are_equal = std::equal(dst_mass, dst_mass + num_bits / 8, dst_mass1);
	std::cout << "MassCapture = " << double(iter * num_bits / (8 * 1024 * 1024)) / (elapsed_ms.count() / 1000.0) << " MB/s\n";
	//std::cout << are_equal << std::endl;

	auto begin1 = std::chrono::high_resolution_clock::now();
	//num_bits /= 8;
	for (int i = 0; i < iter; i++) {
		result = std::memcpy(dst_mass, src_mass, num_bits / 8);
	}
	auto end1 = std::chrono::high_resolution_clock::now();
	auto elapsed_ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - begin1);
	std::cout << "std::memcpy() = " << double (iter * num_bits / (8 * 1024 * 1024)) / (elapsed_ms1.count() / 1000.0) << " MB/s";

	return result;
}




int main() {
	//uint8_t src_mass[2] = { 0b10110101, 0b10010110 };
	//uint8_t* dst_mass = new uint8_t[3];
	//uint8_t dst_mass[3] = { 0xab, 0xcd, 0xef };

	//uint8_t* src_mass = new uint8_t[SRC_SIZE];
	//uint8_t* dst_mass = new uint8_t[DST_SIZE];
	//fill_mass(src_mass, SRC_SIZE);
	//fill_mass(dst_mass, DST_SIZE);


	//size_t src_pos_bit = 0, size_bit = 0, dst_pos_bit = 0;

	//std::cout << "Bit index in source array: ";
	//std::cin >> src_pos_bit;
	//std::cout << "Number of bits to copy: ";
	//std::cin >> size_bit;
	//std::cout << "Bit index in destination array: ";
	//std::cin >> dst_pos_bit;
	//std::cout << std::endl;

	//std::cout << std::setw(20) << "src_mass: ";
	//print_bytes_color(src_mass, SRC_SIZE, src_pos_bit, size_bit, 10);
	//std::cout << std::setw(20) << "dst_mass: ";
	//print_bytes_color(dst_mass, DST_SIZE, dst_pos_bit, size_bit, 12);
	//
	//bool result = math::MassCapture(src_mass, src_pos_bit, size_bit, dst_mass, dst_pos_bit);

	//if (result) {
	//	std::cout << std::setw(20) << "Result: ";
	//	print_bytes_color(dst_mass, DST_SIZE, dst_pos_bit, size_bit, 10);
	//}
	//else {
	//	std::cout << "Copy failed." << std::endl;
	//}

	bool result = get_time();

	return 0;
}
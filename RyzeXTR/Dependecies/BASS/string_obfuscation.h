#pragma once

#include <array>
// ReSharper disable once CppUnusedIncludeDirective
#include "random.h"

template <size_t n, char k>
struct xorstr
{
private:
	static constexpr char enc(const char c)
	{
		return c ^ k;
	}

public:
	template <size_t... s>
	constexpr __forceinline xorstr(const char* str, std::index_sequence<s...>) : encrypted { enc(str[s])... } { }

	__forceinline std::string dec()
	{
		std::string dec;
		dec.resize(n);

		for (auto i = 0; i < n; i++)
			dec[i] = encrypted[i] ^ k;

		return dec;
	}

	__forceinline std::string ot(bool decrypt = true)
	{
		std::string dec;
		dec.resize(n);

		for (auto i = 0; i < n; i++)
		{
			dec[i] = decrypt ? (encrypted[i] ^ k) : encrypted[i];
			encrypted[i] = '\0';
		}

		return dec;
	}

	std::array<char, n> encrypted {};
};

#define _(s) xorstr<sizeof(s), random::_char<__COUNTER__>::value>(s, std::make_index_sequence<sizeof(s)>()).dec().c_str()
#define _ot(s) xorstr<sizeof(s), random::_char<__COUNTER__>::value>(s, std::make_index_sequence<sizeof(s)>()).ot().c_str()
#define __(s) []() -> std::pair<std::string, char> { \
	constexpr auto key = random::_char<__COUNTER__>::value; \
	return std::make_pair(xorstr<sizeof(s), key>(s, std::make_index_sequence<sizeof(s)>()).ot(false), key); \
}()
#define _rt(n, s) auto (n) = reinterpret_cast<char*>(alloca(((s).first.size() + 1) * sizeof(char))); \
	for (size_t i = 0; i < (s).first.size(); i++) \
        (n)[i] = (s).first[i] ^ (s).second; \
    (n)[(s).first.size()] = '\0'

#ifndef _SHORT_INT32_H_
#define _SHORT_INT32_H_

#include <cstdlib>

namespace vasaka {

template <typename T>
inline constexpr int32_t signum(T x) {
	return (T(0) < x) - (x < T(0));
}

inline constexpr int8_t signum_log2(int32_t x) {
	static_assert(8 * sizeof(int32_t) == __builtin_clz(1) + 1, "log is wrong");
	// signum(x) * log2(abs(x))
	return signum(x) * static_cast<int8_t>(8 * sizeof(int32_t) - __builtin_clz(abs(x)) - 1);
}

inline constexpr int8_t abs_log2(int32_t x) {
	static_assert(8 * sizeof(int32_t) == __builtin_clz(1) + 1, "log is wrong");
	return static_cast<int8_t>(8 * sizeof(int32_t) - __builtin_clz(abs(x)) - 1);
}


class short_int32_t {
public:
	constexpr  short_int32_t(): m_log(0), m_tail(0) {};

	short_int32_t(int32_t x) {
		set(x);
	}

	short_int32_t& operator=(int32_t x) {
		set(x);
		return *this;
	}

	bool operator<(short_int32_t x) const {
		return m_log == x.m_log ? (signum(m_log) * m_tail < signum(x.m_log) * x.m_tail) : (m_log < x.m_log);
	}

	bool operator<=(short_int32_t x) const {
		return m_log == x.m_log ? (signum(m_log) * m_tail <= signum(x.m_log) * x.m_tail) : (m_log < x.m_log);
	}

	bool operator>(short_int32_t x) const {
		return m_log == x.m_log ? (signum(m_log) * m_tail > signum(x.m_log) * x.m_tail) : (m_log > x.m_log);
	}

	bool operator>=(short_int32_t x) const {
		return m_log == x.m_log ? (signum(m_log) * m_tail >= signum(x.m_log) * x.m_tail) : (m_log > x.m_log);
	}

	int32_t operator*(short_int32_t x) const {
		return get() * x.get();
	}

	int32_t operator+(short_int32_t x) const {
		return get() + x.get();
	}

	int32_t operator-(short_int32_t x) const {
		return get() - x.get();
	}

	int32_t operator*(int32_t x) const {
		return get() * x;
	}

	int32_t operator+(int32_t x) const {
		return get() + x;
	}

	int32_t operator-(int32_t x) const {
		return get() - x;
	}

	void set(int32_t x) {
		if (x == 0) {
			m_log = 0;
			m_tail = 0;
			return;
		}

		int8_t abs_log = abs_log2(x);
		int32_t interval = abs_log ? 1 << abs_log : 0;
		if (interval == abs(x)) {
			m_log = x < 0 ? -abs_log : abs_log;
			m_tail = 0;
			return;
		}

		//int32_t bin_size = interval < 512 ? 1 : (interval >> 8);
		int32_t bin_size = interval < 512 ? 1 : (x < 0 ? (interval >> 9) : (interval >> 8));

		m_log = x > 0 ? abs_log : -abs_log - 1;
		int32_t tail = x > 0 ? x - interval : ((1 << -m_log) + x);

		m_tail = tail / bin_size;
	}

	int32_t get() const {
		int32_t interval = 1 << abs(m_log);
		int32_t bin_size = interval < 512 ? 1 : (m_log < 0 ? (interval >> 9) : (interval >> 8));

		return signum(m_log) * interval + static_cast<int32_t>(m_tail) * bin_size;
	}

public:
	int8_t m_log;
	uint8_t m_tail;
};

//TODO: fix inversion, try make class constexpr
//static_assert((-35475648).get() + short_int32_t(35475648).get() == 0, "inversion is not correct");

inline int32_t operator*(int32_t x, short_int32_t y) {
	return y * x;
}

inline int32_t operator+(int32_t x, short_int32_t y) {
	return y + x;
}

inline int32_t operator-(int32_t x, short_int32_t y) {
	return x - y.get();
}

}

#endif //_SHORT_INT32_H_

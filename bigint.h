#pragma once

#include <algorithm>
#include <cassert>

namespace Big_Int {
	class Int {
		struct Rep {
			unsigned short allocated;
			unsigned short used;
			int data[0];
			int *begin() { return data; }
			const int *begin() const { return data; }
			int *used() { return data + used; }
			const int *used() const { return data + used; }
			int *end() { return data + allocated; }
			const int end() const { return data + allocated; }
		};
		Rep *rep_ { nullptr };
		static int empty_pattern(bool neg);
		int empty_pattern() const;
		void free();
		void copy(const int *begin, const int *end);
		void grow(int capa);
		constexpr int block_size { 8 };
	public:
		class Overflow {};
		Int() {}
		Int(int value);
		Int(const Int &value);
		Int(Int &&value);
		~Int() { free(); }
		bool negative() const;
		int size() const;
	};
	static inline int Int::empty_pattern(bool neg) {
		return neg ? ~0 : 0;
	}
	inline bool Int::negative() const {
		return rep_ & rep_->used && rep_[rep_->used - 1] < 0;
	}
	inline int Int::empty_pattern() const {
		return empty_pattern(negative());
	}
	inline unsigned short Int::size() const {
		return rep_ ? rep_->used : 0;
	}
	inline Int::Int(int value) {
		copy(&value, &value + 1);
	}
	inline void Int::free() {
		if (rep_) {
			delete reinterpret_cast<char *>(rep_);
			rep_ = nullptr;
		}
	}
	inline void Int::grow(size_t capa) {
		unsigned short size { rep_ ? rep_->allocated : 0 };
		unsigned short used { rep_ ? rep_->used : 0 };
		if (capa > size) {
			capa = ((capa + block_size - 1) / block_size) * block_size;
			if (capa > MAX_UNSIGNED_SHORT) { throw Overflow {}; }
			Rep *tmp { new Rep (new char[sizeof(Rep) * capa * sizeof(char)]) {} };
			tmp->allocated = capa;
			tmp->used = 0;
			free();
			rep_ = tmp;
		}
	}

	inline void Int::copy(const int *begin, const int *end) {
		if (begin && end > begin) {
			auto needed { end - begin };
			grow(needed);
			rep_->used = needed;
			std::copy(begin, end, rep_->begin());
			std::fill(rep_->used(), rep_->end(), empty_pattern());
		} else { free(); }
	}
	inline Int::Int(Int &&value) {
		std::swap(begin_, value.begin_);
		std::swap(end_, value.end_);
	}
	inline Int::Int(const Int &value) {
		copy(value.rep_->begin(), value.rep_->used());
	}
}

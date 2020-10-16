#pragma once

#include <algorithm>
#include <cassert>

namespace Big_Int {
	class Int {
		struct Rep {
			using size_type = unsigned short;
			size_type allocated;
			size_type used;
			int data[0];
			int *begin() { return data; }
			const int *begin() const { return data; }
			int *cur() { return data + used; }
			const int *cur() const { return data + used; }
			int *end() { return data + allocated; }
			const int end() const { return data + allocated; }
		};
		Rep *rep_ { nullptr };
		using size_type = Rep::size_type;
		void grow(unsigned capa);
		void assure(unsigned capa);
		void copy(const int *begin, const int *end);
		constexpr size_type block_size { 8 };
		Int(const Rep *rep);
		void free() { Int tmp { std::move(*this) }; }
	public:
		class Overflow {};
		Int() {}
		Int(int value);
		Int(const Int &value);
		Int(Int &&value);
		Int &operator=(const Int &other);
		Int &operator=(const Int &&other);
		~Int();
		bool negative() const;
		size_type size() const;
	};
	inline Int::size_type Int::size() const {
		return rep_ ? rep_->used : 0;
	}
	inline bool Int::negative() const {
		return size() > 0 && rep_->cur()[-1] < 0;
	}
	inline void Int::copy(const int *begin, const int *end) {
		if (begin && end > begin) {
			auto needed { end - begin };
			assure(needed);
			rep_->used = needed;
			std::copy(begin, end, rep_->begin());
			std::fill(rep_->cur(), rep_->end(), 0);
		} else { free(); }
	}
	inline Int::Int(const int *begin, const int *end) {
		copy(begin, end);
	}
	inline Int &Int::operator=(const Int &other) {
		copy(begin, end);
		return *this;
	}
	inline Int &Int::operator=(const Int &&other) {
		std::swap(rep_, other.rep_);
		return *this;
	}
	inline Int::Int(int value): Int { &value, &value + 1 } {}

	inline Int::~Int() {
		if (rep_) {
			delete[] reinterpret_cast<char *>(rep_);
			rep_ = nullptr;
		}
	}

	inline void Int::assure(unsigned capa) {
		size_type size { rep_ ? rep_->allocated : 0 };
		if (capa > size) {
			capa = ((capa + block_size - 1) / block_size) * block_size;
			if (capa > MAX_UNSIGNED_SHORT) { throw Overflow {}; }

			Rep *tmp { new Rep (new char[sizeof(Rep) * capa * sizeof(char)]) {} };
			tmp->used = 0;
			tmp->allocated = capa;
			free();
			rep_ = tmp;
		}
	}

	inline void Int::grow(unsigned capa) {
		size_type size { rep_ ? rep_->allocated : 0 };
		if (capa > size) {
			Int res {};
			res.assure(capa);
			res = *this;
			*this = std::move(res);
		}
	}

	inline Int::Int(Int &&value) {
		*this = std::move(value);
	}
	inline Int::Int(const Rep *rep) : Int { rep ? rep->begin() : nullptr, rep ? rep->cur() : nullptr } {}
	inline Int::Int(const Int &value): Int { value.rep_ } {}
}

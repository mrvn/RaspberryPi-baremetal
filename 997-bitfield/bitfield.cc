#include <stdio.h>
#include <stdint.h>

template<size_t num> struct Bit {
    static constexpr size_t SHIFT = 1;
    static constexpr uint32_t MASK = 1U << num;
    static constexpr uint32_t decode(uint32_t raw) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return (raw & MASK) >> num;
    }
    static constexpr uint32_t encode(uint32_t raw) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return (raw << num) & MASK;
    }
};

template<size_t high, size_t low> struct Bits {
    static constexpr size_t SHIFT = high - low + 1;
    static constexpr uint32_t MASK = ((1U << SHIFT) - 1) << low;
    static constexpr uint32_t decode(uint32_t raw) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return (raw & MASK) >> low;
    }
    static constexpr uint32_t encode(uint32_t raw) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return (raw << low) & MASK;
    }
};

template<typename ... Ts> struct Field;
template<typename T, typename ... Ts>
struct Field<T, Ts ...> {
    static constexpr size_t SHIFT = T::SHIFT + Field<Ts...>::SHIFT;
    static constexpr uint32_t MASK = T::MASK | Field<Ts...>::MASK;
    using Sub = Field<Ts ...>;

    static constexpr uint32_t decode(uint32_t raw) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return (T::decode(raw) << Sub::SHIFT) | Sub::decode(raw);
    }

    static constexpr uint32_t encode(uint32_t raw) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return T::encode(raw >> Sub::SHIFT) | Sub::encode(raw);
    }
};

template<>
struct Field<> {
    static constexpr size_t SHIFT = 0;
    static constexpr uint32_t MASK = 0;
    static constexpr uint32_t decode(uint32_t) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return 0;
    }

    static constexpr uint32_t encode(uint32_t) {
	// printf("%s(%#x)\n", __PRETTY_FUNCTION__, raw);
	return 0;
    }
};

struct PhysAddr {
    explicit constexpr PhysAddr(uint32_t phys) : x(phys) {
	// printf("%s(%#x) = %#x\n", __PRETTY_FUNCTION__, phys, x);
    }
    uint32_t x;
};

template<size_t num>
class BitValue {
public:
    static constexpr uint32_t MASK = Bit<num>::MASK;
    constexpr BitValue(bool v) : raw_(Bit<num>::encode(v ? 1 : 0)) {
	// printf("%s(%d) = %#x\n", __PRETTY_FUNCTION__, v, raw_);
    }
    constexpr BitValue operator !(void) const {
	// printf("%s(): !%#x\n", __PRETTY_FUNCTION__, raw_);
	return BitValue(raw_ == 0);
    }
    uint32_t raw_;
};

template<typename ... Ts>
class FieldValue {
public:
    static constexpr uint32_t MASK = Field<Ts ...>::MASK;
    constexpr FieldValue(uint32_t raw) : raw_(Field<Ts ...>::encode(raw)) {
	// printf("%s(%d) = %#x\n", __PRETTY_FUNCTION__, raw, raw_);
    }
    uint32_t raw_;
};

class Change {
public:
    constexpr Change(uint32_t set, uint32_t mask)
	: set_(set), mask_(mask) {
	// printf("%s(): {%#x, %#x}\n", __PRETTY_FUNCTION__, set_, mask_);
    }
    template<typename T>
    constexpr Change(T t) : set_(t.raw_), mask_(T::MASK) {
	// printf("%s(T): {%#x, %#x}\n", __PRETTY_FUNCTION__, set_, mask_);
    }
    constexpr Change operator +(const Change &other) const {
	// printf("%s(): {%#x, %#x} + {%#x, %#x}\n", __PRETTY_FUNCTION__, set_, mask_, other.set_, other.mask_);
	return Change(set_ | (other.set_ & ~mask_), mask_ | other.mask_);
    }

    template<typename ... Ts>
    static constexpr Change merge(Change change, Ts&& ... ts) {
	// printf("%s()\n", __PRETTY_FUNCTION__);
	return change + merge(ts...);
    }

    static constexpr Change merge() {
	// printf("%s()\n", __PRETTY_FUNCTION__);
	return Change(0, 0);
    }

    uint32_t set_;
    uint32_t mask_;
};

class Entry {
public:
    /*
    struct {
	uint32_t addr       : 20;
	uint32_t not_global : 1;
	uint32_t shared     : 1;
	uint32_t ap2        : 1;
	uint32_t tex        : 3;
	uint32_t ap         : 2;
	uint32_t cached     : 1;
	uint32_t buffered   : 1;
	uint32_t ONE        : 1;
	uint32_t not_exec   : 1;
    };
    */
    using Addr = FieldValue<Bits<31, 12>>;
    static constexpr const BitValue<11> GLOBAL{false};
    static constexpr const BitValue<10> SHARED{true};
    using AP = FieldValue<Bit<9>, Bits<5, 4> >;
    using TEX = FieldValue<Bits<8, 6> >;
    static constexpr const BitValue<3> CACHED{true};
    static constexpr const BitValue<2> BUFFERED{true};
    static constexpr const BitValue<1> SMALL_PAGE{true};
    static constexpr const BitValue<0> EXEC{false};

    template<typename ... Ts>
    constexpr Entry(PhysAddr phys, Ts&& ... ts) : raw_(Change::merge(Addr(phys.x >> 12), ts..., SMALL_PAGE, !GLOBAL, !EXEC).set_) {
	// printf("%s(): raw_ = %#x\n", __PRETTY_FUNCTION__, raw_);
    }

    uint32_t constexpr raw() const { return raw_; }
    uint32_t raw_;
};

const constexpr BitValue<11> Entry::GLOBAL;
const constexpr BitValue<10> Entry::SHARED;
const constexpr BitValue<3> Entry::CACHED;
const constexpr BitValue<2> Entry::BUFFERED;
const constexpr BitValue<1> Entry::SMALL_PAGE;
const constexpr BitValue<0> Entry::EXEC;

int main() {
    PhysAddr phys(0x8000);
    Entry entry(phys, Entry::GLOBAL, Entry::AP(0b100));
    printf("entry = %#x\n", entry.raw());
}

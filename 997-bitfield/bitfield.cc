#include <stdio.h>
#include <stdint.h>

template<size_t num> struct Bit {
    static constexpr size_t SHIFT = 1;
    static constexpr uint32_t MASK = 1U << num;

    static constexpr uint32_t decode(uint32_t raw) {
	return (raw & MASK) >> num;
    }

    static constexpr uint32_t encode(uint32_t raw) {
	return (raw << num) & MASK;
    }
};

template<size_t high, size_t low> struct Bits {
    static constexpr size_t SHIFT = high - low + 1;
    static constexpr uint32_t MASK = ((1U << SHIFT) - 1) << low;

    static constexpr uint32_t decode(uint32_t raw) {
	return (raw & MASK) >> low;
    }

    static constexpr uint32_t encode(uint32_t raw) {
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
	return (T::decode(raw) << Sub::SHIFT) | Sub::decode(raw);
    }

    static constexpr uint32_t encode(uint32_t raw) {
	return T::encode(raw >> Sub::SHIFT) | Sub::encode(raw);
    }
};

template<>
struct Field<> {
    static constexpr size_t SHIFT = 0;
    static constexpr uint32_t MASK = 0;

    static constexpr uint32_t decode(uint32_t) {
	return 0;
    }

    static constexpr uint32_t encode(uint32_t) {
	return 0;
    }
};

struct PhysAddr {
    explicit constexpr PhysAddr(uint32_t phys) : x(phys) { }
    
    uint32_t x;
};

template<size_t num>
class BitValue {
public:
    using B = Bit<num>;
    static constexpr uint32_t MASK = B::MASK;

    explicit constexpr BitValue(bool v = true) : raw_(B::encode(v ? 1 : 0)) { }

    constexpr BitValue operator !(void) const {
	return BitValue(raw_ == 0);
    }

    static constexpr uint32_t decode(uint32_t v) {
	return Bit<num>::decode(v);
    }

    constexpr uint32_t raw() const {
        return raw_;
    }
private:
    uint32_t raw_;
};

template<typename ... Ts>
class FieldValue {
public:
    static constexpr uint32_t MASK = Field<Ts ...>::MASK;

    explicit constexpr FieldValue(uint32_t raw) : raw_(Field<Ts ...>::encode(raw)) { }

    static constexpr uint32_t decode(uint32_t v) {
	return Field<Ts ...>::decode(v);
    }

    constexpr uint32_t raw() const {
        return raw_;
    }
private:
    uint32_t raw_;
};

class Merge {
public:
    constexpr Merge(uint32_t set, uint32_t mask)
	: set_(set), mask_(mask) { }
    
    template<size_t num>
    constexpr Merge(BitValue<num> t) : set_(t.raw()), mask_(t.MASK) { }

    template<typename ... Ts>
    constexpr Merge(FieldValue<Ts ...> t) : set_(t.raw()), mask_(t.MASK) { }
    
    constexpr Merge operator +(const Merge &other) const {
	return Merge(set_ | (other.set_ & ~mask_), mask_ | other.mask_);
    }

    template<typename ... Ts>
    static constexpr Merge merge(Merge first, Ts&& ... ts) {
	return first + merge(ts...);
    }

    static constexpr Merge merge() {
	return Merge(0, 0);
    }
 
    constexpr uint32_t set() const {
        return set_;
    }

    constexpr uint32_t mask() const {
        return mask_;
    }
private:
    uint32_t set_;
    uint32_t mask_;
};

// Generic bitfield in memory, e.g. pagetable entries
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
    using Addr      = FieldValue<Bits<31, 12>>;
    using Global    = BitValue<11>;
    using Shared    = BitValue<10>;
    using Ap        = FieldValue<Bit<9>, Bits<5, 4> >;
    using Tex       = FieldValue<Bits<8, 6> >;
    using Cached    = BitValue<3>;
    using Buffered  = BitValue<2>;
    using Exec      = BitValue<0>;

    static constexpr const Global GLOBAL{false};
    static constexpr const Shared SHARED{true};
    static constexpr const Cached CACHED{true};
    static constexpr const Buffered BUFFERED{true};
    static constexpr const Exec EXEC{false};

    template<typename ... Ts>
    constexpr Entry(PhysAddr phys, Ts&& ... ts)
        : raw_(Merge::merge(Addr(phys.x >> 12), ts ..., DEFAULT).set()) { }

    template<typename T>
    constexpr uint32_t is(T && t) const {
	return t.decode(raw_) == t.decode(t.raw());
    }

    template<typename T>
    constexpr uint32_t get() const {
	return T::decode(raw_);
    }
    
    constexpr uint32_t raw() const {
        return raw_;
    }
private:
    using SmallPage = BitValue<1>;
    static constexpr const SmallPage SMALL_PAGE{true};
    static constexpr const Merge DEFAULT{
        Merge::merge(SMALL_PAGE, !GLOBAL, !EXEC)
    };
    uint32_t raw_;
};

constexpr const Entry::Global Entry::GLOBAL;
constexpr const Entry::Shared Entry::SHARED;
constexpr const Entry::Cached Entry::CACHED;
constexpr const Entry::Buffered Entry::BUFFERED;
constexpr const Entry::Exec Entry::EXEC;
constexpr const Entry::SmallPage Entry::SMALL_PAGE;
constexpr const Merge Entry::DEFAULT;


volatile uint32_t * peripheral_base(uint32_t base, uint32_t reg) {
    (void)base;
    (void)reg;
    asm volatile ("");
    static volatile uint32_t mem = 0;
    return &mem;
}

// peripheral register, e.g. UART data register
class DR {
public:
    using Oe   = BitValue<11>; // Overrun error
    using Be   = BitValue<10>; // Break error
    using Pe   = BitValue< 9>; // Parity error
    using Fe   = BitValue< 8>; // Framing error
    using Data = FieldValue<Bits<7, 0> >; // Receive/Transmit data character
    
    static constexpr const Oe OE{true};
    static constexpr const Be BE{true};
    static constexpr const Pe PE{true};
    static constexpr const Fe FE{true};

    template<typename ... Ts>
    constexpr DR() : ptr_(peripheral_base(0x201000, 0x00)) { }

    template<typename ... Ts>
    void set(Ts ... ts) {
        Merge merge = Merge::merge(ts ...);
        *ptr_ = merge.set();
    }
    
    template<typename ... Ts>
    void modify(Ts ... ts) {
        Merge merge = Merge::merge(ts ...);
        uint32_t t = *ptr_;
        t &= ~merge.mask();
        t |= merge.set();
        *ptr_ = t;
    }
    
    template<typename T>
    constexpr uint32_t is(T && t) const {
	return t.decode(raw()) == t.decode(t.raw());
    }

    template<typename T>
    constexpr uint32_t get() const {
	return T::decode(raw());
    }
    
    uint32_t raw() const {
        return *ptr_;
    }
private:
    volatile uint32_t *ptr_;
};

constexpr const DR::Oe OE;
constexpr const DR::Be BE;
constexpr const DR::Pe PE;
constexpr const DR::Fe FE;

int main() {
    printf("Testing Entry:\n");
    PhysAddr phys(0x8000);
    Entry entry(phys, Entry::GLOBAL, Entry::Ap(0b100));
    printf("  entry = %#x\n", entry.raw());
    if (entry.is(Entry::GLOBAL)) printf("  is global\n");
    printf("  Ap = %d\n", entry.get<Entry::Ap>());
    if (entry.is(Entry::Ap(0b100))) printf("  has Ap(0x100)\n");

    DR dr;
    printf("Testing DR::set():\n");
    dr.set(DR::OE, DR::Data('h'));
    if (dr.is(DR::OE)) printf("  Overrun error\n");
    if (dr.is(DR::BE)) printf("  Break error\n");
    printf("  data = %c\n", dr.get<DR::Data>());
    printf("Testing DR::modify():\n");
    dr.modify(!DR::OE, DR::BE, DR::Data('x'));
    if (dr.is(DR::OE)) printf("  Overrun error\n");
    if (dr.is(DR::BE)) printf("  Break error\n");
    printf("  data = %c\n", dr.get<DR::Data>());
}

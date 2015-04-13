#include <stdio.h>
#include <stdint.h>

struct Raw { } RAW;

template<size_t high, size_t low> class Bits {
public:
    static constexpr size_t SHIFT = high - low + 1;
    static constexpr uint32_t MASK = ((1U << SHIFT) - 1) << low;
    explicit constexpr Bits(uint32_t x) : raw_((x << low) & MASK) { }
    explicit constexpr Bits(Raw, uint32_t x) : raw_(x & MASK) { }
    constexpr uint32_t raw() const { return raw_; }
    constexpr uint32_t decode() const { return raw_ >> low; }
protected:
    uint32_t raw_;
};

template<size_t num> class Bit : public Bits<num, num> {
public:
    explicit constexpr Bit(Raw r, uint32_t x) : Bits<num, num>(r, x) { }
    explicit constexpr Bit(bool x) : Bits<num, num>(x ? 1 : 0) { }
    explicit operator bool() const { return Bits<num, num>::raw_ != 0; }
    constexpr Bit operator !() const { return Bit(Bits<num, num>::raw_ == 0); }
};

template<typename ... Ts> class Field;

template<typename T, typename ... Ts>
class Field<T, Ts ...> {
public:
    static constexpr size_t SHIFT = T::SHIFT + Field<Ts...>::SHIFT;
    static constexpr uint32_t MASK = T::MASK | Field<Ts...>::MASK;
    using Sub = Field<Ts ...>;

    explicit constexpr Field(Raw, uint32_t x) : raw_(x & MASK) { }
    explicit constexpr Field(uint32_t x)
	: raw_(T(x >> Sub::SHIFT).raw() | Sub(x).raw()) { }
    constexpr uint32_t raw() const { return raw_; }
    constexpr uint32_t decode() const {
	return T(raw_).decode() << Sub::SHIFT | Sub(raw_).decode();
    }
private:
    uint32_t raw_;
};

template<>
struct Field<> {
    static constexpr size_t SHIFT = 0;
    static constexpr uint32_t MASK = 0;
    explicit constexpr Field(Raw, uint32_t) { }
    explicit constexpr Field(uint32_t) { }
    constexpr uint32_t raw() const { return 0; }
    constexpr uint32_t decode() const { return 0; }
};

struct PhysAddr {
    explicit constexpr PhysAddr(uint32_t phys) : x(phys) { }
    
    uint32_t x;
};

class Merge {
public:
    constexpr Merge(uint32_t set, uint32_t mask)
	: set_(set), mask_(mask) { }

    template<typename T>
    constexpr Merge(T t) : set_(t.raw()), mask_(t.MASK) { }
    
    constexpr Merge operator +(const Merge &other) const {
	return Merge(set_ | (other.set_ & ~mask_), mask_ | other.mask_);
    }

    template<typename ... Ts>
    static constexpr Merge merge(Merge first, Ts&& ... ts) {
	return first + merge(ts...);
    }

    static constexpr Merge merge() { return Merge(0, 0); }
    constexpr uint32_t set() const { return set_; }
    constexpr uint32_t mask() const { return mask_; }
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
    using Addr      = Bits<31, 12>;
    using Global    = Bit<11>;
    using Shared    = Bit<10>;
    using Ap        = Field<Bit<9>, Bits<5, 4> >;
    using Tex       = Bits<8, 6>;
    using Cached    = Bit<3>;
    using Buffered  = Bit<2>;
    using Exec      = Bit<0>;

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
	return (raw() & t.MASK) == t.raw();
    }

    template<typename T>
    constexpr uint32_t get() const {
	return T(RAW, raw_).decode();
    }
    
    constexpr uint32_t raw() const {
        return raw_;
    }
private:
    using SmallPage = Bit<1>;
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
    using Oe   = Bit<11>; // Overrun error
    using Be   = Bit<10>; // Break error
    using Pe   = Bit< 9>; // Parity error
    using Fe   = Bit< 8>; // Framing error
    using Data = Bits<7, 0>; // Receive/Transmit data character
    
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
	return (raw() & t.MASK) == t.raw();
    }

    template<typename T>
    constexpr uint32_t get() const {
	return T(RAW, raw()).decode();
    }
    
    uint32_t raw() const {
        return *ptr_;
    }
private:
    volatile uint32_t *ptr_;
};

constexpr const DR::Oe DR::OE;
constexpr const DR::Be DR::BE;
constexpr const DR::Pe DR::PE;
constexpr const DR::Fe DR::FE;

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

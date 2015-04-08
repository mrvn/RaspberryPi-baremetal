#include <stdio.h>
#include <stdint.h>

volatile uint32_t mem = 0;

class Reg {
public:
    union Reg_t {
	struct {
	    uint32_t x : 1;
	    uint32_t y : 1;
	    uint32_t z : 1;
	};
	uint32_t raw;
    };
    
    enum Bit {
	BIT_X, BIT_Y, BIT_Z,
    };

    class Change {
    public:
	Change(uint32_t set, uint32_t clear)
	    : set_(set), clear_(clear) {
	    //printf("%s(): {%d, %d}\n", __PRETTY_FUNCTION__, set_, clear_);
	}
	Change(Bit bit) : set_(1U << bit), clear_(0) {
	    //printf("%s(): bit = %d, set_ = %d\n", __PRETTY_FUNCTION__, bit, set_);
	}
	Change operator !(void) const {
	    //printf("%s(): !{%d, %d}\n", __PRETTY_FUNCTION__, set_, clear_);
	    return Change(clear_, set_);
	}
	Change operator +(const Change &other) const {
	    //printf("%s(): {%d, %d} + {%d, %d}\n", __PRETTY_FUNCTION__, set_, clear_, other.set_, other.clear_);
	    return Change(set_ | other.set_, clear_ | other.clear_);
	}
	uint32_t set_;
	uint32_t clear_;
    };
    
    Reg() : reg_((volatile Reg_t *)&mem) { }
    template<typename ... Ts>
    Change merge_changes(Change change, Ts ... changes) {
	//printf("%s\n", __PRETTY_FUNCTION__);
	return change + merge_changes(changes...);
    }
    Change merge_changes(Change change) {
	//printf("%s\n", __PRETTY_FUNCTION__);
	return change;
    }
    template<typename ... Ts>
    void set(Ts ... changes) {
	//printf("%s\n", __PRETTY_FUNCTION__);
	Change change = merge_changes(changes...);
	uint32_t t = reg_->raw;
	t &= ~change.clear_;
	t |= change.set_;
	reg_->raw = t;
    }

    uint32_t raw() const { return reg_->raw; }
    volatile Reg_t *reg_;
};

Reg::Change operator !(Reg::Bit bit) {
    return !Reg::Change(bit);
}

int main() {
    Reg reg;
    reg.set(Reg::BIT_X, Reg::BIT_Y, Reg::BIT_Z);
    printf("reg = %x\n", reg.raw());
    reg.set(!Reg::BIT_X);
    printf("reg = %x\n", reg.raw());
}

DIRS := $(wildcard */)

all: $(patsubst %,all-%,$(DIRS))
clean: $(patsubst %,clean-%,$(DIRS))

all-%:
	$(MAKE) -C $* all

clean-%:
	$(MAKE) -C $* clean
	rm -f *~

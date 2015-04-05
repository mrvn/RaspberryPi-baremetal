.SUFFIXES:

ifndef ARCH
$(error ARCH is not set)
endif
export ARCH

OBJDIR := _$(ARCH)

.PHONY: $(OBJDIR)
$(OBJDIR):
	+@[ -d $@ ] || mkdir -p $@
	+@$(MAKE) --no-print-directory -C $@ -f $(CURDIR)/Makefile \
	          SRCDIR=$(CURDIR) $(MAKECMDGOALS)

Makefile : ;
%.mk :: ;

% :: $(OBJDIR) ; :

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

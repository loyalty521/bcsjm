-include ../../build/Rules.make

all:
	@$(MAKE) -C build/gcc CROSS=$(GCC_PREFIX) CCFLAGS_ADD=-I../../../IoService/src $(BUILDTYPE)_static
clean:
	@$(MAKE) -C build/gcc CROSS=$(GCC_PREFIX) clean

install: all
	@cp build/gcc/$(BUILDTYPE)/libNcp.a $(INSTALL_DIR)

BUILD_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
TARGET_SHORT := libNcp


CROSS ?= arm-none-linux-gnueabi-

GCC := $(CROSS)gcc
AR := $(CROSS)ar
LD := $(CROSS)ld
STRIP := $(CROSS)strip

INCLUDES :=

CCFLAGS_ADD ?=
CCFLAGS := $(CCFLAGS_ADD) -Wall -c -isystem -Wno-psabi --std=c++11 -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD -I$(BUILD_DIR)/../../lib/IoService/src
LDFLAGS := -shared -fPIC -lstdc++ -lc 
ARFLAGS := -r
MAKEFLAGS := --warn-undefined-variables --no-print-directory

HEADERS := $(wildcard $(BUILD_DIR)/../../src/*.h) $(wildcard $(BUILD_DIR)/../../src/Client/*.h)
SOURCES := $(wildcard $(BUILD_DIR)/../../src/*.cpp) $(wildcard $(BUILD_DIR)/../../src/Client/*.cpp)

DEBUG_OBJS := $(SOURCES:$(BUILD_DIR)/../../src/%.cpp=$(BUILD_DIR)/debug/%.o)
RELEASE_OBJS := $(SOURCES:$(BUILD_DIR)/../../src/%.cpp=$(BUILD_DIR)/release/%.o)

DEBUG_DEPS := $(DEBUG_OBJS:%.o=%.d)
RELEASE_DEPS := $(RELEASE_OBJS:%.o=%.d)

TARGET_DEBUG_DYNAMIC := $(BUILD_DIR)/debug/$(TARGET_SHORT).so
TARGET_RELEASE_DYNAMIC := $(BUILD_DIR)/release/$(TARGET_SHORT).so
TARGET_DEBUG_STATIC := $(BUILD_DIR)/debug/$(TARGET_SHORT).a
TARGET_RELEASE_STATIC := $(BUILD_DIR)/release/$(TARGET_SHORT).a

all: debug_dynamic release_dynamic debug_static release_static

debug_dynamic: $(TARGET_DEBUG_DYNAMIC) 
release_dynamic: $(TARGET_RELEASE_DYNAMIC) 
debug_static: $(TARGET_DEBUG_STATIC) 
release_static: $(TARGET_RELEASE_STATIC) 

-include $(DEBUG_DEPS)
-include $(RELEASE_DEPS)

$(DEBUG_DEPS): $(BUILD_DIR)/debug/%.d: $(BUILD_DIR)/../../src/%.cpp
	@mkdir -p $(dir $@)
	@echo [GCC] $@
	@echo "$(dir $@)\c" > $@; \
	$(GCC) -MM $< $(CCFLAGS) -g -rdynamic -D_DEBUG >> $@; \
	echo "\t@echo [GCC] \$$<" >> $@; \
	echo "\t@\$$(GCC) \$$(CCFLAGS) -g -rdynamic -D_DEBUG -o \$$@ \$$<" >> $@

$(RELEASE_DEPS): $(BUILD_DIR)/release/%.d: $(BUILD_DIR)/../../src/%.cpp
	@mkdir -p $(dir $@)
	@echo [GCC] $@
	@echo "$(dir $@)\c" > $@; \
	$(GCC) -MM $< $(CCFLAGS) -O3 -DNDEBUG >> $@; \
	echo "\t@echo [GCC] \$$<" >> $@; \
	echo "\t@\$$(GCC) \$$(CCFLAGS) -O3 -DNDEBUG -o \$$@ \$$<" >> $@
	
$(TARGET_DEBUG_DYNAMIC) : $(DEBUG_OBJS)
	@echo "Creating $@..."
	$(GCC) $(LDFLAGS) -o $@ $^ $(BUILD_DIR)/../../lib/IoService/build/gcc/debug/libIoService.so

$(TARGET_RELEASE_DYNAMIC) : $(RELEASE_OBJS)
	@echo "Creating $@..."
	$(GCC) $(LDFLAGS) -O3 -o $@ $^	$(BUILD_DIR)/../../lib/IoService/build/gcc/release/libIoService.so
	@$(STRIP) $@

$(TARGET_DEBUG_STATIC) : $(DEBUG_OBJS)
	@echo "Creating $@..."
	@$(AR) $(ARFLAGS) -o $@ $^	
	
$(TARGET_RELEASE_STATIC) : $(RELEASE_OBJS)
	@echo "Creating $@..."
	@$(AR) $(ARFLAGS) -o $@ $^	

Doxygen:
	@echo ---------------Create Doxygen Document -------------
	@doxygen 

clean:
	@rm $(TARGET_DEBUG_DYNAMIC) $(TARGET_RELEASE_DYNAMIC) $(TARGET_DEBUG_STATIC) $(TARGET_RELEASE_STATIC) -rf



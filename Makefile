OBJ_DIR         = ./obj
DEPEND_FILE     = make.d

CXXFLAGS        += -c -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE
CXXFLAGS        += -Wall -W -Wno-format-y2k -Wpointer-arith -Wreturn-type -Wcast-qual -Wwrite-strings
CXXFLAGS        += -Wswitch -Wshadow -Wcast-align -Wuninitialized -Wformat=2
CFLAGS          += -Wstrict-prototypes -Wmissing-prototypes

RELEASE_FLAGS   = -O3 -DNDEBUG
DEBUG_FLAGS     = -O3 -g -DDEBUG

TARGET          = sample

LDFLAGS         += -lstdc++
INCLUDE_FLAGS   = -I ./include


CMD_PREFIX      =
STRIP           = $(CMD_PREFIX)strip
CC              = $(CMD_PREFIX)gcc
CXX             = $(CMD_PREFIX)g++
AR              = $(CMD_PREFIX)ar
MAKE            = $(CMD_PREFIX)make
VPATH           = $(OBJ_DIR)

ifeq ($(strip $(RELEASE)), y)
 CXXFLAGS       += $(RELEASE_FLAGS)
else
 CXXFLAGS       += $(DEBUG_FLAGS) 
endif

CXXFLAGS        += $(INCLUDE_FLAGS)
CFLAGS          += $(CXXFLAGS)

CXX_SRCS        = $(shell find -name "*.cpp")
CC_SRCS         = $(shell find -name "*.c")
OBJS            = ${CXX_SRCS:.cpp=.o} ${CC_SRCS:.c=.o}

all: depend $(OBJS)
	$(CXX) $(addprefix $(OBJ_DIR)/, $(OBJS)) $(LDFLAGS) -o $(TARGET)

release:
	@$(MAKE) -f Makefile RELEASE=y all

clean: depend_clean
	@rm -f $(addprefix $(OBJ_DIR)/, $(OBJS)) $(TARGET)

depend: depend_clean
ifneq ($(strip $(CXX_SRCS)),)
	@$(CXX) $(CXXFLAGS) -M $(CXX_SRCS) >> $(DEPEND_FILE)
endif
ifneq ($(strip $(CC_SRCS)),)
	@$(CC) $(CFLAGS) -M $(CC_SRCS) >> $(DEPEND_FILE)
endif

depend_clean:
	@rm -rf ./$(DEPEND_FILE)

.cpp.o:
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $(OBJ_DIR)/$@

.c.o:
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	$(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$@

-include $(DEPEND_FILE)

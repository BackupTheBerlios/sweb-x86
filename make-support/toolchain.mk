# $Id: toolchain.mk,v 1.1 2005/04/12 17:43:25 nomenquis Exp $
#
# $Log:  $
#
#

#ok, renicing the build is always nice
NICE := nice -n 19 
MAKE_TEMP := make 

# no one defined any cxx? Good, use gcc
ifeq ($(SWEB_USE_CXX),)
SWEB_USE_CXX := gcc
endif

# currently only gcc works
# we should definitely add something here to force
# 32 bit generation on 64 bit build systems

ifeq ($(SWEB_USE_CXX),gcc)
CXX_TEMP := g++ 
CC_TEMP := gcc 
CXX_DEP_TEMP := g++
CC_DEP_TEMP := gcc
AS_TEMP := nasm -f elf
AS_DEP_TEMP := nasm -f elf
LD_TEMP := g++
AR_TEMP := ar
endif

# only c++ and c code is distccd
ifeq ($(SWEB_USE_DISTCC),1)
CXX_TEMP := distcc $(CXX_TEMP)
CC_TEMP := distcc $(CC_TEMP)
endif

# nice this sucker
ifeq ($(SWEB_RENICE_BUILD),1)
CXX_TEMP := $(NICE) $(CXX_TEMP)
CC_TEMP := $(NICE) $(CC_TEMP)
AS_TEMP := $(NICE) $(AS_TEMP)
LD_TEMP := $(NICE) $(LD_TEMP)
AR_TEMP := $(NICE) $(AR_TEMP)
CXX_DEP_TEMP := $(NICE) $(CXX_DEP_TEMP)
CC_DEP_TEMP := $(NICE) $(CC_DEP_TEMP)
AS_DEP_TEMP := $(NICE) $(AS_DEP_TEMP)
MAKE_TEMP := $(NICE) $(MAKE_TEMP)
endif

CXX := $(CXX_TEMP)
CC := $(CC_TEMP)
AS := $(AS_TEMP)
LD := $(LD_TEMP)
AR := $(AR_TEMP)
CXX_DEP := $(CXX_DEP_TEMP)
CC_DEP := $(CC_DEP_TEMP)
AS_DEP := $(AS_DEP_TEMP)
MAKE := $(MAKE_TEMP)


ifeq ($(SWEB_USE_CXX),gcc)
CXXFLAGS := $(CXXFLAGS) -O3 -g -Wno-deprecated
CCFLAGS := $(CCFLAGS) -O3 -g
ASFLAGS := $(ASFLAGS) 
LDFLAGS := $(LDFLAGS) -rdynamic -Wl,--no-keep-memory
endif

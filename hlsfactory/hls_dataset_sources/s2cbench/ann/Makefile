#===========================================================================
#
# File Name    : Makefile
# Description  : Makefile for ANNN
# Release Date : 26/06/14
# Author       : PolyU
#
# Note         : Edit SYSTEMC_HOME path
# Revision History
#---------------------------------------------------------------------------
# Date      Version    Author           Description
#---------------------------------------------------------------------------
#26/06/14        1.0   PolyU, DARClab   Makefile for ANN
#============================================================================

TARGET	= ann.exe

SRCS	=  \
	main.cpp \
	ann_tb.cpp \
	layer.cpp \
	ann.cpp

HDRS	=  \
	ann_tb.h \
	layer.h \
	image_parameters.h \
	config.h \
	train_parameters.h \
	synth_parameters.h \
	ann.h

ifneq (, $(wildcard /bin/uname))
CPU_ARCH = $(shell /bin/uname -m)
endif

ifeq ($(CPU_ARCH),x86_64)
CWB_LIB   = $(CYBER_PATH)/lib64
ARCH_FLAG =
else
CWB_LIB   = $(CYBER_PATH)/lib
ARCH_FLAG = -m32
endif

ifdef CYBER_SYSTEMC_TARGET_ARCH
CWB_SC_ARCH = $(CYBER_SYSTEMC_TARGET_ARCH)
else
ifeq (, $(findstring Windows,$(OS)))
ifeq ($(CPU_ARCH),x86_64)
CWB_SC_ARCH = linux64
else
CWB_SC_ARCH = linux
endif
else
CWB_SC_ARCH = mingw
endif
endif

ifdef CYBER_SYSTEMC_HOME
CWB_SC_HOME = $(CYBER_SYSTEMC_HOME)
else
CWB_SC_HOME = $(CYBER_PATH)/osci
endif

CWB_SC_INCL = -I"$(CWB_SC_HOME)/include"
CWB_SC_LIB  = "$(CWB_SC_HOME)/lib-$(CWB_SC_ARCH)/libsystemc.a"


EMPTY   =
SPACE   = $(EMPTY) $(EMPTY)
ifeq (, $(wildcard  $(subst $(SPACE),\$(SPACE),$(CYBER_PATH))/gcc/bin))
CC      = g++
RM      = rm
else
CC      = "$(CYBER_PATH)/gcc/bin/g++"
RM      = "$(CYBER_PATH)/gcc/bin/rm"
endif
LINKER  = $(CC)
INCL    = -I"."
CFLAGS  = $(ARCH_FLAG) -O1 -DTRAIN
LDFLAGS = $(ARCH_FLAG)
LIBS    = -lm $(CWB_SC_LIB)  -Wl,-rpath="$(CWB_LIB)"
OBJS    = $(notdir $(addsuffix .o,$(basename $(SRCS))))
debug: CFLAGS += -g -DDEBUG -DPRINTD
wave: CFLAGS += -DWAVE_DUMP
layers4: CFLAGS += -DNLAYER_4


$(TARGET) : $(OBJS)
	$(LINKER) -o "$@" $(LDFLAGS) $(OBJS) $(LIBS)

debug: $(OBJS)
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS)

wave:  $(OBJS) 
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS)

layers4:  $(OBJS) 
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) $(CWB_SC_INCL) $(INCL)  -c $< -o $@


ann_tb.o: ann_tb.cpp
	$(CC) $(CFLAGS) $(CWB_SC_INCL) $(INCL) -c $< -o $@

layer.o: layer.cpp
	$(CC) $(CFLAGS) $(CWB_SC_INCL) $(INCL) -c $< -o $@

ann.o: ann.cpp
	$(CC) $(CFLAGS) $(CWB_SC_INCL) $(INCL) -c $< -o $@


clean :
	@$(RM) -f $(OBJS) $(TARGET)

depend :
	makedepend -- $(INCL) $(CFLAGS) -- -Y -fMakefile $(SRCS)


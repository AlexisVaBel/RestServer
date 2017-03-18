TARGET		=http_fileserv
#dir
# ========== can change src,obj,bin ==========
SRCDIR		=.
INCDIR		=$(SRCDIR)
#INCDIR		+=//opt/casablanca/Release/include
SYSINC      	=/usr/include
SYSINC      	+=/opt/casablanca/Release/include
OBJDIR		=./bin
# ========== can change src,obj,bin ==========
#~dir

#Tools
CC			=  gcc
CX			=  g++-4.8
LD			=  g++-4.8
 
#~Tools

DEFINES		= 

CFLAGS		=  -ggdb -O0 -D_LINUX -std=c99
CXXFLAGS	=  -static -Wall -W -g -D_LINUX -o0 
CXXFLAGS	+= -std=c++11
CXXFLAGS	+= -fopenmp
LDFLAGS		=  -Wall

# ========== 	don`t change		==========

ASOURCES	= $(shell find -L $(SRCDIR) -name '*.s')
CSOURCES	= $(shell find -L $(SRCDIR) -name '*.c')
CXXSOURCES	= $(shell find -L $(SRCDIR) -name '*.cpp')

#INCPATH		= $(shell find -L $(CMSIS)  -name '*.h' -exec dirname {} \; | uniq)
INCPATH		+=$(shell find -L $(INCDIR) -name '*.h' -exec dirname {} \; | uniq)
#INCPATH		+=$(shell find -L $(INCDIR) -name '*.hpp' -exec dirname {} \; | uniq)
#INCPATH		+=$(shell find -L $(SYSINC) -name '*.h' -exec dirname {} \; | uniq)

SOURCES		=  $(CSOURCES)
SOURCES		+= $(ASOURCES)
SOURCES		+= $(CXXSOURCES)
#OBJECTS		=  $(CSOURCES:%.c=%.o)
#OBJECTS		+= $(ASOURCES:%.s=%.o)
OBJECTS		+= $(patsubst %.cpp,%.o,$(CXXSOURCES))
OBJECTS		+= $(patsubst %.c,%.o,$(CSOURCES))
OBJECTS		+= $(patsubst %.s,%.o,$(ASOURCES))
INCLUDES	=  $(INCPATH:%=-I%)
INCLUDES	+= $(SYSINC:%=-I%)
LIBS		=-L//opt/casablanca/Release/build.Release/Binaries  -lcpprest
LIBS		+=-lboost_log -lboost_thread -lboost_date_time -lboost_system -lboost_filesystem -lpthread -lcrypto  -lssl
LIBS		+= -Wl,-R/opt/casablanca/Release/build.Release/Binaries
.PHONY: all clean

all: $(SOURCES) $(OBJECTS) $(TARGET)	



$(TARGET):$(OBJECTS) 
#	@echo	'Make execut'
	$(LD) $(LDFLAGS)  $(OBJECTS) -g -o  $(OBJDIR)/$@  $(LIBS)

$(OBJECTS): | $(OBJDIR)
#	echo $(OBJECTS)

$(OBJDIR):
	mkdir -p $(OBJDIR)
		


# $@ name of the object file
# $< name of src file
%.o: %.cpp
	$(CX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@
        
%.o : %.c
	$(CC) -c $(CFLAGS) $(INCLUDES)   $< -o $@	        

$(OBJDIR)/%.o: %.s
	$(AS)	-c $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJDIR)/%.o: %.S
	$(AS)	-c $(CFLAGS) $(INCLUDES) $< -o $@	
    
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
# ========== 	don`t change		==========	

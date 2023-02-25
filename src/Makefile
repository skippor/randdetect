TARGET = randdetect

INCLUDE = ../include

#Mips
CC    = $(CROSS_COMPILE)gcc 
CXX   = $(CROSS_COMPILE)g++ 
STRIP = $(CROSS_COMPILE)strip



ifeq ($(ARCH),mips)
	CFLAGS   +=  -DNDEBUG -DPLAT_ARM  -g $(INCLUDE:%=-I%) -Wall   -Wno-deprecated
	CXXFLAGS +=  -DNDEBUG -DPLAT_ARM -g $(INCLUDE:%=-I%) -Wall   -Wno-deprecated
	LDFLAGS  += 
else
	DPLAT = -DPLAT_X86
	CFLAGS +=   $(DPLAT) -g -DNDEBUG $(INCLUDE:%=-I%)  
	CXXFLAGS += $(DPLAT) -g -DNDEBUG $(INCLUDE:%=-I%) 
	LDFLAGS += 
endif

#get all *.cpp
CPP_SOURCES = $(wildcard *.cpp)
C_SOURCES = $(wildcard *.c)

#get all *.o
CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))
ALL_OBJS = $(CPP_OBJS) $(C_OBJS)

all: $(ALL_OBJS)
	$(CXX)  -o $(TARGET) $^  $(LDFLAGS)
	cp $(TARGET) $(TARGET).g
	echo $(SHAREDHDR)
	$(STRIP) $(TARGET)
	
#main.o: main.cpp
#	$(CXX) -c $< -o $@

cppscan:
	cpptesttrace --cpptesttraceOutputFile=$(TARGET).bdf --cpptesttraceProjectName=$(TARGET) make || echo
	cpptestcli -data /home/cpptest/workspace -resource $(TARGET) -bdf $(TARGET).bdf -config user://mustfix -report /home/cpptest/report || echo
	cppcheck $(INCLUDES) ./ --xml 2>/home/cpptest/report/cppcheck.xml
	
clean:
	rm -f $(ALL_OBJS)
	rm -f $(TARGET)
	rm -f $(TARGET).g

install:
	cp -af $(TARGET) $(PACK_ROOT_DIR)/app1/app/usr/sbin/$(TARGET)

CXX:=$(shell sh -c 'type $(CXX) >/dev/null 2>/dev/null && echo $(CXX) || echo g++')

MYSQL_C_INC ?= $(shell mysql_config --cflags)
MYSQL_C_LIBS ?= $(shell mysql_config --libs)

GMIME_INCLUDES ?=  $(shell pkg-config gmime-2.6 --cflags)
GMIME_LIBS ?=  $(shell pkg-config gmime-2.6 --libs)

CLD_INCLUDES = -I/usr/local/include/cld
CLD_LIBS = -lcld2

OPTIMIZATION = -O0

WARNINGS =     \
                -Wall \
                -Wno-sign-compare \
                -Wno-deprecated-register \
                -Wno-unused-function

DEBUG = -g

STDC = -std=c++1y

LDFLAGS = -lstdc++ -pthread

CXXFLAGS = $(OPTIMIZATION) -fPIC -fstack-protector $(CFLAGS) $(WARNINGS) $(DEBUG) $(STDC) -DUSE_CXX0X

STDLIB =
ifeq ($(shell uname -s), FreeBSD)
STDLIB +=  -stdlib=libc++
LIBXML_INCLUDES =
LIBXML_INCLUDES = -I/usr/local/include/libxml2
endif
CXXFLAGS +=  $(STDLIB)

CFLAGS=-c -Wall

SOURCES = async_daemon.cpp \
             HUtils/HStrings.cpp \
             HUtils/BTreeStore.cpp \
             HUtils/BTreeStoreCPP.cpp \
             HUtils/MLocker.cpp \
             HUtils/AsyncQueue.cpp \
             HUtils/HFiles.cpp \
             HUtils/HDaemon.cpp \
             AsyncEvent/SMTP/AEvConnection.cpp \
             AsyncEvent/SMTP/AEvAcceptor.cpp \
             AsyncEvent/SMTP/SmtpBuffer.cpp \
             AsyncEvent/SMTP/SmtpTypes.cpp \
             AsyncEvent/SMTP/SmtpSession.cpp \
             AsyncEvent/DNS/DnsBuffer.cpp \
             AsyncEvent/DNS/AEvDnsClient.cpp \
             AsyncEvent/DNS/DnsUtils.cpp \
             AsyncEvent/SysSig/AEvSysSig.cpp \
             AsyncEvent/AEvBase/AEventAbstract.cpp \
             AsyncEvent/AEvBase/AEventUtilBase.cpp \
             AsyncEvent/BufferBase/PBufferAbstract.cpp \
             AsyncEvent/BufferBase/BuffAbstract.cpp \
             AsyncEvent/Redis/RedisBuffer.cpp \
             AsyncEvent/Redis/RESProto.cpp \
             AsyncEvent/Redis/RESPBuffer.cpp \
             AsyncEvent/Redis/AEvRedisCli.cpp \
             Logger/Logger.cpp \
             Config/GlobalConf.cpp

EXECUTABLE = aevdaemon

OBJECTS=$(SOURCES:.cpp=.o)

TESTING_SOURCES = testing.cpp \
             HUtils/HStrings.cpp \
             HUtils/BTreeStore.cpp \
             HUtils/BTreeStoreCPP.cpp \
             HUtils/MLocker.cpp \
             HUtils/AsyncQueue.cpp \
             HUtils/HFiles.cpp \
             HUtils/HDaemon.cpp \
             AsyncEvent/BufferBase/PBufferAbstract.cpp \
             AsyncEvent/BufferBase/BuffAbstract.cpp \
             AsyncEvent/Redis/RedisBuffer.cpp \
             AsyncEvent/Redis/RESProto.cpp \
	     AsyncEvent/Redis/AEvRedisMod.cpp \
             AsyncEvent/Redis/RESPBuffer.cpp \
             AsyncEvent/Redis/query.cpp \
             Logger/Logger.cpp \
             Config/GlobalConf.cpp \
             Atomic/AtomicTypes.cpp

TESTING_EXEC = testing

TESTING_OBJ=$(TESTING_SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE) $(TESTING_EXEC)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

$(TESTING_EXEC): $(TESTING_OBJ) 
	$(CXX) $(LDFLAGS) $(TESTING_OBJ) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean: 
	rm $(OBJECTS) $(EXECUTABLE) $(TESTING_OBJ) $(TESTING_EXEC)


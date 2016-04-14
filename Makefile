CXX ?= clang++
CC ?= clang
LD ?= clang
INSTALL = install

MYSQL_CPP_INCLUDES ?= -I/usr/local/include/cppconn -I/usr/local/include
MYSQL_CPP_LIBS ?= -lmysqlcppconn

MYSQL_C_INC ?= $(shell mysql_config --cflags)
MYSQL_C_LIBS ?= $(shell mysql_config --libs)

GMIME_INCLUDES ?=  $(shell pkg-config gmime-2.6 --cflags)
GMIME_LIBS ?=  $(shell pkg-config gmime-2.6 --libs)

LIBXML_INCLUDES ?= -I/usr/include/libxml2
LIBXML_LIBS ?= -lxml2

CLD_INCLUDES = -I/usr/local/include/cld
CLD_LIBS = -lcld2

CXXFLAGS += -fPIC -g -fstack-protector -DUSE_CXX0X -std=c++1y
#CXXFLAGS += -DASIO_STANDALONE -DASIO_CPP11_DATE_TIME -fPIC -g -fstack-protector -DUSE_CXX0X -std=c++1y

STDLIB =
ifeq ($(shell uname -s), FreeBSD)
STDLIB +=  -stdlib=libc++
LIBXML_INCLUDES =
LIBXML_INCLUDES = -I/usr/local/include/libxml2
endif
CXXFLAGS +=  $(STDLIB)

CXXFLAGS +=     \
                -Wall \
                -Wno-sign-compare \
                -Wno-deprecated-register \
                -Wno-unused-function

ASYNCD_LDLIBS = -lstdc++ -pthread

ASYNCD_INCLUDES = 

ASYNCD_NAME = async_daemon

ASYNCD_SOURCES = async_daemon.cpp \
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
             AsyncEvent/Redis/AEvRedisCli.cpp \
             Logger/Logger.cpp \
             Config/GlobalConf.cpp


TESTING_LDLIBS = -lstdc++ $(MYSQL_C_LIBS)

TESTING_INCLUDES = $(MYSQL_C_INC)

TESTING_NAME = testing

TESTING_SOURCES = testing.cpp \
             HUtils/HStrings.cpp \
             HUtils/BTreeStore.cpp \
             HUtils/BTreeStoreCPP.cpp \
             HUtils/MLocker.cpp \
             HUtils/AsyncQueue.cpp \
             HUtils/HFiles.cpp \
             HUtils/HDaemon.cpp \
             HUtils/HFiles.cpp \
             AsyncEvent/BufferBase/PBufferAbstract.cpp \
             AsyncEvent/BufferBase/BuffAbstract.cpp \
             AsyncEvent/Redis/RedisBuffer.cpp \
             Logger/Logger.cpp \
             Config/GlobalConf.cpp


all : $(TESTING_NAME)
# $(ASYNCD_NAME)

$(ASYNCD_NAME): $(ASYNCD_SOURCES)
	$(CXX) $(ASYNCD_INCLUDES) $(CXXFLAGS) -o $@ $^ $(ASYNCD_LDLIBS)

$(TESTING_NAME): $(TESTING_SOURCES)
	$(CXX) $(TESTING_INCLUDES) $(CXXFLAGS) -o $@ $^ $(TESTING_LDLIBS)

install: $(LIBNAME)
	$(INSTALL) $(ASYNCD_NAME) /usr/local/bin

clean:
	rm -rf $(ASYNCD_NAME)
	rm -rf $(TESTING_NAME)


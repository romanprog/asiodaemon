CXX ?= clang++
CC ?= clang
LD ?= clang
INSTALL = install

MYSQL_CPP_INCLUDES ?= -I/usr/local/include/cppconn -I/usr/local/include
MYSQL_CPP_LIBS ?= -lmysqlcppconn

GMIME_INCLUDES ?=  $(shell pkg-config gmime-2.6 --cflags)
GMIME_LIBS ?=  $(shell pkg-config gmime-2.6 --libs)

LIBXML_INCLUDES ?= -I/usr/include/libxml2
LIBXML_LIBS ?= -lxml2

CLD_INCLUDES = -I/usr/local/include/cld
CLD_LIBS = -lcld2

CXXFLAGS += -fPIC -g -fstack-protector -DUSE_CXX0X -std=c++11

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
             HelpfulCodes/HStrings.cpp \
             HelpfulCodes/BTreeStore.cpp \
             HelpfulCodes/BTreeStoreCPP.cpp \
             HelpfulCodes/MLocker.cpp \
             HelpfulCodes/AsyncQueue.cpp \
             HelpfulCodes/HFiles.cpp \
             AsioServer/AsyncSigListen.cpp \
             HelpfulCodes/HDaemon.cpp

ARENA_LDLIBS = -lstdc++ -pthread

ARENA_INCLUDES =

ARENA_NAME = arena

ARENA_SOURCES = arena.cpp \
             HelpfulCodes/HStrings.cpp \
             HelpfulCodes/BTreeStore.cpp \
             HelpfulCodes/BTreeStoreCPP.cpp \
             HelpfulCodes/MLocker.cpp \
             HelpfulCodes/AsyncQueue.cpp \
             HelpfulCodes/HFiles.cpp \
             AsioServer/AsyncSigListen.cpp \
             HelpfulCodes/HDaemon.cpp


all : $(ASYNCD_NAME) $(ARENA_NAME)

$(ASYNCD_NAME): $(ASYNCD_SOURCES)
	$(CXX) $(ASYNCD_INCLUDES) $(CXXFLAGS) -o $@ $^ $(ASYNCD_LDLIBS)

$(ARENA_NAME): $(ARENA_SOURCES)
	$(CXX) $(ARENA_INCLUDES) $(CXXFLAGS) -o $@ $^ $(ARENA_LDLIBS)

install: $(LIBNAME)
	$(INSTALL) $(ASYNCD_NAME) /usr/local/bin

clean:
	rm -rf $(ASYNCD_NAME)


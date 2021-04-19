bin_PROGRAMS=gtwupload
gtwupload_SOURCES=FTPClient.cpp FtpAgent.cpp MessageQueue.cpp GLog.cpp gtwupload.cpp

if DEBUG
  AM_CFLAGS = -g3 -O0 -Wall -DNDEBUG
  AM_CXXFLAGS = -g3 -O0 -Wall -DNDEBUG
else
  AM_CFLAGS = -O3 -Wall
  AM_CXXFLAGS = -O3 -Wall
endif

gtwupload_LDFLAGS = -L/usr/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_date_time-mt -lboost_filesystem-mt -lboost_chrono -lboost_regex-mt
gtwupload_LDADD = ${BOOST_LIBS} -lpthread 
if LINUX
gtwupload_LDADD += -lboost_thread -lrt
endif
if OSX
gtwupload_LDADD += -lboost_thread-mt
endif

bin_PROGRAMS=gtwupload
gtwupload_SOURCES=FTPManager.cpp FtpAgent.cpp GLog.cpp  \
                  daemon.cpp gtwupload.cpp

if DEBUG
  AM_CFLAGS = -g3 -O0 -Wall -DNDEBUG
  AM_CXXFLAGS = -g3 -O0 -Wall -DNDEBUG
else
  AM_CFLAGS = -O3 -Wall
  AM_CXXFLAGS = -O3 -Wall
endif

gtwupload_LDFLAGS = -L/usr/local/lib
BOOST_LIBS_OSX   = -lboost_system-mt -lboost_date_time-mt -lboost_filesystem-mt -lboost_chrono -lboost_regex-mt -lboost_thread-mt
BOOST_LIBS_LINUX = -lboost_system-mt-x64 -lboost_date_time-mt-x64 -lboost_filesystem-mt-x64 -lboost_chrono-mt-x64 -lboost_regex-mt-x64 -lboost_thread-mt-x64
gtwupload_LDADD = -lpthread 
if LINUX
gtwupload_LDADD += ${BOOST_LIBS_OSX} -lrt
endif
if OSX
gtwupload_LDADD += ${BOOST_LIBS_OSX}
endif

include_directories(${ROOT_DIR}/3rdparty/mysqlpp/mysql)

if(WIN32)
#    BUILD_PLUGIN_MACRO(AFMysqlPlugin /usr/local/lib/libmysqlpp.so)
elseif(APPLE)
    BUILD_PLUGIN_MACRO(AFMysqlPlugin /usr/local/lib/libmysqlpp.dylib)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFMysqlPlugin /usr/local/lib/libmysqlpp.so)
endif()
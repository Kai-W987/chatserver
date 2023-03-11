#添加源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./database DB_LIST)
aux_source_directory(./redis REDIS_LIST)

#可执行文件
add_executable(server ${SRC_LIST} ${DB_LIST} ${REDIS_LIST})

#连接库
target_link_libraries(server muduo_net muduo_base mysqlclient hiredis pthread)
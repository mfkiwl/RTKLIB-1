add_executable(rtkrcv rtkrcv.c vt.c)
target_link_libraries(rtkrcv RTKLib)
project(c-server)

add_executable(server main.c request.c response.c log.c fcgi.c io.c)
add_executable(request_test "test/resquest_test.c" request.c)
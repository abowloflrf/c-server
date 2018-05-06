project(c-server)

add_executable(server main.c request.c response.c log.c fcgi.c fcgi.h io.c io.h)
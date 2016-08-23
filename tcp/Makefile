OBJ1 = tcp_cli
OBJ2 = tcp_srv

all: $(OBJ1) $(OBJ2)

$(OBJ1): tcp_cli.o common.o
	gcc tcp_cli.o common.o -o $(OBJ1)

tcp_cli.o: tcp_cli.c common.h
	gcc -g -c tcp_cli.c

common.o: common.c common.h
	gcc -g -c common.c

$(OBJ2): tcp_srv.o common.o
	gcc tcp_srv.o common.o -o $(OBJ2)

tcp_srv.o: tcp_srv.c common.h
	gcc -g -c tcp_srv.c

clean:
	rm -v *.o $(OBJ1) $(OBJ2)

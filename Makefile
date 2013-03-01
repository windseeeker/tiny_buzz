src_file = sock.c  http_sock.c main.c http.c
src_obj = sock.o http_sock.o main.o http.o

buzz: $(src_file) $(src_obj)
	gcc -c -g $(src_file)
	gcc -o buzz -lpthread  $(src_obj)

clean:
	rm $(src_obj) buzz
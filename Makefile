CC := gcc

CFLAGS := -lpthread

server_f : server_f.o logHelper.o serverHelpers.o requestHelpers.o
	$(CC) -o $@ $^ $(CFLAGS)

server_t : server_t.o logHelper.o serverHelpers.o requestHelpers.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean :
	rm -f *.o server_f server_t

server_f.o : server_f.c serverHelpers.h logHelper.h requestHelpers.h
server_t.o : server_t.c serverHelpers.h logHelper.h requestHelpers.h
serverHelpers.o : serverHelpers.c serverHelpers.h
requestHelpers.o : requestHelpers.c requestHelpers.h logHelper.h
logHelper.o : logHelper.c logHelper.h
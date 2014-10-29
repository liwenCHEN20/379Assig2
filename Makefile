CC := gcc

CFLAGS := -DDEBUG

server_f : server_f.o logHelper.o serverHelpers.o requestHelpers.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean :
	rm -f *.o

server_f.o : server_f.c serverHelpers.h logHelper.h requestHelpers.h
serverHelpers.o : serverHelpers.c serverHelpers.h
requestHelpers.o : requestHelpers.c requestHelpers.h logHelper.h
logHelper.o : logHelper.c logHelper.h
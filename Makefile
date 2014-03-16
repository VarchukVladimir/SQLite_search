#CC = x86_64-nacl-gcc
CC = gcc
CCFLAGS = -g
LIBS = -lsqlite3 -lz

APP_NAME = SQL_search

OBJS = SQL_search.o opts.o loadsets.o strfunc.o dbfunc.o filefunc.o env.o
SRC = SQL_search.c opts.c loadsets.c strfunc.c dbfunc.c filefunc.c emv.c

SQL_search: $(OBJS)	
	$(CC) -o $(APP_NAME) $(OBJS) $(CCFLAGS) $(LIBS)
	
SQL_search.o: 
	$(CC) -c -o SQL_search.o SQL_search.c $(CCFLAGS) 
	
opts.o:
	$(CC) -c -o opts.o opts.c $(CCFLAGS)

loadsets.o:
	$(CC) -c -o loadsets.o loadsets.c $(CCFLAGS)

strfunc.o:
	$(CC) -c -o strfunc.o strfunc.c $(CCFLAGS)

dbfunc.o:
	$(CC) -c -o dbfunc.o dbfunc.c $(CCFLAGS)

filefunc.o:
	$(CC) -c -o filefunc.o filefunc.c $(CCFLAGS)

env.o:
	$(CC) -c -o env.o env.c $(CCFLAGS)

clean:
	rm -rf *.o $(APP_NAME) 
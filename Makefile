CC = x86_64-nacl-gcc
#CC = gcc
CCFLAGS = 
LIBS = -lsqlite3 -lz

APP_NAME = SQL_search

OBJS = obj/SQL_search.o obj/opts.o obj/loadsets.o obj/strfunc.o obj/dbfunc.o obj/filefunc.o obj/env.o
SRC = src/SQL_search.c src/opts.c src/loadsets.c src/strfunc.c src/dbfunc.c src/filefunc.c src/emv.c

all: create_dirs $(APP_NAME)

create_dirs:
	@mkdir obj -p 

$(APP_NAME): $(OBJS)	
	$(CC) -o $(APP_NAME) $(OBJS) $(CCFLAGS) $(LIBS)
	
obj/SQL_search.o: src/SQL_search.c  
	$(CC) -c -o $@ $^ $(CCFLAGS) 
	
obj/opts.o: src/opts.c
	$(CC) -c -o $@ $^ $(CCFLAGS)

obj/loadsets.o:	src/loadsets.c
	$(CC) -c -o $@ $^ $(CCFLAGS)

obj/strfunc.o: src/strfunc.c
	$(CC) -c -o $@ $^ $(CCFLAGS)

obj/dbfunc.o: src/dbfunc.c
	$(CC) -c -o $@ $^ $(CCFLAGS)

obj/filefunc.o: src/filefunc.c
	$(CC) -c -o $@ $^ $(CCFLAGS)

obj/env.o: src/env.c
	$(CC) -c -o $@ $^ $(CCFLAGS)

clean:
	rm -rf obj/* $(APP_NAME) 

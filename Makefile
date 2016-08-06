CC := gcc
CFLAGS := -Werror -g
BIN := sserver_run
CFILE := selectServer.c
CHEAD := servient.h

all: $(BIN) send

$(BIN) : $(CFILE) $(CHEAD)
	gcc $(CFLAGS) -o $(BIN) $(CFILE) 

send : client.c $(CHEAD)
	gcc $(CFLAGS) -o send client.c

WS_DEBUG ?= 0
VERSION   = 0.01
CC        = /usr/bin/gcc
CFLAGS    = -Wall -D_REENTRANT -DVERSION=\"$(VERSION)\" -DWS_DEBUG=$(WS_DEBUG)
LDFLAGS   = -lftdi -lpthread

OBJ       = ws.o ws_device.o ws_config.o ws_receive.o ws_send.o ws_misc.o ws_version.o ws_record.o ws_convert.o
HDR       = ws.h
BIN       = ws

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.c $(HDR) Makefile
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -rf $(BIN) $(OBJ)


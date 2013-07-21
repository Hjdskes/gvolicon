CC=gcc
PROG=gvolicon
CFLAGS= -Wall -Os -std=c99 -lasound `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`

PREFIX=/usr/local
BINDIR=/bin

all : $(PROG)

$(PROG) :
	$(CC) $(PROG).c -o $(PROG) $(CFLAGS) $(LDFLAGS)

install: 
	install -d $(DESTDIR)$(PREFIX)$(BINDIR)
	install $(PROG) $(DESTDIR)$(PREFIX)$(BINDIR)

clean:
	rm $(PROG)

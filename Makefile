CC         = gcc
PROG       = gvolicon
PREFIX    ?= /usr/local
BINPREFIX  = ${PREFIX}/bin

LIBS       = -lasound `pkg-config --cflags --libs gtk+-3.0`
CFLAGS     = -Wall -Os -std=c99

$(PROG):
	$(CC) ${CFLAGS} ${LIBS} ${PROG}.c -o ${PROG}

debug: CFLAGS += -O0 -g -pedantic -Wextra
debug: ${PROG}

install: 
	install -d ${DESTDIR}${BINPREFIX}
	install -Dm755 $(PROG) ${DESTDIR}${BINPREFIX}

uninstall: rm -f ${BINPREFIX}/${PROG}

clean:
	rm -f ${PROG}

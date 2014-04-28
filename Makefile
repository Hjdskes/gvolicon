PROG       = gvolicon
CC         = gcc
PREFIX    ?= /usr/local
BINPREFIX  = ${PREFIX}/bin

LIBS       = -lasound `pkg-config --cflags --libs gtk+-3.0`
CFLAGS    += -Wall -Os -std=c99 -DVERSION="\"1.0\""

debug: CFLAGS += -O0 -g -pedantic -Wextra
debug: ${PROG}

${PROG}: ${PROG}.c
	@${CC} ${CFLAGS} ${LIBS} ${PROG}.c -o ${PROG}
	@strip ${PROG}

install: 
	install -d ${DESTDIR}${BINPREFIX}
	install -Dm755 ${PROG} ${DESTDIR}${BINPREFIX}/

uninstall:
	rm -f ${BINPREFIX}/${PROG}

clean:
	rm -f ${PROG}

.PHONY: debug clean install uninstall

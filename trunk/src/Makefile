CC=colorgcc
REFERENCES=gtk+-2.0
CFLAGS=-Wall `pkg-config --cflags $(REFERENCES)` -g -DSTM_POSIX
#LIBS=-Wall -dynamic `curl-config --libs` -dynamic `pkg-config --libs $(REFERENCES)` -g
#STATIC_LIBS=`curl-config --static-libs`
LIBS=-g -Wall `pkg-config --libs $(REFERENCES)`  $(STATIC_LIBS)

PACKAGE=scrabble
VERSION=1.0
PKG=$(PACKAGE)-$(VERSION)

SOURCES=\
	alphabet.c \
	compat.c \
	letter.c \
	marshal.c \
	sc-bag.c \
	sc-board.c \
	sc-dag.c \
	sc-dag2.c \
	sc-dsf.c \
	sc-dumb-player.c \
	sc-game.c \
	sc-human-player.c \
	sc-move.c \
	sc-player.c \
	sc-rack.c \
	scx-alphabet-panel.c \
	scx-board-view.c \
	scx-console.c \
	scx-main-window.c \
	scx-move-entry.c \
	scx-painter.c \
	scx-rack-view.c \
	util.c

HEADERS=\
	alphabet.h \
	letter.h \
	compat.h \
	marshal.h \
	sc-bag.h \
	sc-board.h \
	sc-dag.h \
	sc-dag2.h \
	sc-dsf.h \
	sc-dumb-player.h \
	sc-game.h \
	sc-human-player.h \
	sc-move.h \
	sc-player.h \
	sc-rack.h \
	scx-alphabet-panel.h \
	scx-board-view.h \
	scx-console.h \
	scx-main-window.h \
	scx-move-entry.h \
	scx-painter.h \
	scx-rack-view.h \
	util.h

EXTRA_DIST=\
	Makefile

OBJS=$(SOURCES:.c=.o)

all: scr gen-bad-words

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJS): $(HEADERS)

scr: $(OBJS) main.o
	$(CC) -o $@ $(OBJS) main.o $(LIBS)

gen-bad-words: $(OBJS) gen-bad-words.o
	$(CC) -o $@ $(OBJS) gen-bad-words.o $(LIBS)

clean:
	rm -rf $(OBJS) stm

dist: dist-tar

dist-tar: $(PKG)
	tar cjf $(PKG).tar.bz2 $(PKG)
	rm -rf $(PKG)

dist-zip: $(PKG)
	rm -f $(PKG).zip
	zip -9 -r $(PKG).zip $(PKG)
	rm -rf $(PKG)

$(PKG): $(SOURCES) $(HEADERS) $(EXTRA_DIST)
	mkdir -p $(PKG)
	cp $(SOURCES) $(HEADERS) $(EXTRA_DIST) $(PKG)


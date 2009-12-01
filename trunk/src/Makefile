CC=colorgcc
REFERENCES=gtk+-2.0
CFLAGS=-Wall `pkg-config --cflags $(REFERENCES)` -g -DSTM_POSIX
#LIBS=-Wall -dynamic `curl-config --libs` -dynamic `pkg-config --libs $(REFERENCES)` -g
#STATIC_LIBS=`curl-config --static-libs`
LIBS=-g -Wall `pkg-config --libs $(REFERENCES)`  $(STATIC_LIBS) -pthread

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
	sc-computer-player.c \
	sc-dag.c \
	sc-dag2.c \
	sc-dawg.c \
	sc-dsf.c \
	sc-dumb-player.c \
	sc-game.c \
	sc-human-player.c \
	sc-move.c \
	sc-player.c \
	sc-player-factory.c \
	sc-rack.c \
	sc-rack-model.c \
	scx-alphabet-panel.c \
	scx-board-view.c \
	scx-console.c \
	scx-game-panel.c \
	scx-main-window.c \
	scx-move-entry.c \
	scx-new-game-dialog.c \
	scx-painter.c \
	scx-player-selector.c \
	scx-player-view.c \
	scx-rack-view.c \
	util.c

HEADERS=\
	alphabet.h \
	letter.h \
	compat.h \
	marshal.h \
	sc-bag.h \
	sc-board.h \
	sc-computer-player.h \
	sc-dag.h \
	sc-dag2.h \
	sc-dawg.h \
	sc-dsf.h \
	sc-dumb-player.h \
	sc-game.h \
	sc-human-player.h \
	sc-move.h \
	sc-player.h \
	sc-player-factory.h \
	sc-rack-model.h \
	sc-rack.h \
	scx-alphabet-panel.h \
	scx-board-view.h \
	scx-console.h \
	scx-game-panel.h \
	scx-main-window.h \
	scx-move-entry.h \
	scx-new-game-dialog.h \
	scx-painter.h \
	scx-player-selector.h \
	scx-player-view.h \
	scx-rack-view.h \
	util.h

EXTRA_DIST=\
	Makefile

OBJS=$(SOURCES:.c=.o)

all: crossed-words gen-bad-words dictc simulation

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJS): $(HEADERS)

crossed-words: $(OBJS) main.o
	$(CC) -o $@ $(OBJS) main.o $(LIBS)

gen-bad-words: $(OBJS) gen-bad-words.o
	$(CC) -o $@ $(OBJS) gen-bad-words.o $(LIBS)

dictc: $(OBJS) dictc.o
	$(CC) -o $@ $(OBJS) dictc.o $(LIBS)

simulation: $(OBJS) simulation.o
	$(CC) -o $@ $(OBJS) simulation.o $(LIBS)

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


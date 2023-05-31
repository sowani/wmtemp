CC=gcc
LD=gcc
SED=sed
STRIP=strip

DEST?=/usr/local/bin
MAN?=/usr/local/share/man/man1

#CFLAGS=-Wall -Werror -O2 -I/usr/include/sensors -I/usr/local/include/sensors
CFLAGS=-Wall -O2 -I/usr/include/sensors -I/usr/local/include/sensors
LDFLAGS=
LIBS=-L/usr/X11R6/lib -L/usr/local/lib -lX11 -lXext -lXpm -lsensors

XPM=xpm/celcius_on.o xpm/celcius_off.o xpm/fahrenheit_on.o \
	xpm/fahrenheit_off.o xpm/kelvin_on.o xpm/kelvin_off.o xpm/parts.o
OBJS=main.o dockapp.o temp.o $(XPM)
	
TARGET=wmtemp
MANPAGE=wmtemp.1x

all: $(TARGET)

%.c: %.xpm
	echo "#include \"xpm.h\"" >$@
	$(SED) -e "s/^static //" <$< >>$@

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	$(STRIP) $@

clean:
	rm -f *.o xpm/*.o xpm/*.c core $(TARGET)

install:
	cp $(TARGET) $(DEST)/$(TARGET)
	chmod 755 $(DEST)/$(TARGET)
	cp $(MANPAGE) $(MAN)/$(MANPAGE)
	gzip -9 $(MAN)/$(MANPAGE)
	chmod 644 $(MAN)/$(MANPAGE)
	
main.o: dockapp.h temp.h xpm/xpm.h
dockapp.o: dockapp.h
temp.o: temp.h
xpm/xpm.h: $(XPM)

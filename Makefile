TARGET	=	xxx
SRC		:=	src/main.c 

#PREFIX ?=	/usr/local

CFLAGS +=	-Wall -Wextra -pedantic 
#IFLAGS  =	$(shell pkg-config --cflags glib-2.0 gio-2.0)
#LFLAGS	=	$(shell pkg-config --libs glib-2.0 gio-2.0)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(IFLAGS) $(SRC) $(LFLAGS) $(LDFLAGS) -o $(TARGET)

#install: $(TARGET)
#	mkdir -p $(DESTDIR)$(PREFIX)/bin
#	install $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	$(RM) ./xxx

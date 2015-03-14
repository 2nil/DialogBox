
#ifneq ($(MSYSTEM),MINGW32)
# Variante des compilo sous cygwin
# cygwin gcc
# BUILD=i686-pc-cygwin

# mingw gcc 32 bits on 32 bits plateform
#BUILD=i686-pc-mingw32

# mingw gcc 32 bits on 64 bits plateform (??)
#BUILD=i686-w64-mingw32

# mingw gcc 64 bits on 64 bits plateform
BUILD=x86_64-w64-mingw32
#endif

ifdef BUILD
	CC=$(BUILD)-gcc
	LD=$(BUILD)-ld
	RC=$(BUILD)-windres
	CPPFLAGS += -I /usr/$(BUILD)/sys-root/mingw/include
else
	CC=gcc
	LD=ld
	RC=windres
endif


#CPPFLAGS += -D_UNICODE -DUNICODE
# Nécessaire pour l'unicode avec mingw non 64 bits
#CFLAGS += -finput-charset=ISO-8859-1 -D ISO_8859_1_CHARSET
CPPFLAGS += -I. #-D_WIN32_WINNT=0x600 -D_WIN32_IE=0x600
CFLAGS +=-g -Wall
LDFLAGS=-mwindows
LDLIBS += -static -lgdi32 -lole32 -lcomdlg32 -lcomctl32
SRCS=DialogBox.c UIManage.c MsgBox.c igetopt.c
OBJS=$(SRCS:.c=.o) DlgBox_res.o
TARGET=DialogBox.exe

all : $(TARGET)
	
$(TARGET) : $(OBJS)

clean :
	rm -f $(OBJS) $(TARGET) *~

rclean :
	@rm -f $(OBJS) $(TARGET) *~ *.d


ifneq ($(MAKECMDGOALS),rclean)
%.o: %.c
#  commands to execute (built-in):
	$(COMPILE.c) $(OUTPUT_OPTION) $<

# Régles pour construire les fichier objet d'après les .rc
%.o : %.rc
ifeq ($(findstring 64,$(BUILD)),64)
	cp DialogBox64.manifest DialogBox.manifest
else
	cp DialogBox32.manifest DialogBox.manifest
endif
	$(RC) $(CPPFLAGS) $< --include-dir . $(OUTPUT_OPTION)

%.d: %.c
	@echo "Building "$@" from "$<
	@$(CC) -isystem /usr/include -MM $(CPPFLAGS) $< > $@

%.exe: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

# Inclusion of the dependency files '.d'
ifdef SRCS
-include $(SRCS:.c=.d)
endif
endif


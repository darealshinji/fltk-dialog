ifneq ($(wildcard config.mak),)
include config.mak
endif

# config
LOCAL_JPEG  ?= 1
LOCAL_PNG   ?= 1
LOCAL_ZLIB  ?= 1
STATIC_FLTK ?= 1

fltk = fltk-1.3
OPT ?= -Os

common_CXXFLAGS := $(OPT) -Wall -Wextra \
 -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 \
 -ffunction-sections -fdata-sections

LDFLAGS += \
 -s -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--gc-sections

CXXFLAGS += $(common_CXXFLAGS) -Isrc -I$(fltk) \
 $(shell $(fltk)/fltk-config --cxxflags | tr ' ' '\n' | grep '^-D.*')

ifeq ($(STATIC_FLTK),1)
CXXFLAGS += \
 -DFLTK_VERSION=\"$(shell cat $(fltk)/VERSION)\" \
 -DREVISION=\"$(shell cat $(fltk)/revision)\"
endif

fltk_CXXFLAGS := $(common_CXXFLAGS) \
 -Wno-unused-parameter -Wno-missing-field-initializers

LIBS = $(shell $(fltk)/fltk-config --ldflags --use-images) -lm

fltk_config = \
 --enable-static \
 --disable-shared \
 --disable-debug \
 --disable-gl \
 --enable-threads
ifeq ($(LOCAL_JPEG),1)
fltk_config += --enable-localjpeg
endif
ifeq ($(LOCAL_PNG),1)
fltk_config += --enable-localpng
endif
ifeq ($(LOCAL_ZLIB),1)
fltk_config += --enable-localzlib
endif


BIN = fltk-dialog
OBJS = $(addprefix src/,\
 about.o choice.o calendar.o color.o file.o html.o input.o message.o \
 password.o progress.o slider.o translate.o version.o main.o \
 Flek/FDate.o Flek/Fl_Calendar.o)

define CLEAN
	-rm -f $(BIN) $(OBJS)
	[ ! -f $(fltk)/Makefile ] || $(MAKE) -C $(fltk) $@
endef


all: $(BIN)

clean:
	$(CLEAN)

distclean:
	$(CLEAN)
	-rm -rf $(fltk)/autom4te.cache

mostlyclean:
	-rm -f $(BIN) $(OBJS)

clobber: mostlyclean
	-rm -rf $(fltk)

$(OBJS): $(fltk)/lib/libfltk.a

$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

$(fltk):
	svn co --username="" --password="" "http://seriss.com/public/fltk/fltk/branches/branch-1.3" $@; \
  LANG=C svn info $@ | grep '^Revision:' | cut -d' ' -f2 > $@/revision

$(fltk)/fltk-config: $(fltk)
	test -x $@ || (cd $< && NOCONFIGURE=1 ./autogen.sh && \
  CXXFLAGS="$(fltk_CXXFLAGS)" LDFLAGS="$(LDFLAGS)" \
  ./configure $(fltk_config))

$(fltk)/lib/libfltk.a: $(fltk)/fltk-config
	$(MAKE) -C $(fltk)

src/about.o src/html.o src/main.o: CXXFLAGS+=-Wno-unused-parameter


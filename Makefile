ifneq ($(wildcard config.mak),)
include config.mak
endif

fltk = fltk-1.3
OPT = -Os

CXXFLAGS += $(OPT) -Wall -Wextra -Isrc -I$(fltk)
CXXFLAGS += $(shell $(fltk)/fltk-config --cxxflags | tr ' ' '\n' | grep '^-D.*')
fltk_CXXFLAGS := $(OPT) -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers

def_LDFLAGS := -Wl,-z,defs -Wl,--as-needed
LIBS = $(shell $(fltk)/fltk-config --ldflags) -lm

fltk_config = \
 --enable-static \
 --disable-shared \
 --disable-debug \
 --disable-gl \
 --enable-threads \
 $(NULL)
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
 about.o choice.o calendar.o color.o file.o input.o message.o password.o \
 progress.o slider.o translate.o version.o main.o \
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
	$(CXX) -o $@ $^ $(LDFLAGS) $(def_LDFLAGS) $(LIBS)

$(fltk):
	if svn --version 2>/dev/null >/dev/null; then \
  svn co --username="" --password="" "http://seriss.com/public/fltk/fltk/branches/branch-1.3" $@; \
else \
  git clone --depth 1 "https://github.com/darealshinji/fltk-1.3" $@; \
fi

$(fltk)/patch_applied_stamp: $(fltk)
	test -f $@ || (cd $< && patch -p1 <../fl_ask-window_cb.diff && \
  touch ../$@)

$(fltk)/fltk-config: $(fltk)/patch_applied_stamp
	test -x $@ || (cd $(fltk) && NOCONFIGURE=1 ./autogen.sh && \
  CXXFLAGS="$(fltk_CXXFLAGS)" LDFLAGS="$(LDFLAGS)" \
  ./configure $(fltk_config))

$(fltk)/lib/libfltk.a: $(fltk)/fltk-config
	$(MAKE) -C $(fltk)

src/about.o src/main.o: CXXFLAGS+=-Wno-unused-parameter


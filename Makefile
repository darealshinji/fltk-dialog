ifneq ($(wildcard config.mak),)
include config.mak
endif

# set to "yes" if you want to link
# against system libraries
SYSTEM_JPEG ?= no
SYSTEM_PNG  ?= no
SYSTEM_ZLIB ?= no

# set to "no" if you don't want an embedded FLKT
# icon to appear in taskbar and windows
WITH_DEFAULT_ICON ?= yes

# set to "no" to disable certain features
WITH_L10N        ?= yes
WITH_CALENDAR    ?= yes
WITH_CHECKLIST   ?= yes
WITH_COLOR       ?= yes
WITH_DATE        ?= yes
WITH_DND         ?= yes
WITH_DROPDOWN    ?= yes
WITH_FILE        ?= yes
WITH_FONT        ?= yes
WITH_HTML        ?= yes
WITH_NOTIFY      ?= yes
WITH_PROGRESS    ?= yes
WITH_RADIOLIST   ?= yes
WITH_TEXTINFO    ?= yes
WITH_WINDOW_ICON ?= yes

DYNAMIC_NOTIFY ?= yes

# source directories for FLTK and libpng
fltk = fltk-1.3
libpng = libpng16

libpng_version = 1.6.25
libpng_tarball = libpng-$(libpng_version).tar.xz

BIN = fltk-dialog
OBJS = $(addprefix src/,about.o message.o misc/translate.o version.o main.o)

OPT ?= -Os

common_CFLAGS := $(OPT) -Wall -Wextra \
 -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 \
 -ffunction-sections -fdata-sections

LDFLAGS += \
 -s -Wl,-O1 -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--gc-sections

CXXFLAGS += $(common_CFLAGS) -I. -Isrc -I$(fltk)/build -I$(fltk) \
 $(shell $(fltk)/build/fltk-config --cxxflags | tr ' ' '\n' | grep '^-D.*')

CXXFLAGS += \
 -DFLTK_VERSION=\"$(shell cat $(fltk)/VERSION)\" \
 -DREVISION=\"$(shell cat $(fltk)/revision)\"

HAVE_ITOSTR = no
HAVE_PRINT_DATE = no
HAVE_READSTDIO = no
HAVE_SPLIT = no

ifneq ($(WITH_L10N),no)
CXXFLAGS += -DWITH_L10N
OBJS += src/l10n.o
endif
ifneq ($(WITH_DEFAULT_ICON),no)
CXXFLAGS += -DWITH_DEFAULT_ICON
endif
ifneq ($(WITH_CALENDAR),no)
CXXFLAGS += -DWITH_CALENDAR
OBJS += src/calendar.o src/Flek/Fl_Calendar.o
HAVE_PRINT_DATE = yes
endif
ifneq ($(WITH_CHECKLIST),no)
CXXFLAGS += -DWITH_CHECKLIST
OBJS += src/checklist.o
HAVE_SPLIT = yes
endif
ifneq ($(WITH_COLOR),no)
CXXFLAGS += -DWITH_COLOR
OBJS += src/color.o
endif
ifneq ($(WITH_DATE),no)
CXXFLAGS += -DWITH_DATE
OBJS += src/date.o
HAVE_PRINT_DATE = yes
endif
ifneq ($(WITH_DND),no)
CXXFLAGS += -DWITH_DND
OBJS += src/dnd.o
endif
ifneq ($(WITH_DROPDOWN),no)
CXXFLAGS += -DWITH_DROPDOWN
OBJS += src/dropdown.o
HAVE_SPLIT = yes
endif
ifneq ($(WITH_FILE),no)
CXXFLAGS += -DWITH_FILE
OBJS += src/file.o src/file_dlopen_qtplugin.o
endif
ifneq ($(WITH_FONT),no)
CXXFLAGS += -DWITH_FONT
OBJS += src/font.o
endif
ifneq ($(WITH_HTML),no)
CXXFLAGS += -DWITH_HTML
OBJS += src/html.o
endif
ifneq ($(WITH_NOTIFY),no)
CXXFLAGS += -DWITH_NOTIFY
CXXFLAGS += $(shell pkg-config --cflags libnotify)
OBJS += src/notify.o
endif
ifneq ($(WITH_PROGRESS),no)
CXXFLAGS += -DWITH_PROGRESS
OBJS += src/progress.o
HAVE_READSTDIO = yes
endif
ifneq ($(WITH_RADIOLIST),no)
CXXFLAGS += -DWITH_RADIOLIST
OBJS += src/radiolist.o
HAVE_SPLIT = yes
endif
ifneq ($(WITH_TEXTINFO),no)
CXXFLAGS += -DWITH_TEXTINFO
OBJS += src/textinfo.o
HAVE_READSTDIO = yes
endif
ifneq ($(WITH_WINDOW_ICON),no)
CXXFLAGS += -DWITH_WINDOW_ICON
OBJS += src/window_icon.o
endif

ifneq ($(HAVE_PRINT_DATE),no)
OBJS += src/misc/print_date.o src/Flek/FDate.o
endif
ifneq ($(HAVE_READSTDIO),no)
OBJS += src/misc/readstdio.o
endif
ifneq ($(HAVE_SPLIT),no)
OBJS += src/misc/split.o
endif

ifneq ($(WITH_NOTIFY),no)
ifneq ($(DYNAMIC_NOTIFY),no)
CXXFLAGS += -DDYNAMIC_NOTIFY
LIBS += -ldl
else
LIBS += $(shell pkg-config --libs libnotify)
endif
endif


fltk_CFLAGS := $(common_CFLAGS) \
  -Wno-unused-parameter -Wno-missing-field-initializers

cmake_config = \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_CXX_FLAGS="$(fltk_CFLAGS)" \
  -DCMAKE_C_FLAGS="$(fltk_CFLAGS)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_OPTIM="$(OPT)"

LIBS += $(fltk)/build/lib/libfltk_images.a

ifneq ($(SYSTEM_JPEG),no)
LIBS += -ljpeg
else
cmake_config += -DOPTION_USE_SYSTEM_LIBJPEG="OFF"
LIBS += $(fltk)/build/lib/libfltk_jpeg.a
endif

ifneq ($(SYSTEM_PNG),no)
LIBS += -lpng
else
libpng_a = $(libpng)/build/libpng.a
cmake_config += -DOPTION_USE_SYSTEM_LIBPNG="ON" \
 -DHAVE_LIBPNG_PNG_H="$(CURDIR)/$(libpng)/build/png.h" \
 -DLIB_png="$(CURDIR)/$(libpng_a)"
LIBS += $(libpng_a)
endif

ifneq ($(SYSTEM_ZLIB),no)
LIBS += -lz
else
cmake_config += -DOPTION_USE_SYSTEM_ZLIB="OFF"
LIBS += $(fltk)/build/lib/libfltk_z.a
endif

libfltk = $(fltk)/build/lib/libfltk.a
LIBS += $(libfltk) $(shell $(fltk)/build/fltk-config --use-images --ldflags)

ifeq ($(V),1)
cmake_vebose = -DCMAKE_VERBOSE_MAKEFILE="ON"
else
silent = @
endif



all: $(BIN)

configure: checkout-sources
	autoconf

download: checkout-sources
checkout-sources: $(libpng) $(fltk)

clean: mostlyclean
	[ ! -f $(fltk)/build/Makefile ] || $(MAKE) -C $(fltk)/build clean
	[ ! -f $(libpng)/build/Makefile ] || $(MAKE) -C $(libpng)/build clean

distclean: mostlyclean
	-rm -rf $(fltk)/build $(libpng)/build autom4te.cache
	-rm -f config.mak config.log config.status

mostlyclean:
	-rm -f $(BIN) *.so qtgui_so.h src/*.o src/Flek/*.o src/misc/*.o

clobber: mostlyclean
	-rm -rf $(fltk) $(libpng) autom4te.cache
	-rm -f configure config.mak config.log config.status
	-rm -f $(libpng_tarball)

$(OBJS): $(libpng_a) $(libfltk)

$(BIN): $(OBJS)
	@echo "Linking CXX executable $@"
	$(silent)$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

.cpp.o:
	@echo "Building CXX object $@"
	$(silent)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(libpng): $(libpng)/checkout_stamp
$(libpng)/checkout_stamp:
	rm -rf `dirname $@` $(libpng_tarball) && \
  wget "https://sourceforge.net/projects/libpng/files/$(libpng)/$(libpng_version)/$(libpng_tarball)" && \
  tar xf $(libpng_tarball) && \
  mv libpng-$(libpng_version) `dirname $@` && \
  rm -f $(libpng_tarball) && \
  touch $@

$(fltk): $(fltk)/revision
$(fltk)/revision:
	svn co --username="" --password="" "http://seriss.com/public/fltk/fltk/branches/branch-1.3" $(fltk); \
  LANG=C svn info $(fltk) | grep '^Revision:' | cut -d' ' -f2 > $@

$(fltk)/build/Makefile: $(fltk)
	mkdir -p $(fltk)/build
	cd $(fltk)/build && cmake .. $(cmake_config) $(cmake_vebose)

$(libpng)/build/Makefile: $(libpng)
	mkdir -p $(libpng)/build
	cd $(libpng)/build && cmake .. $(cmake_vebose) \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_CXX_FLAGS="$(common_CFLAGS)" \
  -DCMAKE_C_FLAGS="$(common_CFLAGS)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)" \
  -DPNG_DEBUG="OFF" \
  -DPNG_SHARED="OFF" \
  -DPNG_STATIC="ON"

$(fltk)/build/fltk-config: $(libfltk)

$(libpng_a): $(libpng)/build/Makefile
	$(MAKE) -C $(libpng)/build

$(libfltk): $(libpng_a) $(fltk)/build/Makefile
	$(MAKE) -C $(fltk)/build

src/file_dlopen_qtplugin.o: qtgui_so.h
qtgui_so.h: qt4gui.so qt5gui.so
	xxd -i qt4gui.so > $@ && xxd -i qt5gui.so >> $@

qt4gui.so: src/file_qtplugin_qt4.o
	$(CXX) -shared -o $@ $^ $(LDFLAGS) $(shell pkg-config --libs QtGui QtCore)

qt5gui.so: src/file_qtplugin_qt5.o
	$(CXX) -shared -o $@ $^ $(LDFLAGS) $(shell pkg-config --libs Qt5Widgets Qt5Core)

src/file_qtplugin_qt4.o: src/file_qtplugin.cpp
	$(CXX) -std=c++0x -fPIC -DPIC $(CXXFLAGS) $(shell pkg-config --cflags QtGui QtCore) -c -o $@ $<

src/file_qtplugin_qt5.o: src/file_qtplugin.cpp
	$(CXX) -std=c++0x -fPIC -DPIC $(CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

src/about.o src/font.o src/html.o src/main.o src/message.o src/window_icon.o: CXXFLAGS+=-Wno-unused-parameter


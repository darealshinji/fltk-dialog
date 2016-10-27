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

FLTK_VERSION = 1.3.4rc2


# source directories
fltk = 3rdparty/fltk
png  = 3rdparty/png
zlib = 3rdparty/zlib


BIN  = fltk-dialog
OBJS = $(addprefix src/,about.o message.o misc/translate.o version.o main.o)


# default build flags
common_CFLAGS := -Os -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 -ffunction-sections -fdata-sections
CFLAGS        ?= $(common_CFLAGS)
CXXFLAGS      ?= $(common_CFLAGS)
LDFLAGS       ?= -s -Wl,-O1 -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--gc-sections

# fltk-dialog build flags
main_CXXFLAGS := -Wall -Wextra $(CXXFLAGS) $(CPPFLAGS) -I. -Isrc -I$(fltk)/build -I$(fltk) -DFLTK_VERSION=\"$(FLTK_VERSION)\"
main_CXXFLAGS += $(shell $(fltk)/build/fltk-config --cxxflags | tr ' ' '\n' | grep '^-D.*')

# libfltk build flags
fltk_CFLAGS   := -Wall $(CFLAGS) $(CPPFLAGS) -Wno-unused-parameter -Wno-missing-field-initializers
fltk_CXXFLAGS := -Wall $(CXXFLAGS) $(CPPFLAGS) -Wno-unused-parameter -Wno-missing-field-initializers


HAVE_ITOSTR     = no
HAVE_PRINT_DATE = no
HAVE_READSTDIO  = no
HAVE_SPLIT      = no
ifneq ($(WITH_L10N),no)
main_CXXFLAGS += -DWITH_L10N
OBJS          += src/l10n.o
endif
ifneq ($(WITH_DEFAULT_ICON),no)
main_CXXFLAGS += -DWITH_DEFAULT_ICON
endif
ifneq ($(WITH_CALENDAR),no)
main_CXXFLAGS += -DWITH_CALENDAR
OBJS          += src/calendar.o src/Flek/Fl_Calendar.o
HAVE_PRINT_DATE = yes
endif
ifneq ($(WITH_CHECKLIST),no)
main_CXXFLAGS += -DWITH_CHECKLIST
OBJS          += src/checklist.o
HAVE_SPLIT = yes
endif
ifneq ($(WITH_COLOR),no)
main_CXXFLAGS += -DWITH_COLOR
OBJS          += src/color.o
endif
ifneq ($(WITH_DATE),no)
main_CXXFLAGS += -DWITH_DATE
OBJS          += src/date.o
HAVE_PRINT_DATE = yes
endif
ifneq ($(WITH_DND),no)
main_CXXFLAGS += -DWITH_DND
OBJS          += src/dnd.o
endif
ifneq ($(WITH_DROPDOWN),no)
main_CXXFLAGS += -DWITH_DROPDOWN
OBJS          += src/dropdown.o
HAVE_SPLIT = yes
endif
ifneq ($(WITH_FILE),no)
main_CXXFLAGS += -DWITH_FILE
OBJS          += src/file.o src/file_dlopen_qtplugin.o
endif
ifneq ($(WITH_FONT),no)
main_CXXFLAGS += -DWITH_FONT
OBJS          += src/font.o
endif
ifneq ($(WITH_HTML),no)
main_CXXFLAGS += -DWITH_HTML
OBJS          += src/html.o
endif
ifneq ($(WITH_NOTIFY),no)
main_CXXFLAGS += -DWITH_NOTIFY $(shell pkg-config --cflags libnotify)
OBJS          += src/notify.o
endif
ifneq ($(WITH_PROGRESS),no)
main_CXXFLAGS += -DWITH_PROGRESS
OBJS          += src/progress.o
HAVE_READSTDIO = yes
endif
ifneq ($(WITH_RADIOLIST),no)
main_CXXFLAGS += -DWITH_RADIOLIST
OBJS          += src/radiolist.o
HAVE_SPLIT = yes
endif
ifneq ($(WITH_TEXTINFO),no)
main_CXXFLAGS += -DWITH_TEXTINFO
OBJS          += src/textinfo.o
HAVE_READSTDIO = yes
endif
ifneq ($(WITH_WINDOW_ICON),no)
main_CXXFLAGS += -DWITH_WINDOW_ICON
OBJS          += src/window_icon.o
endif
ifneq ($(HAVE_PRINT_DATE),no)
OBJS          += src/misc/print_date.o src/Flek/FDate.o
endif
ifneq ($(HAVE_READSTDIO),no)
OBJS          += src/misc/readstdio.o
endif
ifneq ($(HAVE_SPLIT),no)
OBJS          += src/misc/split.o
endif
ifneq ($(WITH_NOTIFY),no)
ifneq ($(DYNAMIC_NOTIFY),no)
main_CXXFLAGS += -DDYNAMIC_NOTIFY
main_LIBS     += -ldl
else
main_LIBS     += $(shell pkg-config --libs libnotify)
endif
endif


# Qt plugin CXXFLAGS
plugin_CXXFLAGS := -std=c++0x -fPIC -DPIC $(main_CXXFLAGS)


extra_include :=
extra_libdirs :=
fltk_cmake_config = \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_CXX_FLAGS="$(fltk_CXXFLAGS) $(extra_include)" \
  -DCMAKE_C_FLAGS="$(fltk_CFLAGS) $(extra_include)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS) $(extra_libdirs)" \
  -DOPTION_USE_GL="OFF" \
  -DOPTION_OPTIM=""


main_LIBS += $(fltk)/build/lib/libfltk_images.a

ifneq ($(SYSTEM_JPEG),no)
main_LIBS += -ljpeg
else
fltk_cmake_config += -DOPTION_USE_SYSTEM_LIBJPEG="OFF"
main_LIBS += $(fltk)/build/lib/libfltk_jpeg.a
endif

ifneq ($(SYSTEM_PNG),no)
main_LIBS         += -lpng
else
libpng_a           = $(png)/build/libpng.a
fltk_cmake_config += \
  -DOPTION_USE_SYSTEM_LIBPNG="ON" \
  -DHAVE_LIBPNG_PNG_H="$(CURDIR)/$(png)/build/png.h" \
  -DLIB_png="$(CURDIR)/$(libpng_a)"
main_LIBS         += $(libpng_a)
extra_include     += -I"$(CURDIR)/$(png)/build" -I"$(CURDIR)/$(png)"
extra_libdirs     += -L"$(CURDIR)/$(png)/build"
endif

ifneq ($(SYSTEM_ZLIB),no)
main_LIBS         += -lz
else
libz_a             = $(zlib)/build/libz.a
fltk_cmake_config += \
  -DOPTION_USE_SYSTEM_ZLIB="ON" \
  -DZLIB_INCLUDE_DIR="$(CURDIR)/$(zlib)" \
  -DZLIB_LIBRARY_RELEASE="$(CURDIR)/$(libz_a)" \
  -DLIB_zlib="$(CURDIR)/$(libz_a)"
main_LIBS         += $(libz_a)
extra_include     += -I"$(CURDIR)/$(zlib)"
extra_libdirs     += -L"$(CURDIR)/$(zlib)/build"
endif

libfltk    = $(fltk)/build/lib/libfltk.a
main_LIBS += $(libfltk) $(shell $(fltk)/build/fltk-config --use-images --ldflags)

ifeq ($(V),1)
cmake_vebose = -DCMAKE_VERBOSE_MAKEFILE="ON"
else
silent = @
endif

msg_GENH    = @echo "Generating header file $@"
msg_CXX     = @echo "Building CXX object $@"
msg_CXXLD   = @echo "Linking CXX executable $@"
msg_CXXLDSO = @echo "Linking CXX shared object $@"

CMAKE ?= cmake
XXD   ?= xxd



all: $(BIN)

clean: mostlyclean
	[ ! -f $(fltk)/build/Makefile ] || $(MAKE) -C $(fltk)/build clean
	[ ! -f $(png)/build/Makefile ] || $(MAKE) -C $(png)/build clean
	[ ! -f $(zlib)/build/Makefile ] || $(MAKE) -C $(zlib)/build clean

distclean: mostlyclean
	-rm -rf $(fltk)/build $(png)/build $(zlib)/build autom4te.cache
	-rm -f config.mak config.log config.status

mostlyclean:
	-rm -f $(BIN) *.so qtgui_so.h src/*.o src/Flek/*.o src/misc/*.o

maintainer-clean: distclean
	-rm -f configure

$(OBJS): $(libpng_a) $(libfltk)

$(BIN): $(OBJS)
	$(msg_CXXLD)
	$(silent)$(CXX) -o $@ $^ $(LDFLAGS) $(main_LIBS) $(LIBS)

.cpp.o:
	$(msg_CXX)
	$(silent)$(CXX) $(main_CXXFLAGS) -c -o $@ $<


$(fltk)/build/fltk-config: $(libfltk)

$(fltk)/build/Makefile: $(fltk)
	mkdir -p $(fltk)/build
	cd $(fltk)/build && $(CMAKE) .. $(fltk_cmake_config) $(cmake_vebose)


ifeq ($(SYSTEM_ZLIB),no)
libpng_build_Makefile_dep = $(libz_a)
endif

$(png)/build/Makefile: $(libpng_build_Makefile_dep)
	mkdir -p $(png)/build
	cd $(png)/build && \
  CFLAGS="-Wall $(CFLAGS) $(CPPFLAGS) -I$(CURDIR)/$(zlib) -I$(CURDIR)/$(zlib)/build" \
  LDFLAGS="$(LDFLAGS) -L$(CURDIR)/$(zlib)/build" \
  ../configure --disable-shared \
    --with-zlib-prefix="fltk_dialog_" \
    --with-libpng-prefix="fltk_dialog_"

$(zlib)/build/Makefile:
	mkdir -p $(zlib)/build
	cd $(zlib)/build && $(CMAKE) .. $(cmake_vebose) \
  -DCMAKE_BUILD_TYPE="None" \
  -DCMAKE_C_FLAGS="-Wall $(CFLAGS) $(CPPFLAGS)" \
  -DCMAKE_EXE_LINKER_FLAGS="$(LDFLAGS)"


$(libpng_a): $(png)/build/Makefile
	$(MAKE) -C $(png)/build && \
  cd $(png)/build && rm -f libpng.so libpng.a && \
  ln -s .libs/libpng16.a libpng.a

$(libz_a): $(zlib)/build/Makefile
	$(MAKE) -C $(zlib)/build && rm -f $(zlib)/build/libz.so*

$(libfltk): $(libpng_a) $(fltk)/build/Makefile
	$(MAKE) -C $(fltk)/build


qtgui_so.h: qt4gui.so qt5gui.so
	$(msg_GENH)
	$(silent)$(XXD) -i qt4gui.so > $@ && $(XXD) -i qt5gui.so >> $@

qt4gui.so: src/file_qtplugin_qt4.o
	$(msg_CXXLDSO)
	$(silent)$(CXX) -shared -o $@ $^ $(LDFLAGS) $(shell pkg-config --libs QtGui QtCore)

qt5gui.so: src/file_qtplugin_qt5.o
	$(msg_CXXLDSO)
	$(silent)$(CXX) -shared -o $@ $^ $(LDFLAGS) $(shell pkg-config --libs Qt5Widgets Qt5Core)

src/file_qtplugin_qt4.o: src/file_qtplugin.cpp
	$(msg_CXX)
	$(silent)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags QtGui QtCore) -c -o $@ $<

src/file_qtplugin_qt5.o: src/file_qtplugin.cpp
	$(msg_CXX)
	$(silent)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

src/file_dlopen_qtplugin.o: qtgui_so.h

src/file_qtplugin.cpp: $(libfltk)


src/about.o src/font.o src/html.o src/main.o src/message.o src/window_icon.o: main_CXXFLAGS+=-Wno-unused-parameter


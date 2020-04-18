BUILDDIR  ?= $(CURDIR)
SOURCEDIR ?= $(BUILDDIR)

ifneq ($(USE_DLOPEN),)

DEFINES += -DUSE_DLOPEN

ifneq ($(QT_CXXFLAGS),)
ifneq ($(QT_LDFLAGS),)
HAVE_QT = yes
endif
endif

ifneq ($(HAVE_QT),)
DEFINES += -DHAVE_QT
ifneq ($(USE_EXTERNAL_PLUGINS),)
DEFINES += -DUSE_EXTERNAL_PLUGINS
endif
endif

endif  # USE_DLOPEN

INCLUDES += -I$(BUILDDIR) -I$(SOURCEDIR)

CFLAGS ?= -Wall -O2 -std=c99
CXXFLAGS ?= -Wall -O2
#CXXFLAGS ?= $(shell fltk-config --use-images --cflags)
LDFLAGS ?= -lfltk -lfltk_images -lmagic
#LDFLAGS ?= $(shell fltk-config --use-images --ldlags)

BIN_CFLAGS = $(INCLUDES) $(CFLAGS) $(CPPFLAGS)
BIN_CXXFLAGS = $(DEFINES) $(INCLUDES) $(CXXFLAGS) $(CPPFLAGS)
BIN_LDFLAGS  = $(LDFLAGS)

#QT_CXXFLAGS ?= -Wall -O2 $(shell pkg-config --cflags Qt5Widgets Qt5Core)
#QT_LDFLAGS ?= $(shell pkg-config --libs Qt5Widgets Qt5Core)

CC ?= gcc
CXX ?= g++
XXDCMD ?= xxd -i

ICONS = \
  eye-closed.png \
  eye.png \
  go-back-gray.png \
  go-back.png \
  go-up-gray.png \
  go-up.png \
  icon-any.png \
  icon-desktop.png \
  icon-dir.png \
  icon-hdd.png \
  icon-home.png \
  icon-link-any.png \
  icon-link-dir.png \
  list-ordered-1.png \
  list-ordered-2.png \
  $(NULL)

_SRCS = \
  about.cpp \
  calendar.cpp \
  checklist.cpp \
  color.cpp \
  date.cpp \
  dnd.cpp \
  dropdown.cpp \
  file.cpp \
  file_fltk.cpp \
  font.cpp \
  html.cpp \
  ico_image.cpp \
  img_to_rgb.cpp \
  indicator.cpp \
  l10n.cpp \
  main.cpp \
  message.cpp \
  misc.cpp \
  notify.cpp \
  progress.cpp \
  radiolist.cpp \
  radiolist_browser.cpp \
  textinfo.cpp \
  $(NULL)

ifneq ($(USE_DLOPEN),)
_SRCS += icns_image.cpp indicator_gtk.cpp
endif

GENHDRS  = $(addprefix $(BUILDDIR)/,fltk_png.h icon_png.h icon_ind_png.h image_missing_png.h icons.h)
ifneq ($(HAVE_QT),)
#ifneq ($(USE_EXTERNAL_PLUGINS),)
GENHDRS += $(BUILDDIR)/qtplugin_so.h
#endif
endif

SRCS = $(addprefix $(SOURCEDIR)/,$(_SRCS))
OBJS = $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(_SRCS)))
BIN  = $(BUILDDIR)/fltk-dialog

ifneq ($(V),1)
silent    = @
msg_GEN   = @echo " Generate     $(notdir $@)";
msg_C     = @echo " C   compile  $(notdir $@)";
msg_CXX   = @echo " CXX compile  $(notdir $@)";
msg_LDCXX = @echo " CXX link     $(notdir $@)";
endif

define NL


endef


.PHONY: all clean

all: $(BIN)

clean:
	-rm -f $(BIN)
	-rm -f $(OBJS)
	-rm -f $(GENHDRS)
	-rm -f $(addprefix $(BUILDDIR)/,qtplugin.o qtplugin.so qtplugin_so.h icons.h_)

$(BIN): $(OBJS)
	$(msg_LDCXX)$(CXX) -o $@ $^ $(BIN_LDFLAGS)

$(OBJS): $(SRCS)
$(SRCS): $(GENHDRS)

%.cpp.o:
	$(msg_CXX)$(CXX) $(BIN_CXXFLAGS) -c $(addprefix $(SOURCEDIR)/,$(basename $(notdir $@))) -o $@

%.c.o:
	$(msg_C)$(CC) $(BIN_CFLAGS) -c $(addprefix $(SOURCEDIR)/,$(basename $(notdir $@))) -o $@

$(BUILDDIR)/fltk_png.h: $(SOURCEDIR)/fltk.png
	$(msg_GEN)cd $(SOURCEDIR) && $(XXDCMD) fltk.png > $@

$(BUILDDIR)/icon_png.h: $(SOURCEDIR)/icon.png
	$(msg_GEN)cd $(SOURCEDIR) && $(XXDCMD) icon.png > $@ && sed -i 's|^unsigned |static const unsigned |g' $@

$(BUILDDIR)/icon_ind_png.h: $(SOURCEDIR)/icon_ind.png
	$(msg_GEN)cd $(SOURCEDIR) && $(XXDCMD) icon_ind.png > $@ && sed -i 's|^unsigned |static const unsigned |g' $@

$(BUILDDIR)/image_missing_png.h: $(SOURCEDIR)/image-missing.png
	$(msg_GEN)cd $(SOURCEDIR) && $(XXDCMD) image-missing.png > $@ && sed -i 's|^unsigned |static const unsigned |g' $@

$(BUILDDIR)/icons.h: $(addprefix $(SOURCEDIR)/icons/,$(ICONS))
	$(msg_GEN)rm -f $@_;
	$(foreach FILE,$(addprefix icons/,$(ICONS)),$(silent)cd $(SOURCEDIR) && $(XXDCMD) $(FILE) >> $@_;${NL})
	$(silent)mv $@_ $@

$(BUILDDIR)/qtplugin_so.h: $(BUILDDIR)/qtplugin.so
	$(msg_GEN)cd $(BUILDDIR) && $(XXDCMD) qtplugin.so > $@

$(BUILDDIR)/qtplugin.so: $(BUILDDIR)/qtplugin.o
	$(msg_LDCXX)$(CXX) -shared -o $@ $< $(QT_LDFLAGS) -lpthread -s

$(BUILDDIR)/qtplugin.o: $(SOURCEDIR)/qtplugin.cpp
	$(msg_CXX)$(CXX) $(QT_CXXFLAGS) -fPIC -o $@ -c $<


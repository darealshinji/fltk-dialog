rsvg_convert_so.h: rsvg_convert.so
	$(msg_GENH)
	$(Q)xxd -i $< > $@

rsvg_modules = glib-2.0 gio-2.0 gdk-pixbuf-2.0 cairo pangocairo libxml-2.0 libcroco-0.6

rsvg_convert.so: src/rsvg_convert.o $(librsvg)
	$(msg_CCLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs $(rsvg_modules)) -lm

src/rsvg_convert.cpp: $(librsvg)
src/rsvg_convert.o: src/rsvg_convert.cpp
	$(msg_CC)
	$(Q)$(CXX) -I librsvg $(plugin_CXXFLAGS) -Wno-deprecated-declarations $(shell pkg-config --cflags $(rsvg_modules)) -c -o $@ $<

$(librsvg): librsvg/Makefile
	$(MAKE) -C librsvg V=$(make_verbose)

librsvg/Makefile:
	cd librsvg && ./configure --disable-shared --disable-tools --disable-introspection --disable-pixbuf-loader --with-pic

ifneq ($(EMBEDDED_PLUGINS),no)
src/dlopen_rsvg_plugin.o: rsvg_convert_so.h
else
src/dlopen_rsvg_plugin.o: rsvg_convert.so
endif


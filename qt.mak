qtplugins =

ifneq ($(HAVE_QT4),no)
qtplugins += qt4gui.so
endif
ifneq ($(HAVE_QT5),no)
qtplugins += qt5gui.so
endif

qtgui_so.h: $(qtplugins)
	$(msg_GENH)
	$(Q)rm -f $@
ifneq ($(HAVE_QT4),no)
	$(Q)xxd -i qt4gui.so >> $@
endif
ifneq ($(HAVE_QT5),no)
	$(Q)xxd -i qt5gui.so >> $@
endif

qt4gui.so: src/file_qtplugin_qt4.o
	$(msg_CXXLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs QtGui QtCore)

qt5gui.so: src/file_qtplugin_qt5.o
	$(msg_CXXLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs Qt5Widgets Qt5Core)

src/file_qtplugin_qt4.o: src/file_qtplugin.cpp icon_qrc.h
	$(msg_CXX)
	$(Q)$(CXX) $(plugin_CXXFLAGS) -Wno-unused-variable $(shell pkg-config --cflags QtGui QtCore) -c -o $@ $<

src/file_qtplugin_qt5.o: src/file_qtplugin.cpp icon_qrc.h
	$(msg_CXX)
	$(Q)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

icon_qrc.h: src/icon.png
	$(msg_GENH)
	$(Q)printf "static const unsigned char qt_resource_data[] = {\n " > $@
	$(Q)printf "%08x" $$(wc -c < $<) | sed 's|.\{2\}| 0x&,|g' >> $@
	$(Q)printf "\n" >> $@
	$(Q)xxd -i < $< >> $@
	$(Q)printf "};\n" >> $@

ifneq ($(EMBEDDED_PLUGINS),no)
src/file_dlopen_qtplugin.o: qtgui_so.h
else
src/file_dlopen_qtplugin.o: $(qtplugins)
endif

src/file.cpp: qtgui_so.h
src/file_qtplugin.cpp: $(libfltk)


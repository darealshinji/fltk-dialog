qtplugins = qtgui.so qtindicator.so

qtgui_so.h: $(qtplugins)
	$(msg_GENH)
	$(Q)xxd -i qtgui.so > $@

qtindicator_so.h: $(qtplugins)
	$(msg_GENH)
	$(Q)xxd -i qtindicator.so > $@

qtgui.so: src/file_qtplugin.o
	$(msg_CXXLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs Qt5Widgets Qt5Core)

qtindicator.so: src/indicator_qtplugin.o
	$(msg_CXXLDSO)
	$(Q)$(CXX) -shared -o $@ $^ $(LDFLAGS) -s $(shell pkg-config --libs Qt5Widgets Qt5Core) -lpthread -pthread

src/file_qtplugin.o: src/file_qtplugin.cpp
	$(msg_CXX)
	$(Q)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

src/indicator_qtplugin.o: src/indicator_qtplugin.cpp
	$(msg_CXX)
	$(Q)$(CXX) $(plugin_CXXFLAGS) $(shell pkg-config --cflags Qt5Widgets Qt5Core) -c -o $@ $<

ifneq ($(EMBEDDED_PLUGINS),no)
src/file.cpp: qtgui_so.h
src/file_dlopen_qtplugin.o: qtgui_so.h
src/indicator.cpp: qtindicator_so.h
else
src/file.cpp: $(qtplugins)
src/file_dlopen_qtplugin.o: $(qtplugins)
src/indicator.cpp: $(qtplugins)
endif

src/file_qtplugin.cpp: $(libfltk)
src/indicator_qtplugin.cpp: $(libfltk)


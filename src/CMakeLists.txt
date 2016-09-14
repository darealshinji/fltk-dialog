# To do: automatically build fltk and libpng as sub-projects

##################################################################
cmake_minimum_required(VERSION 2.8)
project(fltk-dialog)

set(fltk-dialog_VERSION_MAJOR 0)
set(fltk-dialog_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
include(CheckFunctionExists)
include(CheckLibraryExists)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING
		"Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
endif()

##################################################################
include_directories("${CMAKE_SOURCE_DIR}")

set(SOURCES
	Flek/FDate.cpp
	Flek/Fl_Calendar.cpp
	about.cpp
	choice.cpp
	message.cpp
	translate.cpp
	version.cpp
	main.cpp)

add_executable(fltk-dialog ${SOURCES})

##################################################################
check_function_exists(select HAVE_SELECT)
check_library_exists(dl dlopen "" HAVE_LIBDL)

if(HAVE_LIBDL)
	set(LIBDL_LIBRARY "-ldl")
	set(HAVE_DLOPEN 1)
else()
	check_function_exists(dlopen HAVE_DLOPEN)
endif()

find_package(LibNotify)

##################################################################
set(FLTK_SKIP_OPENGL 1)
set(FLTK_SKIP_FORMS 1)
set(FLTK_SKIP_FLUID 1)

find_package(FLTK)

##################################################################
option(WITH_DEFAULT_ICON "Use embedded default window icon" ON)

if(WITH_DEFAULT_ICON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_DEFAULT_ICON")
endif()

##################################################################
option(WITH_CALENDAR "Use calendar dialog" ON)

if(WITH_CALENDAR)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_CALENDAR")
	set(SOURCES	${SOURCES} calendar.cpp)
endif()

##################################################################
option(WITH_COLOR "Use color selection dialog" ON)

if(WITH_COLOR)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_COLOR")
	set(SOURCES	${SOURCES} color.cpp)
endif()

##################################################################
option(WITH_DND "Use drag & drop dialog" ON)

if(WITH_DND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_DND")
	set(SOURCES	${SOURCES} dnd.cpp)
endif()

##################################################################
option(WITH_ENTRY "Use entry dialog" ON)

if(WITH_ENTRY)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_ENTRY")
	set(SOURCES	${SOURCES} input.cpp)
endif()

##################################################################
option(WITH_FILE "Use file/directory chooser dialog" ON)

if(WITH_FILE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_FILE")
	set(SOURCES	${SOURCES} file.cpp)
endif()

##################################################################
option(WITH_HTML "Use HMTL dialog" ON)

if(WITH_HTML)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_HTML")
	set(SOURCES	${SOURCES} html.cpp)
endif()

##################################################################
option(WITH_PASSWORD "Use password dialog" ON)

if(WITH_PASSWORD)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_PASSWORD")
	set(SOURCES	${SOURCES} password.cpp)
endif()

##################################################################
option(WITH_PROGRESS "Use progress dialog" ON)

if(WITH_PROGRESS AND HAVE_SELECT)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_PROGRESS")
	set(SOURCES	${SOURCES} progress.cpp)
endif()

##################################################################
option(WITH_SCALE "Use scale dialog" ON)

if(WITH_SCALE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_SCALE")
	set(SOURCES	${SOURCES} slider.cpp)
endif()

##################################################################
option(WITH_TEXTINFO "Use text-info dialog" ON)

if(WITH_TEXTINFO AND HAVE_SELECT)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_TEXTINFO")
	set(SOURCES	${SOURCES} textinfo.cpp)
endif()

##################################################################
option(WITH_NOTIFY "Use notification dialog" ON)
option(DYNAMIC_NOTIFY "Use dlopen\(\) to load libnotify" ON)

if(NOT LIBNOTIFY_FOUND AND WITH_NOTIFY)
	message(STATUS "\ncannot find LibNotify - disable notification dialog\n")
	set(WITH_NOTIFY OFF)
endif()

if(NOT HAVE_DLOPEN AND DYNAMIC_NOTIFY AND WITH_NOTIFY)
	message(STATUS "\ncannot find dlopen() function - disable dynamic LibNotify\n")
	set(DYNAMIC_NOTIFY OFF)
endif()

if(WITH_NOTIFY)
	include_directories(${LIBNOTIFY_INCLUDE_DIRS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_NOTIFY")
	set(SOURCES	${SOURCES} notify.cpp)
	if(DYNAMIC_NOTIFY)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDYNAMIC_NOTIFY")
		set(LIBS ${LIBS} ${LIBDL_LIBRARY})
	else()
		set(LIBS ${LIBS} ${LIBNOTIFY_LIBRARIES})
	endif(DYNAMIC_NOTIFY)
endif()

##################################################################
option(WITH_WINDOW_ICON "Use window icon select option" ON)

if(WITH_WINDOW_ICON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_WINDOW_ICON")
	set(SOURCES	${SOURCES} window_icon.cpp)
endif()

##################################################################

target_link_libraries(fltk-dialog ${LIBS})

install(TARGETS fltk-dialog DESTINATION bin)


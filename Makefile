### UI Files ##################################################################

UI_FILES = Window.ui
UI_HEADERS = $(UI_FILES:%.ui=ui_%.h)

###############################################################################

INCS += -I/opt/local/include \
        -I/opt/local/include/QtGui \
        -I/opt/local/include/QtOpenGL \
        -I/opt/local/libexec/qt4/include 

LIBS += -L/opt/local/lib \
        -L/opt/local/libexec/qt4/lib \
	-lQtCore -lQtGui -lQtOpenGL \
	`pkg-config --libs opencv` \
        -framework OpenGL -framework GLUT

FLAGS =	-O4 -Wno-c++11-extensions -Wno-deprecated
#FLAGS =	-g -Wno-c++11-extensions -Wno-deprecated

# System libraries are chosen to be the same as the reference Makefile by qmake.
# OpenGL.framework is needed for glu (as Qt dropped it from its own QtOpenGL framework).

## Future Work: Use -framework for Qt libraries (from MacPorts)

### .cpp/.h Files and Object Files #############################################

CPP_SRCS = main.cpp \
	   Window.cpp \
	   Widget.cpp \
	   Coord2.cpp \
	   Graph.cpp \
	   Net.cpp \
	   Timer.cpp
CPP_HDRS = common.h \
	   Coord2.h \
	   Graph.h \
	   Net.h \
	   Timer.h
BINARY	=	qttag
## Header files that make use of Qt's Meta Object utility
HEADERS_WITH_QT_MOC =	Window.h \
                        Widget.h \

CPP_OBJS = $(CPP_SRCS:%.cpp=%.o)

MOC_OBJS = $(HEADERS_WITH_QT_MOC:%.h=moc_%.o)
# Future plan: .cpp files

###############################################################################

## Qt Meta Object Compiler ##
moc_%.cpp: %.h
	moc $< -o $@

## Qt UI File Compiler ##
ui_%.h: %.ui
	uic $< -o $@

%.o:	%.cpp
	$(CXX) -c $< $(INCS) $(FLAGS)

###############################################################################

all: release

release: ui_headers $(BINARY)

ui_headers: $(UI_HEADERS) 

qttag:	$(MOC_OBJS) $(CPP_OBJS)
	$(CXX) -o $@ $^ $(LIBS) $(FLAGS)

depend :
	-if [ -f Make-depend ] ; \
	  then rm -f      Make-depend ; \
	fi
	$(CXX) -MM $(FLAGS) $(INCS) $(CPP_SRCS) > Make-depend 

clean:
	rm *.o moc_*.cpp ui_*.h $(BINARY)

-include Make-depend

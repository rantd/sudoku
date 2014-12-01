CXX=g++
CXXFLAGS=`pkg-config opencv --cflags`
LDLIBS=`pkg-config opencv --libs`
SOURCES=main.cpp detector.cpp procdigit.cpp neural.cpp sudoku.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sudoku

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDLIBS) -o $@

clean :
	rm -f *.o sudoku

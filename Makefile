TARGET_APP = main.exe
TARGET_TEST = test

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
GTEST_FLAGS = -lgtest -lgtest_main -lpthread

DEPS = Sequence.h ArraySequence.h MutableArraySequence.h \
       ImmutableArraySequence.h ListSequence.h DynamicArray.h \
       LinkedList.h Exceptions.h Deque.h Vector.h SquareMatrix.h

OBJ_APP = main.o
OBJ_TEST = TestRun.o TestListSeq.o TestLinkList.o TestDynArr.o TestArrSeq.o \
           TestDeque.o TestVector.o TestSquareMatr.o

all: $(TARGET_APP) $(TARGET_TEST)

$(TARGET_APP): $(OBJ_APP)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(TARGET_TEST): $(OBJ_TEST)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(GTEST_FLAGS)

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

run_tests: $(TARGET_TEST)
	./$(TARGET_TEST)

clean:
	rm -f *.o $(TARGET_APP) $(TARGET_TEST)

.PHONY: all clean run_tests
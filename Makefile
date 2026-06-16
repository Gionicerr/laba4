CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

GTEST_LDFLAGS = -lgtest -lgtest_main -pthread

TEST_TARGET = run_tests
MAIN_TARGET = main.exe

TEST_SRC = LazyTest.cpp
MAIN_SRC = Main.cpp

all: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET) $(GTEST_LDFLAGS)

$(MAIN_TARGET): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

main: $(MAIN_TARGET)
	./$(MAIN_TARGET)

clean:
	rm -f $(TEST_TARGET) $(CONSOLE_TARGET)
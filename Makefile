# Компилятор
CXX = g++

# Флаги компиляции: 
# -std=c++17 : используем стандарт C++17
# -Wall -Wextra : включаем все стандартные и дополнительные предупреждения
# -pthread : требуется для Google Test
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Флаги линковки:
# Подключаем библиотеки gtest и gtest_main
LDFLAGS = -lgtest -lgtest_main -pthread

# Имя итогового исполняемого файла
TARGET = run_tests

# Исходные файлы для компиляции
SRC = TestLazy.cpp

# Правило по умолчанию (срабатывает просто при вводе команды make)
all: $(TARGET)

# Инструкция: как собирать TARGET из SRC
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Правило для быстрой компиляции и автоматического запуска тестов
test: $(TARGET)
	./$(TARGET)

# Правило для очистки скомпилированных файлов
clean:
	rm -f $(TARGET)
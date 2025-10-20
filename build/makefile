# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I../src
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

TARGET = ../bin/COGENT

# Файлы
SRCS = $(wildcard ../src/*.cpp)
OBJS = $(SRCS:../src/%.cpp=%.o)
DEPS = $(OBJS:.o=.d)

# Правила
all: $(TARGET)

$(TARGET): $(OBJS) | ../bin
	$(CXX) $(OBJS) -o $(TARGET)

%.o: ../src/%.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

../bin:
	mkdir -p ../bin

# Включение зависимостей
-include $(DEPS)

clean:
	rm -f $(OBJS) $(TARGET) *.d

distclean: clean
	rm -rf ../bin

# Запуск
run: $(TARGET)
	cd .. && ./bin/myprogram

.PHONY: all clean distclean run

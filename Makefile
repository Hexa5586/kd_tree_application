CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# 定义两个目标文件名 
TARGET1  = kd_tree_demo
TARGET2  = ips_generator
TARGET3  = ips_query

DEPS     = kd_tree.hpp

ifeq ($(OS),Windows_NT)
    RM = del /f /q
    FIX_PATH = $(subst /,\,$1)
    EXE = .exe
    CLEAN_QUERY = 2>nul || exit 0
else
    RM = rm -f
    FIX_PATH = $1
    EXE =
    CLEAN_QUERY =
endif

.PHONY: all clean run

all: $(TARGET1)$(EXE) $(TARGET2)$(EXE) $(TARGET3)$(EXE)

$(TARGET1)$(EXE): $(TARGET1).o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET2)$(EXE): $(TARGET2).o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET3)$(EXE): $(TARGET3).o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET3)$(EXE)

clean:
	$(RM) $(call FIX_PATH,*.o $(TARGET1)$(EXE) $(TARGET2)$(EXE) $(TARGET3)$(EXE)) ips.txt $(CLEAN_QUERY)
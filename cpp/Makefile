
CPP_FILES :=  sample.cc helper.cc optimizado.cc parallel.cc main.cc
H_FILES := sample.h helper.h optimizado.h parallel.h
TARGET := main.o
CXXFLAGS := -std=c++11 -Wall -Wextra  -pedantic  -O3



build: $(CPP_FILES) $(H_FILES)
	$(CXX) $(CXXFLAGS) $(CPP_FILES) -o $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET)





CPP_FILES :=  sample.cc helper.cc method.cc benchmark.cc fdif.cc
H_FILES := sample.h helper.h method.h fdif.h
TARGET := benchmark.o
CXXFLAGS := -std=c++14 -Wall -Wextra  -pedantic  -O3



build: $(CPP_FILES) $(H_FILES)
	$(CXX) $(CXXFLAGS) $(CPP_FILES) -o $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET)


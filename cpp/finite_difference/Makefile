
CPP_FILES := helper.cc method.cc 
H_FILES := helper.h 
TARGET := method.o
CXXFLAGS := -std=c++14 -Wall -Wextra  -pedantic  -O3



build: $(CPP_FILES) $(H_FILES)
	$(CXX) $(CXXFLAGS) $(CPP_FILES) -o $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET)




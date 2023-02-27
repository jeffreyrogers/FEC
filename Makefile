CXXFLAGS=-std=c++20 -Wall -Wextra -O2
INCFLAGS=-I src
BIN=test_codecs
BIN_UBSAN=$(BIN)_ubsan
BIN_ADDRSAN=$(BIN)_addrsan
SOURCES=src/test.cpp src/fec.h src/codec.h src/golay.h src/hamming.h

.PHONY: all
all: $(BIN) $(BIN_UBSAN) $(BIN_ADDRSAN)

$(BIN): $(SOURCES)
	$(CXX) -o $(BIN) $(CXXFLAGS) $(INCFLAGS) src/test.cpp

$(BIN_UBSAN): $(SOURCES)
	$(CXX) -o $(BIN_UBSAN) $(CXXFLAGS) -fsanitize=undefined $(INCFLAGS) src/test.cpp

$(BIN_ADDRSAN): $(SOURCES)
	$(CXX) -o $(BIN_ADDRSAN) $(CXXFLAGS) -fsanitize=address $(INCFLAGS) src/test.cpp

.PHONY: format
format:
	clang-format -i --style=LLVM src/*

.PHONY: clean
clean:
	rm -f $(BIN)

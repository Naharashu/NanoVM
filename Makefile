CXX=clang++
CXX_FLAGS=-O2 -g -pipe -flto=thin -std=c++20 -Wall
CXX_FLAGS_FASTER=-O3 -pipe -flto=thin -std=c++20 -Wall 
CXX_FLAGS_DEBUG=-O0 -g3 -pipe -std=c++20 -Wall -Wextra

ASMJIT := $(wildcard /usr/include/asmjit/x86.h)
ifeq ($(ASMJIT),)
$(error "ERROR - ASMJIT not found on that pc, please install asmjit")
endif


all: main

main:
	$(CXX) $(CXX_FLAGS) src/*.cpp -lasmjit  -o nanovm

native:
	@echo "WARNING - -march=native makes executable only work with your cpu"
	@echo "INFO - if you want more optimized executable, use make faster"
	$(CXX) $(CXX_FLAGS) src/*.cpp -lasmjit -march=native -o nanovm

faster:
	$(CXX) $(CXX_FLAGS_FASTER) src/*.cpp -lasmjit -o nanovm

debug:
	$(CXX) $(CXX_FLAGS_DEBUG) src/*.cpp -lasmjit -o nanovm

clean:
	rm -f ./nanovm

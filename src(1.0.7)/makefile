DEPS = ./ICUReadFile.cpp ./ICUMapping.cpp ./ICUNumberConverting.cpp \
		./ICUDictionary.cpp  ./SpecialCase.cpp  ./Address.cpp \
		./Math.cpp ./DateTime.cpp 

MAIN = main.cpp
TEST = test.cpp

main: $(DEPS) $(MAIN) makefile
	c++ -std=c++11 -o main $(MAIN) $(DEPS) \
		-lpthread -ldl -lm \
		-I../include \
		-L../lib \
		-licudata -licui18n -licuio -licuuc \

test: $(DEPS) $(TEST) makefile ./ICUHelper.h
	c++ -std=c++11 -o test $(TEST) $(DEPS) \
		-lpthread -ldl -lm \
		-I../include \
		-L../lib \
		-licudata -licui18n -licuio -licuuc \

clean:
	rm -f main log output.txt input.txt


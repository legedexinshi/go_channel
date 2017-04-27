EXEC = test_unbuffered test_buffered test_pointer

all : $(EXEC)

%: %.cpp
	g++ -std=c++11 $@.cpp -o $@

clean :
	rm $(EXEC)

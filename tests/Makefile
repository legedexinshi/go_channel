EXEC = test_unbuffered test_buffered test_pointer test_close

all : $(EXEC)

%: %.cpp
	g++ -std=c++11 $@.cpp -o $@

clean :
	rm $(EXEC)

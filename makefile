CXX=g++
#INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/ -I$(PROJECTPATH)
#LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader
CXXFLAGS=--std=c++11 -g -D DEBUG -Wall -Wno-sign-compare $(INCLUDEPATH)
LDFLAGS= $(LIBRARYPATH) 
OBJECTS=main.o conjugate_gradient_trainer.cpp 
EXECUTABLE=main

.PHONY: all clean check_headers gen_test big_test small_test

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

check_headers:
	g++ --std=c++11 -g -O3 -o main `ls *.hpp` -Wall -Wno-sign-compare 

gen_test:
	python test_gen.py
	
big_test: main
	./main < test.in > out.log
	python rmse.py out.log test.out

small_test: main
	./main < test2.in > out2.log
	python rmse.py out2.log test2.out

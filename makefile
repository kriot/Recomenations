CXX=g++
#INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/ -I$(PROJECTPATH)
#LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader
CXXFLAGS=--std=c++11 -c -g -D DEBUG -Wall -Wno-sign-compare $(INCLUDEPATH)
LDFLAGS=-lopencv_core $(LIBRARYPATH) 
OBJECTS=main.o conjugate_gradient_trainer.o data.o recommender.o recommender_trainer.o defines.o recommender_session_normal_around_user.o data_element.o
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

gen_sessions_test:
	python test_gen_with_sessions.py
	
big_test: main
	./main < test.in > out.log
	python rmse.py out.log test.out

small_test: main
	./main < test2.in > out2.log
	python rmse.py out2.log test2.out

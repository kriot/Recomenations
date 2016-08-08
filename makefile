all: main 

main: main.cpp
	g++ --std=c++11 -g -D DEBUG -o main main.cpp -Wall -Wno-sign-compare 
release: 
	g++ --std=c++11 -g -O3 -o main main.cpp -Wall -Wno-sign-compare 

check_headers:
	g++ --std=c++11 -g -O3 -o main `ls *.hpp` -Wall -Wno-sign-compare 

clean:
	rm -f main

gen_test:
	python test_gen.py
	
big_test: main
	./main < test.in > out.log
	python rmse.py out.log test.out

small_test: main
	./main < test2.in > out2.log
	python rmse.py out2.log test2.out

.PHONY: h1
SUPPORT = MySwitching.cpp MyER.cpp MyGraph.cpp mydegree.cpp johnson.cpp
all: h1 

h1:
	@session3T50Larabic

h2:
	@session3catalan

h3:
	@session3chinese

h4:
	@session3czech
	
supportObjects:
	g++ -c $(SUPPORT)
mainObject:
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++11 -fopenmp -MMD -MP -MF"main.d" -MT"main.o" -o "main.o" "./main.cpp"
session3:
	g++ -fopenmp -o "session3"  ./MyER.o ./MyGraph.o ./MySwitching.o ./johnson.o ./main.o ./mydegree.o 

clean:
	rm *.o
	rm session3

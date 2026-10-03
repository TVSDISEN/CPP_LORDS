CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC = src/Controller.cpp src/Comparator.cpp src/GainP.cpp src/GainI.cpp src/GainD.cpp \
      src/FeedbackGain.cpp src/Motor.cpp src/CSVOutput.cpp src/Simulator.cpp

all: motorsim

motorsim: main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) main.cpp $(SRC) -o motorsim

test_controller: $(SRC) tests/test_controller.cpp
	$(CXX) $(CXXFLAGS) $(SRC) tests/test_controller.cpp -o test_controller

test_motor: src/Motor.cpp tests/test_motor.cpp
	$(CXX) $(CXXFLAGS) src/Motor.cpp tests/test_motor.cpp -o test_motor

test_output: src/CSVOutput.cpp tests/test_output.cpp
	$(CXX) $(CXXFLAGS) src/CSVOutput.cpp tests/test_output.cpp -o test_output

clean:
	rm -f motorsim test_controller test_motor test_output
	rm -f data/*.csv
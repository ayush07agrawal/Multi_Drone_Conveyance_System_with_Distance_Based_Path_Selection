# Makefile — Multi-Drone Conveyance System (Theory + Simulation)
#
# Windows:  mingw32-make        / mingw32-make run / mingw32-make clean
# Linux:    make                / make run         / make clean

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

SRCS = main.cpp \
       ChargingStation.cpp \
       Drone.cpp \
       Graph.cpp \
       Simulator.cpp \
       MultiDroneSystem.cpp \
       Comparator.cpp \
       Scenarios.cpp

OBJS = $(SRCS:.cpp=.o)

ifeq ($(OS), Windows_NT)
    TARGET  = drone_sim.exe
    RM      = del /Q
    RUN_CMD = drone_sim.exe
else
    TARGET  = drone_sim
    RM      = rm -f
    RUN_CMD = ./drone_sim
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(RUN_CMD)

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: all run clean

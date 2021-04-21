#source files
SRCS = main.cpp input.cpp

#target name
TARG = sequential

#compiler, compile flags, linking flags
CC = g++
OPTS = -I /usr/local/include/opencv4/ -Wall -O -Wno-unused-result
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -pthread

OBJS = $(SRCS:.cpp=.o)

program: all tidy

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(OPTS) -c $< -o $@

d: OPTS += -g
d: $(TARG)

tidy:
	rm -f $(OBJS)

clean: tidy
	rm -f $(TARG)

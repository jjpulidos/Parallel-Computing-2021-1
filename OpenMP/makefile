#source files
SRCS = input.cpp
SRCS_SEQ = image-effect-seq.cpp $(SRCS)
SRCS_PAR = image-effect-par.cpp $(SRCS)

#target name
SEQ = sequential
PAR = paralel

#compiler, compile flags, linking flags
CC = g++
OPTS = -I /usr/local/include/opencv4/ -Wall -Wno-return-type -fopenmp #-O
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -pthread -fopenmp

OBJS_SEQ = $(SRCS_SEQ:.cpp=.o)
OBJS_PAR = $(SRCS_PAR:.cpp=.o)

program: all tidy

all: $(SEQ) $(PAR)

$(SEQ): $(OBJS_SEQ)
	$(CC) -o $(SEQ) $(OBJS_SEQ) $(LIBS)

$(PAR): $(OBJS_PAR)
	$(CC) -o $(PAR) $(OBJS_PAR) $(LIBS)

%.o: %.cpp
	$(CC) $(OPTS) -c $< -o $@

d: OPTS += -g
d: $(SEQ) $(PAR)

tidy:
	rm -f $(OBJS_SEQ) $(OBJS_PAR)

clean: tidy
	rm -f $(SEQ) $(PAR)

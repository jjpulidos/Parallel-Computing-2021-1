# nvcc input.cpp image-effect-par.cu -o parallel `pkg-config opencv4 --cflags --libs`
#source files
SRCS = input.cpp
# SRCS_SEQ = image-effect-seq.cpp $(SRCS)
SRCS_PAR = image-effect-par.cu $(SRCS)

#target name
# SEQ = sequential
PAR = paralel

#compiler, compile flags, linking flags
CC = nvcc
OPTS = -I /usr/local/include/opencv4/ #-O
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

ifeq ($(OPENCV), 1) 
COMMON+= -DOPENCV
CFLAGS+= -DOPENCV
LDFLAGS+= `pkg-config --libs opencv4` -lstdc++
COMMON+= `pkg-config --cflags opencv4` 
endif

# OBJS_SEQ = $(SRCS_SEQ:.cpp=.o)
OBJS_PAR = $(SRCS_PAR:.cu=.o)

program: all tidy

all: $(PAR)

# $(SEQ): $(OBJS_SEQ)
# 	$(CC) -o $(SEQ) $(OBJS_SEQ) $(LIBS)

$(PAR): $(OBJS_PAR)
	$(CC) -o $(PAR) $(OBJS_PAR) $(LIBS)

%.o: %.cu
	$(CC) $(OPTS) -c $< -o $@

d: OPTS += -g
d: $(PAR)

tidy:
	rm -f $(OBJS_PAR)

clean: tidy
	rm -f $(PAR)

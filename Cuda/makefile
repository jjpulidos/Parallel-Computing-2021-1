CFLAGS = `pkg-config --cflags opencv4`
LIBS = `pkg-config --libs opencv4`

all:
	nvcc input.cpp image-effect-cuda.cu -o cuda $(CFLAGS) $(LIBS)
# clean: 
# 	@rm -rf *~

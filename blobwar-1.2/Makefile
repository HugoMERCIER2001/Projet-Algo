

OBJS = strategy.o blobwar.o main.o font.o mouse.o image.o widget.o rollover.o button.o label.o board.o rules.o blob.o network.o bidiarray.o shmem.o

OBJS_launchComputation = launchStrategy.o strategy.o bidiarray.o shmem.o


LIBS = -lSDL_image -lSDL_ttf -lm `sdl-config --libs` -lSDL_net -lpthread

CFLAGS = -Wall -Werror -O3 -g `sdl-config --cflags`  -Wno-strict-aliasing -DDEBUG
CC = g++

# $(sort) remove duplicate object
OBJS_ALL = $(sort $(OBJS) $(OBJS_launchComputation))

blobwar: $(OBJS) launchStrategy
	$(CC) $(OBJS) $(CFLAGS) -o blobwar $(LIBS)
$(OBJS_ALL):	%.o:	%.cc common.h
	$(CC) -c $<  $(CFLAGS)
launchStrategy: $(OBJS_launchComputation)
	$(CC) $(OBJS_launchComputation) $(CFLAGS) -o launchStrategy $(LIBS)
clean:
	rm -f *.o core blobwar launchStrategy *~

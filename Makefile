# SDL 2 (Use it for modern PCs, Android or ARM devices with embedded GPUs)

PRGNAME     = binary/pac.elf
CC			= gcc
CXX			= g++
NETWORK		= NO

SRCDIR		= VS_files/Voxer
SRCDIR		+= ./GLU/src/libutil GLU/src/libtess GLU/src/libnurbs/interface GLU/src/libnurbs/internals GLU/src/libnurbs/nurbtess
VPATH		= $(SRCDIR)
SRC_CP		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_CP		= $(notdir $(patsubst %.c, %.o, $(SRC_CP)))
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJS		= $(OBJ_CP) $(OBJ_C)

CXXFLAGS		= -O0 -g3 -Wall -Wextra -I/usr/include/SDL2 -Wno-write-strings -Isrc -Isrc/include -I. -Iplatform/generic -IVS_files/Voxer -IGLU/include -IGLU/src/include
CXXFLAGS		+= -DLSB_FIRST -DSDL2 -DHOME_SUPPORT -DFORCE_FULLSCREEN
CXXFLAGS		+= -Ilib3ds

ifeq ($(PROFILE), YES)
CXXFLAGS 		+= -fprofile-generate=./
else ifeq ($(PROFILE), APPLY)
CXXFLAGS		+= -fprofile-use -fbranch-probabilities
endif

CFLAGS		= $(CXXFLAGS)

LDFLAGS     = -lc -lgcc -lm -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lfreetype -lGL


# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(CXX) $(CXXFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_CP) : %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_CP) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o

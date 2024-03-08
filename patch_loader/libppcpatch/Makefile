
EE_LIB = libppcpatch.a
EE_OBJS = patchbuffer.o patchbuffer_full.o

#prep patch before building patch_loader
all: $(EE_LIB)

clean:
	rm -f $(EE_LIB) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

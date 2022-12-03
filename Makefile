include config.mk
include lib/libs.mk
all: out/cgrass

out/cgrass: obj/main.c.o out
	if [ -n '$(wildcard obj/*.cpp.o)' ]; then $(CXX) $(LDFLAGS) -o'out/cgrass' $(wildcard obj/*.o) $(wildcard lib/bin/*.a); else $(CC) $(LDFLAGS) -o'out/cgrass' $(wildcard obj/*.o) $(wildcard lib/bin/*.a); fi
	$(OBJCOPY) --only-keep-debug 'out/cgrass' 'out/cgrass.dbg'
	chmod -x 'out/cgrass.dbg'
	$(OBJCOPY) --strip-unneeded 'out/cgrass'
	$(OBJCOPY) --add-gnu-debuglink='out/cgrass.dbg' 'out/cgrass'

obj/%.cpp.o: obj src/%.cpp
	$(CXX) -c -o'$@' 'src/$(patsubst obj/%.cpp.o,%,$@).cpp' $(CXXFLAGS)

obj/%.c.o: obj src/%.c
	$(CC) -c -o'$@' 'src/$(patsubst obj/%.c.o,%,$@).c' $(CFLAGS)

clean:
	$(RM) -r out
	$(RM) -r obj

out:
	$(MKDIR) out

obj:
	$(MKDIR) obj

.PHONY: clean all

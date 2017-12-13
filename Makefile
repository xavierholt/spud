OBJS=\
  obj/cache.o \
  obj/dh-ratchet.o \
  obj/hmac-ratchet.o \
  obj/session.o \
  obj/wrappers.o

TESTS=\
	example.out \
	session.out

CXX_FLAGS=-Wall -Werror

objects: $(OBJS)

tests: objects $(TESTS)

obj/%.o : src/%.cpp
	${CXX} $(CXX_FLAGS) -c -o $@ $<

%.out: test/%.cpp
	${CXX} $(CXX_FLAGS) -std=c++11 -g -o $@ $< lib/nacl/* obj/*.o

clean:
	rm obj/*.o
	rm *.out
OBJS=\
  obj/cache.o \
  obj/dh-ratchet.o \
  obj/hmac-ratchet.o \
  obj/session.o \
  obj/socket.o \
  obj/state.o \
  obj/wrappers.o

TESTS=\
	keygen.out \
	example.out \
	session.out \
	server.out \
	client.out

CXX_FLAGS=-Wall -Werror

objects: $(OBJS)

tests: objects $(TESTS)

obj/%.o : src/%.cpp
	${CXX} $(CXX_FLAGS) -c -o $@ $<

%.out: test/%.cpp
	${CXX} $(CXX_FLAGS) -std=c++11 -o $@ $< lib/nacl/* obj/*.o

clean:
	rm obj/*.o
	rm *.out

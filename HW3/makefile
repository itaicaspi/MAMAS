CC= g++
OBJS= Cache.o MemHier.o cacheSim.o
EXEC= CacheSim
FLAGS= -O0 -g -Wall -Werror

$(EXEC) : $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@
Cache.o : Cache.cpp Cache.h
	$(CC) -c $(FLAGS) $*.cpp
cacheSim.o : cacheSim.cpp MemHier.h Cache.h
	$(CC) -c $(FLAGS) $*.cpp
MemHier.o : MemHier.cpp MemHier.h Cache.h
	$(CC) -c $(FLAGS) $*.cpp

clean: 
	rm -f $(OBJS) $(EXEC)
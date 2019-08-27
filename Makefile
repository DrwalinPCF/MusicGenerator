
CFLAGS = -Ofast -m64 -s

all: wavgenerate.exe notesgenerator.exe
	@echo done

wavgenerate.exe: bin/wavgenerate.o bin/wav.o bin/noteparser.o
	g++ -o $@ $^ $(CFLAGS)

notesgenerator.exe: bin/notesgenerator.o
	g++ -o $@ $^ $(CFLAGS)

bin/%.o: src/%.cpp
	g++ -c $(CFLAGS) -o $@ $<

clear:
	del bin\*.o /F
	del wavgenerate.exe /F
	del notesgenerator.exe /F
	cls

rsim: cpu.o syscall.o stages.o simulator.o memory.o dump.o
	g++ $(FLAGS) -m64 $^ -o rsim -lpcap

cpu.o: sim/cpu.cc sim/memory.h sim/types.h sim/cpu.h sim/stages.h sim/instruction.h sim/syscall.h sim/dump.h
	g++ $(FLAGS) -m64 -c sim/cpu.cc

stages.o: sim/stages.cc sim/cpu.h sim/memory.h sim/types.h sim/instruction.h sim/stages.h 
	g++ $(FLAGS) -m64 -c sim/stages.cc

syscall.o: sim/syscall.cc sim/syscall.h sim/cpu.h sim/memory.h sim/types.h sim/instruction.h sim/stages.h 
	g++ $(FLAGS) -m64 -c sim/syscall.cc

dump.o: sim/dump.cc sim/memory.h sim/cpu.h sim/cpu.cc sim/dump.h
	g++ $(FLAGS) -m64 -c sim/dump.cc

simulator.o: sim/simulator.cc sim/memory.h sim/types.h sim/cpu.h sim/instruction.h sim/stages.h sim/dump.h 
	g++ $(FLAGS) -m64 -c sim/simulator.cc

memory.o: sim/memory.cc sim/memory.h sim/types.h
	g++ $(FLAGS) -m64 -c sim/memory.cc

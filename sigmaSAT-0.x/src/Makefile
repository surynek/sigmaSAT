all: debug

debug:	experiment_main.o_dbg system_main.o_dbg system.o_dbg graph.o_dbg encode.o_dbg experiment.o_dbg local.o_dbg test.o_dbg 
	g++ -otestSIGMA system.o_dbg experiment.o_dbg graph.o_dbg test.o_dbg
	g++ -osystemSIGMA graph.o_dbg system.o_dbg system_main.o_dbg
	g++ -oencodeSIGMA encode.o_dbg
	g++ -olocalSIGMA local.o_dbg
	g++ -oexperimentSIGMA system.o_dbg graph.o_dbg experiment.o_dbg experiment_main.o_dbg

optimized:	system.o_opt graph.o_opt encode.o_opt local.o_opt experiment.o_opt test.o_opt experiment_main.o_opt system_main.o_opt 
	g++ -otestSIGMA system.o_opt experiment.o_opt graph.o_opt test.o_opt
	g++ -osystemSIGMA graph.o_opt system.o_opt system_main.o_opt
	g++ -oencodeSIGMA encode.o_opt
	g++ -olocalSIGMA local.o_opt
	g++ -oexperimentSIGMA system.o_opt graph.o_opt experiment.o_opt experiment_main.o_opt

system.o_dbg:	version.h defs.h config.h graph.h system.h experiment.h system.cpp
	g++ -Wall -Wextra -pedantic -g -c -osystem.o_dbg system.cpp

experiment.o_dbg:	version.h defs.h config.h graph.h system.h experiment.h experiment.cpp
	g++ -Wall -Wextra -pedantic -g -c -oexperiment.o_dbg experiment.cpp

graph.o_dbg:	version.h defs.h config.h graph.h system.h experiment.h graph.cpp
	g++ -Wall -Wextra -pedantic -g -c -ograph.o_dbg graph.cpp

test.o_dbg:	version.h defs.h config.h graph.h system.h experiment.h test.cpp
	g++ -Wall -Wextra -pedantic -g -c -otest.o_dbg test.cpp

system_main.o_dbg:	version.h defs.h config.h graph.h system.h system_main.cpp
	g++ -Wall -Wextra -pedantic -g -c -osystem_main.o_dbg system_main.cpp

encode.o_dbg:	version.h defs.h config.h graph.h system.h encode.cpp
	g++ -Wall -Wextra -pedantic -g -c -oencode.o_dbg encode.cpp

local.o_dbg:	version.h defs.h config.h graph.h system.h local.cpp
	g++ -Wall -Wextra -pedantic -g -c -olocal.o_dbg local.cpp

experiment_main.o_dbg:	version.h defs.h config.h graph.h system.h experiment.h experiment_main.cpp
	g++ -Wall -Wextra -pedantic -g -c -oexperiment_main.o_dbg experiment_main.cpp


system.o_opt:	version.h defs.h config.h graph.h system.h experiment.h system.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -osystem.o_opt system.cpp

experiment.o_opt:	version.h defs.h config.h graph.h system.h experiment.h experiment.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -oexperiment.o_opt experiment.cpp

graph.o_opt:	version.h defs.h config.h graph.h system.h experiment.h graph.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -ograph.o_opt graph.cpp

test.o_opt:	version.h defs.h config.h graph.h system.h experiment.h test.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -otest.o_opt test.cpp

system_main.o_opt:	version.h defs.h config.h graph.h system.h system_main.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -osystem_main.o_opt system_main.cpp

encode.o_opt:	version.h defs.h config.h graph.h system.h encode.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -oencode.o_opt encode.cpp

local.o_opt:	version.h defs.h config.h graph.h system.h local.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -olocal.o_opt local.cpp

experiment_main.o_opt:	version.h defs.h config.h graph.h system.h experiment.h experiment_main.cpp
	g++ -Wall -Wextra -pedantic -c -O9 -mtune=native -oexperiment_main.o_opt experiment_main.cpp


clean:
	rm -f *~ *.o_dbg *.o_opt *.bak output.txt *.cnf testSIGMA systemSIGMA encodeSIGMA localSIGMA experimentSIGMA

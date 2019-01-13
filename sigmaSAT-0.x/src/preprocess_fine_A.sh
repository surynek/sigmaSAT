PATH=$PATH:../../preprocess/Shatter
export PATH

#python runpreprocess.py -sigma .sigma "./experimentSIGMA -tlpc2 -l12 -lp3.2 -up30.0 -al0" ../../preprocess_fine .cnf 2>preprocess_sigma_output.txt
#python runpreprocess.py -hypre .HyPre "../../preprocess/HyPre/hypre" ../../preprocess_fine .cnf 2>preprocess_HyPre_output.txt
#python runpreprocess.py -niver .NiVer "../../preprocess/Niver/niver" ../../preprocess_fine .cnf 2>preprocess_NiVer_output.txt
#python runpreprocess.py -liver .LiVer "../../preprocess/Liver/liver" ../../preprocess_fine .cnf 2>preprocess_LiVer_output.txt
#python runpreprocess.py -shatter .Shatter "../../preprocess/Shatter/shatter.pl" ../../preprocess_fine .cnf 2>preprocess_Shatter_output.txt


python runtest.py -st .cnf.mini "../../solvers/minisat_static -cpu-lim=256" ../../preprocess_fine .cnf
#python runtest.py -st .sigma.mini "../../solvers/minisat_static -cpu-lim=256" ../../preprocess_fine .sigma
#python runtest.py -st .HyPre.mini "../../solvers/minisat_static -cpu-lim=256" ../../preprocess_fine .HyPre
#python runtest.py -st .NiVer.mini "../../solvers/minisat_static -cpu-lim=256" ../../preprocess_fine .NiVer
#python runtest.py -st .LiVer.mini "../../solvers/minisat_static -cpu-lim=256" ../../preprocess_fine .LiVer
#python runtest.py -st .Shatter.mini "../../solvers/minisat_static -cpu-lim=256" ../../preprocess_fine .Shatter
# access-control-logging-system-developed-in-C

Program needs a prog.c program from user (included example), that will do stuff with files(open-fopen-write-fwrite) and of course the files that
the prog tries to access. It creates with append mode a file in /tmp direcotry as a logfile.

make all 												                      ->BUILD 
make run_log 									                        ->update logfile
make functionality=1 run_monitor 						          ->run monitor with functionality 1
make functionality=2 path="given path" run_monitor  	->run monitor with functionality 2
make functionality=3 run_monitor  						        ->run monitor with functionality 3
make clear	 											                    ->CLEAN LOGFILE
make clean	 											                    ->erase executable users program and .so library

-my_handle.c: creates the log files and appends the file accesses information
-Log_Monitoring.c: opens the logfile and performs checks

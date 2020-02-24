all: my_handle.c
	gcc prog.c -o prog
	gcc -Wall -fPIC -shared -o my_handle.so my_handle.c -ldl -lssl -lcrypto
	gcc Log_Monitoring.c -o log_monitoring

run_log: my_handle.so prog
	LD_PRELOAD=./my_handle.so ./prog

run_monitor: log_monitoring
	./log_monitoring /tmp/access_control_logfile.txt ${functionality} ${path}

clean:
	rm -rf my_handle.so prog log_monitoring

clear:
	>/tmp/access_control_logfile.txt
	>test.txt

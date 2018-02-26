
DIR_TEST = ./EventLoop_test ./Client_test ./Server_test \
			./Timer_test

tests: 
	for dir in $(DIR_TEST); do\
		(cd $$dir; ${MAKE});\
	done

.PHONY: clean

clean:
	for dir in $(DIR_TEST); do\
		(cd $$dir; ${MAKE} clean);\
	done
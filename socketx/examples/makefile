
DIR_EXP = ./echo ./multi-echo ./messageQueue

examples: 
	for dir in $(DIR_EXP); do\
		(cd $$dir; ${MAKE});\
	done

.PHONY: clean

clean:
	for dir in $(DIR_EXP); do\
		(cd $$dir; ${MAKE} clean);\
	done
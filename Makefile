OBJS += src/main.o src/arghandler.o src/logging.o
OUT ?= build/tcap
DBGOUT ?= debug/tcap

CC = gcc
CCLD = gcc
WFLAGS += -Wall -Wextra -Wpedantic -Werror
CFLAGS += -O2
LFLAGS += 

CFLAGS_FILE ?= compile_flags.txt

.SUFFIXES: .c .o

.c.o:
	@echo -e "\tCC $<"
	@$(CC) -c $(WFLAGS) $(CFLAGS) -g3 $< -o $@

all : $(OUT)

$(OUT): $(OBJS)
	@mkdir -p build
	@echo -e "\tCCLD $(OBJS)"
	@$(CCLD) $(LFLAGS) -s $(OBJS) -o $(OUT)
	
	@echo ""
	@echo "Output: $(OUT)"
	@echo "Finished."

debug: $(OBJS)
	@mkdir -p debug
	
	@echo -e "\tCCLD $(OBJS)"
	@$(CCLD) $(LFLAGS) -g3 $(OBJS) -o $(DBGOUT)

	@echo ""
	@echo "Output: $(DBGOUT)"
	@echo "Finished."

.PHONY: clean gen_cflags

clean:
	@echo -e "\tRM $(OBJS) $(OUT) build"
	@rm -f $(OBJS) $(OUT)
	@rm -rf build/

gen_cflags:
	@echo $(WFLAGS) $(CFLAGS) $(LFLAGS) | sed -z 's/ /\n/g' | tee $(CFLAGS_FILE)
	@echo -e "\t|"
	@echo -e "\tV"
	@echo -e $(CFLAGS_FILE)

OBJS += src/main.o src/arghandler.o src/logging.o
OUT ?= build/tcap
DBGOUT ?= debug/tcap

CCLD = $(CC)
WFLAGS += -Wall -Wextra -Wpedantic -Werror
CFLAGS += -O2
LFLAGS ?= -s

CFLAGS_FILE ?= compile_flags.txt

.SUFFIXES: .c .o

.c.o:
	@echo -e "\tCC $<"
	@$(CC) -c $(WFLAGS) $(CFLAGS) -g3 $< -o $@

all : $(OUT)

$(OUT): $(OBJS)
	@mkdir -p build
	@echo -e "\tCCLD $(OBJS)"
	@$(CCLD) $(LFLAGS) $(OBJS) -o $(OUT)
	
	@echo ""
	@echo "Output: $(OUT)"
	@echo "Finished."


debug: 
	@mkdir -p debug
	@rm -f $(DBGOUT)
	@CFLAGS+="-Ddebug" LFLAGS+="-g3" OUT=$(DBGOUT) $(MAKE)

.PHONY: clean gen_cflags debug

clean:
	@echo -e "\tRM $(OBJS) $(OUT) build"
	@rm -f $(OBJS) $(OUT)
	@rm -rf build/
	@rm -rf debug/

gen_cflags:
	@echo $(WFLAGS) $(CFLAGS) $(LFLAGS) | sed -z 's/ /\n/g' | tee $(CFLAGS_FILE)
	@echo -e "\t|"
	@echo -e "\tV"
	@echo -e $(CFLAGS_FILE)

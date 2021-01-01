LLVM = clang-11
BIN_DIR = ../bin
OBJS = ping.c
TARGET = ping

default: $(TARGET)

$(TARGET):
	mkdir -p $(BIN_DIR)
	$(LLVM) -o $(BIN_DIR)/$(TARGET) $(OBJS)

.PHONY: clean
clean:
	-rm -r $(BIN_DIR)/$(TARGET)

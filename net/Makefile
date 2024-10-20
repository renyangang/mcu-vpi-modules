ifeq ($(OS),Windows_NT)
    LIB_EXT := .lib
	MKDIR := powershell -Command "New-Item -ItemType Directory -Force"
    RMDIR := powershell -Command "Remove-Item -Recurse -Force"
    RM := powershell -Command "Remove-Item -Recurse -Force"
	VPICC := iverilog-vpi.exe
else
	LIB_EXT := .a
	MKDIR := mkdir -p
    RMDIR := rm -rf
    RM := rm -f
	VPICC := iverilog-vpi
endif

VPIDIR := D:/soft/iverilog

BUILD_DIR := ./lib


all:
	$(MKDIR) $(BUILD_DIR)
	go build -buildmode=c-archive -o $(BUILD_DIR)/libnet$(LIB_EXT)
	gcc -shared -I$(BUILD_DIR) -I$(VPIDIR)/include/iverilog -o $(BUILD_DIR)/netm.vpi net.c $(BUILD_DIR)/libnet$(LIB_EXT) $(VPIDIR)/lib/libvpi.a
clean:
	$(RMDIR) $(BUILD_DIR)


.DEFAULT_GOAL := all
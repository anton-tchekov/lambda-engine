# Optimization level for release builds
OPT=2

# --- Linux ---
# Compiler
CC_LINUX=gcc

# Debug ar
AR_LINUX_DEBUG=ar

# Release ar (for LTO)
AR_LINUX_RELEASE=gcc-ar

# Compiler flags
CFLAGS_LINUX=$(FOLDER) -fPIC -Wall -Wextra -Wshadow -Wconversion -std=c99

# Release object file directory
OBJDIR_LINUX_RELEASE=obj/linux64_release

# Debug object file directory
OBJDIR_LINUX_DEBUG=obj/linux64_debug

# --- Windows ---
# 32-bit compiler
CC_WIN32=i686-w64-mingw32-gcc

# 64-bit compiler
CC_WIN64=x86_64-w64-mingw32-gcc

# 32-bit ar (for LTO)
AR_WIN32=i686-w64-mingw32-gcc-ar

# 64-bit ar (for LTO)
AR_WIN64=x86_64-w64-mingw32-gcc-ar

# Compiler flags
CFLAGS_WINDOWS=$(FOLDER) -fPIC -Wall -Wextra -Wshadow -Wconversion -std=c99

# 32-bit object file directory
OBJDIR_WIN32=obj/win32

# 64-bit object file directory
OBJDIR_WIN64=obj/win64

# Include source and header files list
FOLDER=
SOURCE=
HEADER=
SHADER=
include game.mk

# Generate list of object files
OBJS_LINUX_DEBUG=$(join $(addprefix obj/linux64_debug/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.o)))
OBJS_LINUX_RELEASE=$(join $(addprefix obj/linux64_release/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.o)))
OBJS_WIN32=$(join $(addprefix obj/win32/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.o)))
OBJS_WIN64=$(join $(addprefix obj/win64/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.o)))

# List of glsl shader sources
SHADER_LIST=$(addprefix src/, $(SHADER))

HEADER_COMMON=$(addprefix src/, $(HEADER))

# Compile for all architectures and build types
.PHONY: all
all: linux_debug linux_release win32 win64

%.glsl:

.PHONY: shaders
shaders: $(SHADER_LIST)
	./util/shader_packer src/shaders $(SHADER_LIST)
	./util/shader_checker src/glsl/renderer2d_vs.glsl src/glsl/renderer2d_fs.glsl
	./util/shader_checker src/glsl/shadow_vs.glsl src/glsl/shadow_fs.glsl
	./util/shader_checker src/glsl/shadow_map_vs.glsl src/glsl/shadow_map_fs.glsl
	./util/shader_checker src/glsl/shadow_bw_vs.glsl src/glsl/shadow_bw_fs.glsl
	./util/shader_checker src/glsl/blur_vs.glsl src/glsl/blur_fs.glsl
	./util/shader_checker src/glsl/lighting_vs.glsl src/glsl/lighting_fs.glsl
	./util/shader_checker src/glsl/billboard_vs.glsl src/glsl/billboard_fs.glsl
	./util/shader_checker src/glsl/billboard_sel_vs.glsl src/glsl/billboard_sel_fs.glsl
	./util/shader_checker src/glsl/skybox_vs.glsl src/glsl/skybox_fs.glsl
	./util/shader_checker src/glsl/selection_vs.glsl src/glsl/selection_fs.glsl
	./util/shader_checker src/glsl/selection_position_vs.glsl src/glsl/selection_position_fs.glsl

# Linux debug rules
obj/linux64_debug/%.o: src/%.c
	$(CC_LINUX) -c -o $@ $< $(CFLAGS_LINUX) -DCOLOR_TERMINAL -DBITS64 -DDEBUG -g

.PHONY: linux_debug
linux_debug: shaders $(OBJS_LINUX_DEBUG)
	@echo
	@echo Generating static library
	@echo
	$(AR_LINUX_DEBUG) rcs bin/linux64_debug/engine.a $(OBJS_LINUX_DEBUG)

# Linux release rules
obj/linux64_release/%.o: src/%.c
	$(CC_LINUX) -c -o $@ $< $(CFLAGS_LINUX) -DCOLOR_TERMINAL -DBITS64 -ffast-math -O$(OPT) -flto

.PHONY: linux_release
linux_release: shaders $(OBJS_LINUX_RELEASE)
	@echo
	@echo Generating static library
	@echo
	$(AR_LINUX_RELEASE) rcs bin/linux64_release/engine.a $(OBJS_LINUX_RELEASE)

# Windows 32-bit rules
obj/win32/%.o: src/%.c
	$(CC_WIN32) -c -o $@ $< $(CFLAGS_WINDOWS) -DBITS32 -ffast-math -O$(OPT) -flto

.PHONY: win32
win32: shaders $(OBJS_WIN32)
	@echo
	@echo Generating static library
	@echo
	$(AR_WIN32) rcs bin/win32/engine.a $(OBJS_WIN32)

# Windows 64-bit rules
obj/win64/%.o: src/%.c
	$(CC_WIN64) -c -o $@ $< $(CFLAGS_WINDOWS) -DBITS64 -ffast-math -O$(OPT)

.PHONY: win64
win64: shaders $(OBJS_WIN64)
	@echo
	@echo Generating static library
	@echo
	$(AR_WIN64) rcs bin/win64/engine.a $(OBJS_WIN64)

# Delete all object files
.PHONY: clean
clean:
	find obj -name '*.o' -exec rm -r {} \;

# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug_x64
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

RESCOMP = windres
INCLUDES += -I../source -I../source/src -I../source/include -I../raylib/src -I../raylib/src/external -I../raylib/src/external/glfw/include
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),debug_x64)
TARGETDIR = ../_bin/Debug
TARGET = $(TARGETDIR)/NightSkyEngineLite.exe
OBJDIR = obj/x64/Debug/NightSkyEngineLite
DEFINES += -DDEBUG -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -D_WIN32
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -g -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -g -std=c++11
LIBS += ../_bin/Debug/raylib.lib -lwinmm -lkernel32 -lopengl32 -lgdi32
LDDEPS += ../_bin/Debug/raylib.lib
ALL_LDFLAGS += $(LDFLAGS) -L../_bin/Debug -L/usr/lib64 -m64

else ifeq ($(config),debug_x86)
TARGETDIR = ../_bin/Debug
TARGET = $(TARGETDIR)/NightSkyEngineLite.exe
OBJDIR = obj/x86/Debug/NightSkyEngineLite
DEFINES += -DDEBUG -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -D_WIN32
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m32 -g -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m32 -g -std=c++11
LIBS += ../_bin/Debug/raylib.lib -lwinmm -lkernel32 -lopengl32 -lgdi32
LDDEPS += ../_bin/Debug/raylib.lib
ALL_LDFLAGS += $(LDFLAGS) -L../_bin/Debug -L/usr/lib32 -m32

else ifeq ($(config),release_x64)
TARGETDIR = ../_bin/Release
TARGET = $(TARGETDIR)/NightSkyEngineLite.exe
OBJDIR = obj/x64/Release/NightSkyEngineLite
DEFINES += -DNDEBUG -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -D_WIN32
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -std=c++11
LIBS += ../_bin/Release/raylib.lib -lwinmm -lkernel32 -lopengl32 -lgdi32
LDDEPS += ../_bin/Release/raylib.lib
ALL_LDFLAGS += $(LDFLAGS) -L../_bin/Release -L/usr/lib64 -m64 -mwindows -s

else ifeq ($(config),release_x86)
TARGETDIR = ../_bin/Release
TARGET = $(TARGETDIR)/NightSkyEngineLite.exe
OBJDIR = obj/x86/Release/NightSkyEngineLite
DEFINES += -DNDEBUG -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -D_WIN32
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m32 -O2 -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m32 -O2 -std=c++11
LIBS += ../_bin/Release/raylib.lib -lwinmm -lkernel32 -lopengl32 -lgdi32
LDDEPS += ../_bin/Release/raylib.lib
ALL_LDFLAGS += $(LDFLAGS) -L../_bin/Release -L/usr/lib32 -m32 -mwindows -s

endif

# Per File Configurations
# #############################################


# File sets
# #############################################

GENERATED :=
OBJECTS :=

GENERATED += $(OBJDIR)/AtlasSprite.o
GENERATED += $(OBJDIR)/BattleActor.o
GENERATED += $(OBJDIR)/CString.o
GENERATED += $(OBJDIR)/FighterGameState.o
GENERATED += $(OBJDIR)/InputBuffer.o
GENERATED += $(OBJDIR)/PlayerCharacter.o
GENERATED += $(OBJDIR)/ScriptAnalyzer.o
GENERATED += $(OBJDIR)/State.o
GENERATED += $(OBJDIR)/StateMachine.o
GENERATED += $(OBJDIR)/Subroutine.o
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/AtlasSprite.o
OBJECTS += $(OBJDIR)/BattleActor.o
OBJECTS += $(OBJDIR)/CString.o
OBJECTS += $(OBJDIR)/FighterGameState.o
OBJECTS += $(OBJDIR)/InputBuffer.o
OBJECTS += $(OBJDIR)/PlayerCharacter.o
OBJECTS += $(OBJDIR)/ScriptAnalyzer.o
OBJECTS += $(OBJDIR)/State.o
OBJECTS += $(OBJDIR)/StateMachine.o
OBJECTS += $(OBJDIR)/Subroutine.o
OBJECTS += $(OBJDIR)/main.o

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking NightSkyEngineLite
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning NightSkyEngineLite
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(GENERATED)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(GENERATED)) del /s /q $(subst /,\\,$(GENERATED))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/AtlasSprite.o: ../source/AtlasSprite.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/BattleActor.o: ../source/Battle/Actors/BattleActor.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/FighterGameState.o: ../source/Battle/Actors/FighterGameState.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/PlayerCharacter.o: ../source/Battle/Actors/PlayerCharacter.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/InputBuffer.o: ../source/Battle/InputBuffer.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/ScriptAnalyzer.o: ../source/Battle/NightSkyScript/ScriptAnalyzer.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/State.o: ../source/Battle/State.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/StateMachine.o: ../source/Battle/StateMachine.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/Subroutine.o: ../source/Battle/Subroutine.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/CString.o: ../source/CString.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: ../source/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif
CXX_FLAGS=/nologo /EHsc /c

# This is the default python install path
PYTHON_PATH=$(LOCALAPPDATA)\Programs\Python\Python37

LINK_FLAGS=/nologo

!ifndef CONFIG
CONFIG=debug
CXX_FLAGS=$(CXX_FLAGS) /Zi
LINK_FLAGS=$(LINK_FLAGS) /DEBUG
!endif

!if "$(CONFIG)" == "release"
CXX_FLAGS=$(CXX_FLAGS) /O2
!endif

OUT_DIR=..\build\$(CONFIG)
OBJ_DIR=$(OUT_DIR)\source
DETOURS_OBJ_DIR=$(OBJ_DIR)\detours

all: dirs \
	$(OUT_DIR)\test_compiler.exe \
	$(OUT_DIR)\vapid_injection64.dll \
	$(OUT_DIR)\vapid.exe \
	$(OUT_DIR)\UnitTests.exe \
	$(OUT_DIR)\CacheServer.exe\
	$(OUT_DIR)\WorkerPoolServer.exe

#	$(OUT_DIR)\vapid.pyd \

dirs:
	@if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)" && echo.   Created $(OUT_DIR)
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)" && echo.   Created $(OBJ_DIR)
	@if not exist "$(DETOURS_OBJ_DIR)" mkdir "$(DETOURS_OBJ_DIR)" && echo.   Created $(DETOURS_OBJ_DIR)

clean:
    @rmdir /q /s $(OUT_DIR) 2>nul
    @echo Deleted $(OUT_DIR)

AllHeaders = \
	Assert.h\
	AtScopeExit.h\
	BufOps.h\
	DebugLog.h\
	MutexProtectedResource.h\
	NetworkUtil.h\
	GeneratedCommon.h\
	Hash.h\
	CacheServer.h\
	WorkerPoolServer.h\
	ArgParser.h\

Worker.inl : $(AllHeaders) GeneratedWorker.inl
Master.inl : $(AllHeaders) GeneratedMaster.inl

GeneratedCommon.h GeneratedMaster.inl GeneratedWorker.inl GeneratedDetours.inl : generate_overloaded.py GenerationInput.h
	python generate_overloaded.py

PCH=$(OBJ_DIR)\WindowsInclude.pch
USE_PCH=/FIWindowsInclude.h /YuWindowsInclude.h /Fp$(PCH) /I:$(OBJ_DIR)
PCH_OBJ=$(OBJ_DIR)\WindowsInclude.cpp.o

$(PCH): WindowsInclude.cpp WindowsInclude.h
	@$(CC) $(CXX_FLAGS) /YcWindowsInclude.h /Fp:$(PCH) /Fo:$(PCH_OBJ) WindowsInclude.cpp

$(OBJ_DIR)\test_compiler.cpp.o: test_compiler.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ test_compiler.cpp

$(OBJ_DIR)\UnitTests.cpp.o: UnitTests.cpp Worker.inl Master.inl $(AllHeaders) $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ UnitTests.cpp

$(OBJ_DIR)\vapid_injection.cpp.o: vapid_injection.cpp Worker.inl GeneratedDetours.inl $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ vapid_injection.cpp

$(OBJ_DIR)\vapid.cpp.o: vapid.cpp Master.inl $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ vapid.cpp

$(OBJ_DIR)\vapid_python.cpp.o: vapid_python.cpp $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /I$(PYTHON_PATH)\include /Fo:$@ vapid_python.cpp

$(OBJ_DIR)\vapid_master.cpp.o: vapid_master.cpp $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ vapid_master.cpp

$(OBJ_DIR)\Hash.cpp.o: Hash.cpp $(AllHeaders)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ Hash.cpp

$(OBJ_DIR)\CacheServer.cpp.o: CacheServer.cpp $(AllHeaders) $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ CacheServer.cpp

$(OBJ_DIR)\CacheServerMain.cpp.o: CacheServerMain.cpp $(AllHeaders) $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ CacheServerMain.cpp

$(OBJ_DIR)\WorkerPoolServer.cpp.o: WorkerPoolServer.cpp $(AllHeaders) $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ WorkerPoolServer.cpp

$(OBJ_DIR)\WorkerPoolServerMain.cpp.o: WorkerPoolServerMain.cpp $(AllHeaders) $(PCH)
	@$(CC) $(CXX_FLAGS) $(USE_PCH) /Fo:$@ WorkerPoolServerMain.cpp

$(DETOURS_OBJ_DIR)\creatwth.cpp.o: detours\creatwth.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\detours.cpp.o: detours\detours.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\modules.cpp.o: detours\modules.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\disasm.cpp.o: detours\disasm.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\image.cpp.o: detours\image.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\disolarm.cpp.o: detours\disolarm.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\disolarm64.cpp.o: detours\disolarm64.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\disolx86.cpp.o: detours\disolx86.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\disolx64.cpp.o: detours\disolx64.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

$(DETOURS_OBJ_DIR)\disolia64.cpp.o: detours\disolia64.cpp
	@$(CC) $(CXX_FLAGS) /Fo:$@ $**

DETOURS_OBJS=\
	$(DETOURS_OBJ_DIR)\creatwth.cpp.o \
	$(DETOURS_OBJ_DIR)\detours.cpp.o \
	$(DETOURS_OBJ_DIR)\modules.cpp.o \
	$(DETOURS_OBJ_DIR)\disasm.cpp.o \
	$(DETOURS_OBJ_DIR)\image.cpp.o \
	$(DETOURS_OBJ_DIR)\disolarm.cpp.o \
	$(DETOURS_OBJ_DIR)\disolarm64.cpp.o \
	$(DETOURS_OBJ_DIR)\disolx86.cpp.o \
	$(DETOURS_OBJ_DIR)\disolx64.cpp.o \
	$(DETOURS_OBJ_DIR)\disolia64.cpp.o \

$(OBJ_DIR)\detours.lib: $(DETOURS_OBJS)
	@lib /nologo /out:$@ $**

$(OUT_DIR)\test_compiler.exe: $(OBJ_DIR)\test_compiler.cpp.o
	@link $(LINK_FLAGS) /OUT:$@ $**
		
$(OUT_DIR)\UnitTests.exe: $(OBJ_DIR)\UnitTests.cpp.o $(OBJ_DIR)\Hash.cpp.o $(OBJ_DIR)\CacheServer.cpp.o $(OBJ_DIR)\WorkerPoolServer.cpp.o $(PCH_OBJ)
	@link $(LINK_FLAGS) /OUT:$@ ws2_32.lib $**
		
$(OUT_DIR)\CacheServer.exe: $(OBJ_DIR)\CacheServerMain.cpp.o $(OBJ_DIR)\CacheServer.cpp.o $(PCH_OBJ)
	@link $(LINK_FLAGS) /OUT:$@ ws2_32.lib $**
		
$(OUT_DIR)\WorkerPoolServer.exe: $(OBJ_DIR)\WorkerPoolServerMain.cpp.o $(OBJ_DIR)\WorkerPoolServer.cpp.o $(PCH_OBJ)
	@link $(LINK_FLAGS) /OUT:$@ ws2_32.lib $**
		
$(OUT_DIR)\vapid.exe: $(OBJ_DIR)\detours.lib $(OBJ_DIR)\vapid.cpp.o $(PCH_OBJ)
	@link $(LINK_FLAGS) /OUT:$@ ws2_32.lib $**
		
$(OUT_DIR)\vapid_injection64.dll: $(OBJ_DIR)\detours.lib $(OBJ_DIR)\vapid_injection.cpp.o $(PCH_OBJ)
	@link $(LINK_FLAGS) /export:DetourFinishHelperProcess,@1,NONAME /subsystem:console /DLL /OUT:$@ ws2_32.lib $**

$(OUT_DIR)\vapid.pyd: $(PYTHON_PATH)/libs/python3.lib $(OBJ_DIR)\vapid_python.cpp.o $(PCH_OBJ)
	@link $(LINK_FLAGS) /NOIMPLIB /NOENTRY /DLL /OUT:$@ $**

UnitTests: $(OUT_DIR)\UnitTests.exe
	@echo Running Unit Tests
	$(OUT_DIR)\UnitTests.exe
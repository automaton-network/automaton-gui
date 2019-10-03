# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DDEBUG=1 -D_DEBUG=1 -DJUCER_LINUX_MAKE_6D53C8B4=1 -DJUCE_APP_VERSION=0.0.1 -DJUCE_APP_VERSION_HEX=0x1 $(shell pkg-config --cflags x11 xinerama xext freetype2 libcurl) -pthread -I../../JuceLibraryCode -I../../../../automaton/src/local_third_party/JUCE/modules $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  JUCE_TARGET_APP := PlaygroundGUI

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -I ../../../../automaton/src -I ../../../../automaton/src/local_third_party/bitcoin/src -I ../../../../automaton/src/local_third_party/boost -I ../../../../automaton/src/local_third_party/openssl/include -I ../../../../automaton/src/local_third_party/cryptopp -I ../../../../automaton/src/local_third_party/curl/include -I ../../../../automaton/src/local_third_party/ed25519/src -I ../../../../automaton/src/local_third_party/g3log/build/include -I ../../../../automaton/src/local_third_party/g3log/src -I ../../../../automaton/src/local_third_party/gmp -I ../../../../automaton/src/local_third_party/googletest/googlemock/include -I ../../../../automaton/src/local_third_party/googletest/googletest/include -I ../../../../automaton/src/local_third_party/json -I ../../../../automaton/src/local_third_party/lua -I ../../../../automaton/src/local_third_party/protobuf/src -I ../../../../automaton/src/local_third_party/replxx/include -I ../../../../automaton/src/local_third_party/sol2/single/sol -I ../../../../automaton/src/local_third_party/zlib $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell pkg-config --libs x11 xinerama xext freetype2 libcurl) ../../../../automaton/src/build/libblockchain_cpp_node.a ../../../../automaton/src/build/libautomaton-core.a ../../../../automaton/src/build/libkoh-miner.a ../../../../automaton/src/local_third_party/bitcoin/src/secp256k1/.libs/libsecp256k1.a ../../../../automaton/src/local_third_party/boost/stage/lib/libboost_filesystem.a ../../../../automaton/src/local_third_party/boost/stage/lib/libboost_iostreams.a ../../../../automaton/src/local_third_party/boost/stage/lib/libboost_system.a ../../../../automaton/src/local_third_party/openssl/libssl.a ../../../../automaton/src/local_third_party/openssl/libcrypto.a ../../../../automaton/src/local_third_party/cryptopp/libcryptopp.a ../../../../automaton/src/local_third_party/curl/lib/.libs/libcurl.a ../../../../automaton/src/local_third_party/g3log/build/libg3logger.a ../../../../automaton/src/local_third_party/gmp/.libs/libgmp.a ../../../../automaton/src/local_third_party/lua/liblua.a ../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf-lite.a ../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf-lite.a ../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf.a ../../../../automaton/src/local_third_party/replxx/build/libreplxx.a ../../../../automaton/src/local_third_party/zlib/libz.a -lrt -ldl -lpthread -lgmp $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCER_LINUX_MAKE_6D53C8B4=1 -DJUCE_APP_VERSION=0.0.1 -DJUCE_APP_VERSION_HEX=0x1 $(shell pkg-config --cflags x11 xinerama xext freetype2 libcurl) -pthread -I../../JuceLibraryCode -I../../../../automaton/src/local_third_party/JUCE/modules $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  JUCE_TARGET_APP := PlaygroundGUI

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -I ../../../../automaton/src -I ../../../../automaton/src/local_third_party/bitcoin/src -I ../../../../automaton/src/local_third_party/boost -I ../../../../automaton/src/local_third_party/openssl/include -I ../../../../automaton/src/local_third_party/cryptopp -I ../../../../automaton/src/local_third_party/curl/include -I ../../../../automaton/src/local_third_party/ed25519/src -I ../../../../automaton/src/local_third_party/g3log/build/include -I ../../../../automaton/src/local_third_party/g3log/src -I ../../../../automaton/src/local_third_party/gmp -I ../../../../automaton/src/local_third_party/googletest/googlemock/include -I ../../../../automaton/src/local_third_party/googletest/googletest/include -I ../../../../automaton/src/local_third_party/json -I ../../../../automaton/src/local_third_party/lua -I ../../../../automaton/src/local_third_party/protobuf/src -I ../../../../automaton/src/local_third_party/replxx/include -I ../../../../automaton/src/local_third_party/sol2/single/sol -I ../../../../automaton/src/local_third_party/zlib $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) $(shell pkg-config --libs x11 xinerama xext freetype2 libcurl) -fvisibility=hidden ../../../../automaton/src/build/libblockchain_cpp_node.a ../../../../automaton/src/build/libautomaton-core.a ../../../../automaton/src/build/libkoh-miner.a ../../../../automaton/src/local_third_party/bitcoin/src/secp256k1/.libs/libsecp256k1.a ../../../../automaton/src/local_third_party/boost/stage/lib/libboost_filesystem.a ../../../../automaton/src/local_third_party/boost/stage/lib/libboost_iostreams.a ../../../../automaton/src/local_third_party/boost/stage/lib/libboost_system.a ../../../../automaton/src/local_third_party/openssl/libssl.a ../../../../automaton/src/local_third_party/openssl/libcrypto.a ../../../../automaton/src/local_third_party/cryptopp/libcryptopp.a ../../../../automaton/src/local_third_party/curl/lib/.libs/libcurl.a ../../../../automaton/src/local_third_party/g3log/build/libg3logger.a ../../../../automaton/src/local_third_party/gmp/.libs/libgmp.a ../../../../automaton/src/local_third_party/lua/liblua.a ../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf-lite.a ../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf-lite.a ../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf.a ../../../../automaton/src/local_third_party/replxx/build/libreplxx.a ../../../../automaton/src/local_third_party/zlib/libz.a -lrt -ldl -lpthread -lgmp $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/FormMaker_de6ddb10.o \
  $(JUCE_OBJDIR)/AutomatonContractData_2dae4430.o \
  $(JUCE_OBJDIR)/DemoSimNet_f027b031.o \
  $(JUCE_OBJDIR)/DemoMiner_b95613be.o \
  $(JUCE_OBJDIR)/Miner_521421b0.o \
  $(JUCE_OBJDIR)/NetworkView_2d1f0d3.o \
  $(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(RESOURCES)
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors x11 xinerama xext freetype2 libcurl
	@echo Linking "PlaygroundGUI - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/FormMaker_de6ddb10.o: ../../Source/Components/FormMaker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FormMaker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomatonContractData_2dae4430.o: ../../Source/Data/AutomatonContractData.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomatonContractData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DemoSimNet_f027b031.o: ../../Source/Demos/DemoSimNet.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DemoSimNet.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DemoMiner_b95613be.o: ../../Source/Demos/DemoMiner.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DemoMiner.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Miner_521421b0.o: ../../Source/Miner/Miner.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Miner.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NetworkView_2d1f0d3.o: ../../Source/Network/NetworkView.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NetworkView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

clean:
	@echo Cleaning PlaygroundGUI
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping PlaygroundGUI
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)

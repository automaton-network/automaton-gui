# JUCE Configuration

Configuring JUCE in order to link to Automaton Core libraries is a bit of a challenge. This is not the best solution, but it is a solution.

Here are some of the configurations that we're modifying and plugging into the Jucer configuration.

## Mac OS X

### Extra Preprocessor Definitions

```
```

### Extra Compiler Flags

```
-I $(SRCROOT)/../../../../automaton/src
-I $(SRCROOT)/../../../../automaton/src/local_third_party/bitcoin/src
-I $(SRCROOT)/../../../../automaton/src/local_third_party/boost
-I $(SRCROOT)/../../../../automaton/src/local_third_party/openssl/include
-I $(SRCROOT)/../../../../automaton/src/local_third_party/cryptopp
-I $(SRCROOT)/../../../../automaton/src/local_third_party/curl/include
-I $(SRCROOT)/../../../../automaton/src/local_third_party/ed25519/src
-I $(SRCROOT)/../../../../automaton/src/local_third_party/g3log/build/include
-I $(SRCROOT)/../../../../automaton/src/local_third_party/g3log/src
-I $(SRCROOT)/../../../../automaton/src/local_third_party/gmp
-I $(SRCROOT)/../../../../automaton/src/local_third_party/googletest/googlemock/include
-I $(SRCROOT)/../../../../automaton/src/local_third_party/googletest/googletest/include
-I $(SRCROOT)/../../../../automaton/src/local_third_party/json
-I $(SRCROOT)/../../../../automaton/src/local_third_party/lua
-I $(SRCROOT)/../../../../automaton/src/local_third_party/protobuf/src
-I $(SRCROOT)/../../../../automaton/src/local_third_party/replxx/include
-I $(SRCROOT)/../../../../automaton/src/local_third_party/sol2/single/sol
-I $(SRCROOT)/../../../../automaton/src/local_third_party/zlib
```

### Extra Linker Flags

```
$(SRCROOT)/../../../../automaton/src/build/libautomaton-core.a
$(SRCROOT)/../../../../automaton/src/build/libblockchain_cpp_node.a
$(SRCROOT)/../../../../automaton/src/build/libkoh-miner.a
$(SRCROOT)/../../../../automaton/src/local_third_party/bitcoin/src/secp256k1/.libs/libsecp256k1.a
$(SRCROOT)/../../../../automaton/src/local_third_party/boost/stage/lib/libboost_filesystem.a
$(SRCROOT)/../../../../automaton/src/local_third_party/boost/stage/lib/libboost_iostreams.a
$(SRCROOT)/../../../../automaton/src/local_third_party/boost/stage/lib/libboost_system.a
$(SRCROOT)/../../../../automaton/src/local_third_party/openssl/libssl.a
$(SRCROOT)/../../../../automaton/src/local_third_party/openssl/libcrypto.a
$(SRCROOT)/../../../../automaton/src/local_third_party/cryptopp/libcryptopp.a
$(SRCROOT)/../../../../automaton/src/local_third_party/curl/lib/.libs/libcurl.a
$(SRCROOT)/../../../../automaton/src/local_third_party/g3log/build/libg3logger.a
$(SRCROOT)/../../../../automaton/src/local_third_party/gmp/.libs/libgmp.a
$(SRCROOT)/../../../../automaton/src/local_third_party/lua/liblua.a
$(SRCROOT)/../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf-lite.a
$(SRCROOT)/../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf-lite.a
$(SRCROOT)/../../../../automaton/src/local_third_party/protobuf/src/.libs/libprotobuf.a
$(SRCROOT)/../../../../automaton/src/local_third_party/replxx/build/libreplxx.a
$(SRCROOT)/../../../../automaton/src/local_third_party/zlib/libz.a
```

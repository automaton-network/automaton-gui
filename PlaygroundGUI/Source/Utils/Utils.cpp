/*
 * Automaton Playground
 * Copyright (c) 2019 The Automaton Authors.
 * Copyright (c) 2019 The automaton.network Authors.
 *
 * Automaton Playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Automaton Playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Automaton Playground.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Utils.h"

#include <secp256k1_recovery.h>
#include <secp256k1.h>
#include "automaton/core/io/io.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/crypto/cryptopp/Keccak_256_cryptopp.h"
#include <boost/multiprecision/cpp_int.hpp>
using json = nlohmann::json;

using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::crypto::cryptopp::Keccak_256_cryptopp;

const String Utils::numericalIntegerAllowed = "0123456789";
const String Utils::numericalFloatAllowed = "0123456789.";


std::string Utils::gen_ethereum_address(const std::string& privkey_hex) {
  auto private_key = hex2bin(privkey_hex);
  auto priv_key = (unsigned char *)private_key.c_str();
  Keccak_256_cryptopp hash;
  secp256k1_context* context = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
  secp256k1_pubkey* pubkey = new secp256k1_pubkey();

  if (!secp256k1_ec_pubkey_create(context, pubkey, priv_key)) {
    LOG(WARNING) << "Invalid priv_key " << bin2hex(std::string(reinterpret_cast<const char*>(priv_key), 32));
    delete pubkey;
    secp256k1_context_destroy(context);
    return "";
  }

  uint8_t pub_key_serialized[65];
  uint8_t eth_address[32];
  size_t outLen = 65;
  secp256k1_ec_pubkey_serialize(context, pub_key_serialized, &outLen, pubkey, SECP256K1_EC_UNCOMPRESSED);
  hash.calculate_digest(pub_key_serialized + 1, 64, eth_address);
  delete pubkey;
  secp256k1_context_destroy(context);

  return "0x" + bin2hex(std::string(reinterpret_cast<char*>(eth_address + 12), 20));
}

std::unique_ptr<Drawable> Utils::loadSVG(const String& xmlData) {
  auto svg = XmlDocument::parse(xmlData);
  return std::unique_ptr<Drawable>(Drawable::createFromSVG(*svg));
}

static std::map<CoinUnit, String> coinUnitsFactor{
    {CoinUnit::Gwei,  "1000000000"},
    {CoinUnit::ether, "1000000000000000000"},
    {CoinUnit::AUTO, "1000000000000000000"}
};

String Utils::fromWei(CoinUnit unitTo, const String& value) {
  BigInteger whole;
  whole.parseString(value, 10);
  BigInteger factor;
  factor.parseString(coinUnitsFactor[unitTo], 10);
  BigInteger remainder;
  whole.divideBy(factor, remainder);

  auto decimals = remainder.toString(10).paddedLeft('0', static_cast<int>(unitTo)).trimCharactersAtEnd("0");
  if (decimals.isEmpty())
    return whole.toString(10);

  return whole.toString(10) + "." + decimals;
}

String Utils::toWei(CoinUnit unitTo, const String& value) {
  StringArray tokens;
  tokens.addTokens(value, ".", "");
  if (tokens.size() > 2) {
    return "";  // More than one dot
  }

  String whole = tokens[0];
  String decimals;
  if (tokens.size() > 1) {
    decimals = tokens[1];
  }

  BigInteger factor;
  auto unitFactor = coinUnitsFactor[unitTo].dropLastCharacters(decimals.length());
  if (unitFactor.isEmpty())
    return "";  // Exceeds max precision

  factor.parseString(unitFactor, 10);

  BigInteger result;
  result.parseString(whole + decimals, 10);

  return (result * factor).toString(10);
}

String Utils::divideBigInt(const String& dividend, const String& divisor, uint64 precision) {
  BigInteger a;
  a.parseString(dividend, 10);
  BigInteger b;
  b.parseString(divisor, 10);

  BigInteger reminder;
  a.divideBy(b, reminder);

  String decimals;
  for (int i = 0; i < precision; ++i) {
    reminder *= 10;
    BigInteger reminder2;
    reminder.divideBy(b, reminder2);
    decimals += reminder.toString(10);
    reminder = reminder2;

    if (reminder2.isZero())
      break;
  }

  if (decimals.isEmpty())
    return a.toString(10);

  return a.toString(10) + "." + decimals;
}

bool Utils::isZeroTime(const Time& time) {
  return time.toMilliseconds() == 0;
}

String Utils::getNormalizedAddress(const String& address) {
  if (address.length() < 2 || (address.length() != 40 && address.length() != 42)) {
    DBG("Incorrect address size! Provided address of size " + address.length());
    jassertfalse;
    return String();
  }

  if (address.substring(0, 2) == "0x")
    return address.substring(2);
  return address;
}

#if AUTOMATON_JUCE_UNIT_TESTS
class UtilsTest : public UnitTest {
 public:
  UtilsTest() : UnitTest("Utils") {
  }

  void runTest() override {
    beginTest("FromWei converter");
    expect(Utils::fromWei(CoinUnit::ether, "123456789") == "0.000000000123456789");
    expect(Utils::fromWei(CoinUnit::ether, "12345678900008") == "0.000012345678900008");

    beginTest("ToWei converter");
    expect(Utils::toWei(CoinUnit::ether, "0.000000000123456789") == "123456789");
    expect(Utils::toWei(CoinUnit::ether, "0.000012345678900008") == "12345678900008");

    beginTest("DivideBigInt");
    expect(Utils::divideBigInt("17", "4", 10) == "4.25");
    expect(Utils::divideBigInt("1000", "6", 10) == "166.6666666666");
    expect(Utils::divideBigInt("123456789", "12345678900008", 10) == "0.0000099999");
    expect(Utils::divideBigInt("12345678900008", "123456789", 10) == "100000.0000000648");
  }
};

static UtilsTest test;
#endif

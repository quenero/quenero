#include "gtest/gtest.h"

#include "common/quenero.h"
#include "cryptonote_core/quenero_name_system.h"
#include "quenero_economy.h"

TEST(quenero_name_system, name_tests)
{
  struct name_test
  {
    std::string name;
    bool allowed;
  };


  name_test const session_wallet_names[] = {
      {"Hello", true},
      {"1Hello", true},
      {"1Hello1", true},
      {"_Hello1", true},
      {"1Hello_", true},
      {"_Hello_", true},
      {"999", true},
      {"xn--tda", true},
      {"xn--Mnchen-Ost-9db", true},

      {"-", false},
      {"@", false},
      {"'Hello", false},
      {"@Hello", false},
      {"[Hello", false},
      {"]Hello", false},
      {"Hello ", false},
      {" Hello", false},
      {" Hello ", false},

      {"Hello World", false},
      {"Hello\\ World", false},
      {"\"hello\"", false},
      {"hello\"", false},
      {"\"hello", false},
  };

  for (uint16_t type16 = 0; type16 < static_cast<uint16_t>(ons::mapping_type::_count); type16++)
  {
    auto type = static_cast<ons::mapping_type>(type16);
    if (type == ons::mapping_type::wallet) continue; // Not yet supported
    name_test const *names = session_wallet_names;
    size_t names_count     = quenero::char_count(session_wallet_names);

    for (size_t i = 0; i < names_count; i++)
    {
      name_test const &entry = names[i];
      ASSERT_EQ(ons::validate_ons_name(type, entry.name), entry.allowed) << "Values were {type=" << type << ", name=\"" << entry.name << "\"}";
    }
  }
}

TEST(quenero_name_system, value_encrypt_and_decrypt)
{
  std::string name         = "my ons name";
  ons::mapping_value value = {};
  value.encrypted          = false;
  value.len                = 32;
  memset(&value.buffer[0], 'a', value.len);

  // The type here is not hugely important for decryption except  (as opposed to
  // session) doesn't fall back to argon2 decryption if decryption fails.
  constexpr auto type = ons::mapping_type::session;

  // Encryption and Decryption success
  {
    auto mval = value;
    ASSERT_TRUE(mval.encrypt(name));
    ASSERT_FALSE(mval == value);
    ASSERT_TRUE(mval.decrypt(name, type));
    ASSERT_TRUE(mval == value);
  }

  // Decryption Fail: Encrypted value was modified
  {
    auto mval = value;
    ASSERT_FALSE(mval.encrypted);
    ASSERT_TRUE(mval.encrypt(name));
    ASSERT_TRUE(mval.encrypted);

    mval.buffer[0] = 'Z';
    ASSERT_FALSE(mval.decrypt(name, type));
    ASSERT_TRUE(mval.encrypted);
  }

  // Decryption Fail: Name was modified
  {
    std::string name_copy = name;
    auto mval = value;
    ASSERT_TRUE(mval.encrypt(name_copy));

    name_copy[0] = 'z';
    ASSERT_FALSE(mval.decrypt(name_copy, type));
  }
}

TEST(quenero_name_system, value_encrypt_and_decrypt_heavy)
{
  std::string name         = "abcdefg";
  ons::mapping_value value = {};
  value.encrypted          = false;
  value.len                = 33;
  memset(&value.buffer[0], 'a', value.len);

  // Encryption and Decryption success for the older argon2-based encryption key
  {
    auto mval = value;
    auto mval_new = value;
    ASSERT_TRUE(mval.encrypt(name, nullptr, true));
    ASSERT_TRUE(mval_new.encrypt(name, nullptr, false));
    ASSERT_EQ(mval.len + 24, mval_new.len); // New value appends a 24-byte nonce
    ASSERT_TRUE(mval.decrypt(name, ons::mapping_type::session));
    ASSERT_TRUE(mval_new.decrypt(name, ons::mapping_type::session));
    ASSERT_TRUE(mval == value);
    ASSERT_TRUE(mval_new == value);
  }
}

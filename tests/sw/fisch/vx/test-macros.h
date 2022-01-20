#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <gtest/gtest.h>

/**
 * General test for register with boolean value type.
 * @param Type Register type to test
 */
#define FISCH_TEST_BOOLEAN_REGISTER_GENERAL(Type)                                                  \
	TEST(Type, General)                                                                            \
	{                                                                                              \
		Type config;                                                                               \
		Type::Value const value = Type::Value(!config.get());                                      \
		config.set(value);                                                                         \
		EXPECT_EQ(config.get(), value);                                                            \
                                                                                                   \
		Type config2(value);                                                                       \
		EXPECT_EQ(config2.get(), value);                                                           \
                                                                                                   \
		Type other_config = config;                                                                \
                                                                                                   \
		EXPECT_EQ(other_config, config);                                                           \
		EXPECT_NE(Type(), config);                                                                 \
	}

/**
 * Ostream test for register with boolean value type.
 * @param Type Register type to test
 * @param Name Name to expect in string representation
 */
#define FISCH_TEST_BOOLEAN_REGISTER_OSTREAM(Type, Name)                                            \
	TEST(Type, Ostream)                                                                            \
	{                                                                                              \
		Type obj;                                                                                  \
		obj.set(Type::Value(true));                                                                \
                                                                                                   \
		std::stringstream stream;                                                                  \
		stream << obj;                                                                             \
                                                                                                   \
		EXPECT_EQ(stream.str(), #Name "(true)");                                                   \
	}

/**
 * Serialization test for register with boolean value type.
 * @param Type Register type to test
 */
#define FISCH_TEST_BOOLEAN_REGISTER_CEREAL(Type)                                                   \
	TEST(Type, CerializeCoverage)                                                                  \
	{                                                                                              \
		Type obj1, obj2;                                                                           \
		obj1.set(Type::Value(!obj1.get()));                                                        \
                                                                                                   \
		std::ostringstream ostream;                                                                \
		{                                                                                          \
			cereal::JSONOutputArchive oa(ostream);                                                 \
			oa(obj1);                                                                              \
		}                                                                                          \
                                                                                                   \
		std::istringstream istream(ostream.str());                                                 \
		{                                                                                          \
			cereal::JSONInputArchive ia(istream);                                                  \
			ia(obj2);                                                                              \
		}                                                                                          \
		ASSERT_EQ(obj1, obj2);                                                                     \
	}

/**
 * General test for register with ranged integer value type.
 * @param Type Register type to test
 * @param ValueDefault Default value to expect
 * @param ValueOutOfRange Out-of-range value to expect std::out_of_range throw for
 * @param ValueTest Test value to use in accessor test
 */
#define FISCH_TEST_RANGED_REGISTER_GENERAL(Type, ValueDefault, ValueOutOfRange, ValueTest)         \
	TEST(Type, General)                                                                            \
	{                                                                                              \
		EXPECT_NO_THROW(Type::Value());                                                            \
		EXPECT_EQ(Type::Value().value(), ValueDefault);                                            \
		EXPECT_THROW(Type::Value(ValueOutOfRange), std::overflow_error);                           \
                                                                                                   \
		Type config;                                                                               \
		EXPECT_EQ(config.get(), Type::Value());                                                    \
		auto const value = Type::Value(ValueTest);                                                 \
		config.set(value);                                                                         \
		EXPECT_EQ(config.get(), value);                                                            \
                                                                                                   \
		Type config2(value);                                                                       \
		EXPECT_EQ(config2.get(), value);                                                           \
                                                                                                   \
		Type other_config = config;                                                                \
                                                                                                   \
		EXPECT_EQ(other_config, config);                                                           \
		EXPECT_NE(Type(), config);                                                                 \
	}

/**
 * General test for register with integer value type.
 * This type is used instead of ranged integer value type for performance reasons, e.g. for Omnibus.
 * @param Type Register type to test
 * @param ValueDefault Default value to expect
 * @param ValueTest Test value to use in accessor test
 */
#define FISCH_TEST_NUMBER_REGISTER_GENERAL(Type, ValueDefault, ValueTest)                          \
	TEST(Type, General)                                                                            \
	{                                                                                              \
		EXPECT_EQ(Type::Value().value(), ValueDefault);                                            \
		Type config;                                                                               \
		EXPECT_EQ(config.get(), Type::Value());                                                    \
		auto const value = Type::Value(ValueTest);                                                 \
		config.set(value);                                                                         \
		EXPECT_EQ(config.get(), value);                                                            \
                                                                                                   \
		Type config2(value);                                                                       \
		EXPECT_EQ(config2.get(), value);                                                           \
                                                                                                   \
		Type other_config = config;                                                                \
                                                                                                   \
		EXPECT_EQ(other_config, config);                                                           \
		EXPECT_NE(Type(), config);                                                                 \
	}

/**
 * Serialization test for register with ranged or unranged integer value type.
 * @param Type Register type to test
 * @param Val Value to use to check (de-)serialization
 */
#define FISCH_TEST_NUMBER_REGISTER_CEREAL(Type, Val)                                               \
	TEST(Type, CerializeCoverage)                                                                  \
	{                                                                                              \
		Type obj1, obj2;                                                                           \
		obj1.set(Type::Value(Val));                                                                \
                                                                                                   \
		std::ostringstream ostream;                                                                \
		{                                                                                          \
			cereal::JSONOutputArchive oa(ostream);                                                 \
			oa(obj1);                                                                              \
		}                                                                                          \
                                                                                                   \
		std::istringstream istream(ostream.str());                                                 \
		{                                                                                          \
			cereal::JSONInputArchive ia(istream);                                                  \
			ia(obj2);                                                                              \
		}                                                                                          \
		ASSERT_EQ(obj1, obj2);                                                                     \
	}

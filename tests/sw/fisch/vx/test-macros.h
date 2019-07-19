#pragma once
#include <gtest/gtest.h>

#define FISCH_TEST_BOOLEAN_REGISTER_GENERAL(Type)                                                  \
	TEST(Type, General)                                                                            \
	{                                                                                              \
		Type config;                                                                               \
		bool const value = !config.get();                                                          \
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

#define FISCH_TEST_BOOLEAN_REGISTER_OSTREAM(Type, Name)                                            \
	TEST(Type, Ostream)                                                                            \
	{                                                                                              \
		Type obj;                                                                                  \
		obj.set(true);                                                                             \
                                                                                                   \
		std::stringstream stream;                                                                  \
		stream << obj;                                                                             \
                                                                                                   \
		EXPECT_EQ(stream.str(), #Name "(true)");                                                   \
	}

#define FISCH_TEST_BOOLEAN_REGISTER_CEREAL(Type)                                                   \
	TEST(Type, CerializeCoverage)                                                                  \
	{                                                                                              \
		Type obj1, obj2;                                                                           \
		obj1.set(!obj1.get());                                                                     \
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

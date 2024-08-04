#include <vector>
#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

class KokoTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown() {}
};


TEST_F(KokoTest, Apa)
{
    ASSERT_EQ(true, false);
}
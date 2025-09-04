#include "gtest/gtest.h"
#include "../src/utils.h"

TEST(Utils, SetVideoResolution_FirstOption)
{
    int width = 0;
    int height = 0;

    utils::setVideoResolution("0", width, height);
    EXPECT_EQ(width, 352);
    EXPECT_EQ(height, 288);
}

TEST(Utils, SetVideoResolution_InvalidOption)
{
    int width = 0;
    int height = 0;

    EXPECT_FALSE(utils::setVideoResolution("a", width, height));
}

TEST(Utils, SetVideoResolution_NegativeOption)
{
    int width = 0;
    int height = 0;

    EXPECT_FALSE(utils::setVideoResolution("-1", width, height));
}

TEST(Utils, SetVideoResolution_AboveBoundsOption)
{
    int width = 0;
    int height = 0;

    EXPECT_FALSE(utils::setVideoResolution("999999", width, height));
}

TEST(Utils, SetOffset_Center)
{
    int offsetX = 0; int offsetY = 0;
    bool placeInCenter = false;

    char arg1[] = "center";
    char arg2[] = "20";
    bool result = utils::setOffset(arg1, arg2,
        offsetX, offsetY, placeInCenter);

    EXPECT_TRUE(result);
    EXPECT_EQ(offsetX, 0);
    EXPECT_EQ(offsetY, 0);
    EXPECT_EQ(placeInCenter, true);
}

TEST(Utils, SetOffset_ValidNumbers)
{
    int offsetX = 0, offsetY = 0;
    bool placeInCenter = false;

    char arg1[] = "10";
    char arg2[] = "20";
    bool result = utils::setOffset(arg1, arg2, offsetX, offsetY, placeInCenter);

    EXPECT_TRUE(result);
    EXPECT_FALSE(placeInCenter);
    EXPECT_EQ(offsetX, 10);
    EXPECT_EQ(offsetY, 20);
}

TEST(Utils, SetOffset_InvalidArgument)
{
    int offsetX = 123, offsetY = 456;
    bool placeInCenter = false;

    char arg1[] = "abc";
    char arg2[] = "20";
    bool result = utils::setOffset(arg1, arg2, offsetX, offsetY, placeInCenter);

    EXPECT_FALSE(result);
}

TEST(Utils, SetOffset_OutOfRange)
{
    int offsetX = 0, offsetY = 0;
    bool placeInCenter = false;

    char arg1[] = "999999999999999999999";
    char arg2[] = "20";
    bool result = utils::setOffset(arg1, arg2, offsetX, offsetY, placeInCenter);

    EXPECT_FALSE(result);
}

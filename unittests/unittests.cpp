// unittests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "gtest/gtest.h"

int _tmain(int argc, _TCHAR* argv[])
{
   ::testing::InitGoogleTest(&argc, argv);
   const int result = RUN_ALL_TESTS();
   getchar();
   return result;
}

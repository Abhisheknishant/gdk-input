// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/gamepad.h>

using namespace gdk;

TEST_CASE("mouse constructors", "[mouse]")
{
    initGLFW();

    SECTION("this is not a test, this is rock and roll")
    {
        REQUIRE(0 == 0);
    }
}


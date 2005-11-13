#include "common.h"

class GfxOGLTest : public CxxTest::TestSuite, public GfxTest
{

public:

    void setUp() { oglInit(); }

    void tearDown() { libFree(); }

    void testntWindowAndNTRenderWindow() { ntWindowAndNTRenderWindow(); }

    void testExternalWindow() { externalWindow(); }

    void testChangeUserOptions() { changeOptions(); }

    void testFullscreen() { fullscreen(); }

    void testDefaultBackbufferSize() { defaultBackbufferSize(); }

    void testMultiRenderer() { multiRenderer(); }
};

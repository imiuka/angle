//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "libANGLE/angletypes.h"
#include "libANGLE/AttributeMap.h"
#include "libANGLE/Config.h"
#include "libANGLE/ContextState.h"
#include "libANGLE/State.h"
#include "libANGLE/Surface.h"
#include "libANGLE/renderer/FramebufferImpl_mock.h"
#include "libANGLE/renderer/SurfaceImpl.h"
#include "tests/angle_unittests_utils.h"

using namespace rx;
using namespace testing;

namespace
{

class MockSurfaceImpl : public rx::SurfaceImpl
{
  public:
    MockSurfaceImpl() : SurfaceImpl(mockState), mockState(nullptr) {}
    virtual ~MockSurfaceImpl() { destructor(); }

    MOCK_METHOD1(destroy, void(const egl::Display *));
    MOCK_METHOD1(initialize, egl::Error(const egl::Display *));
    MOCK_METHOD1(createDefaultFramebuffer, rx::FramebufferImpl *(const gl::FramebufferState &data));
    MOCK_METHOD1(swap, egl::Error(const egl::Display *));
    MOCK_METHOD2(swapWithDamage, egl::Error(EGLint *, EGLint));
    MOCK_METHOD4(postSubBuffer, egl::Error(EGLint, EGLint, EGLint, EGLint));
    MOCK_METHOD2(querySurfacePointerANGLE, egl::Error(EGLint, void**));
    MOCK_METHOD2(bindTexImage, egl::Error(gl::Texture*, EGLint));
    MOCK_METHOD1(releaseTexImage, egl::Error(EGLint));
    MOCK_METHOD3(getSyncValues, egl::Error(EGLuint64KHR *, EGLuint64KHR *, EGLuint64KHR *));
    MOCK_METHOD1(setSwapInterval, void(EGLint));
    MOCK_CONST_METHOD0(getWidth, EGLint());
    MOCK_CONST_METHOD0(getHeight, EGLint());
    MOCK_CONST_METHOD0(isPostSubBufferSupported, EGLint(void));
    MOCK_CONST_METHOD0(getSwapBehavior, EGLint(void));
    MOCK_METHOD3(getAttachmentRenderTarget,
                 gl::Error(GLenum,
                           const gl::ImageIndex &,
                           rx::FramebufferAttachmentRenderTarget **));

    MOCK_METHOD0(destructor, void());

    egl::SurfaceState mockState;
};

TEST(SurfaceTest, DestructionDeletesImpl)
{
    NiceMock<MockEGLFactory> factory;

    MockSurfaceImpl *impl = new MockSurfaceImpl;
    EXPECT_CALL(factory, createWindowSurface(_, _, _)).WillOnce(Return(impl));

    egl::Config config;
    egl::Surface *surface = new egl::WindowSurface(
        &factory, &config, static_cast<EGLNativeWindowType>(0), egl::AttributeMap());

    EXPECT_CALL(*impl, destroy(_)).Times(1).RetiresOnSaturation();
    EXPECT_CALL(*impl, destructor()).Times(1).RetiresOnSaturation();

    surface->onDestroy(nullptr);

    // Only needed because the mock is leaked if bugs are present,
    // which logs an error, but does not cause the test to fail.
    // Ordinarily mocks are verified when destroyed.
    Mock::VerifyAndClear(impl);
}

} // namespace

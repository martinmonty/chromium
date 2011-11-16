// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GL_GL_CONTEXT_STUB_H_
#define UI_GFX_GL_GL_CONTEXT_STUB_H_
#pragma once

#include "ui/gfx/gl/gl_context.h"

namespace gfx {

// A GLContext that does nothing for unit tests.
class GL_EXPORT GLContextStub : public GLContext {
 public:
  GLContextStub();
  virtual ~GLContextStub();

  // Implement GLContext.
  virtual bool Initialize(
      GLSurface* compatible_surface, GpuPreference gpu_preference) OVERRIDE;
  virtual void Destroy() OVERRIDE;
  virtual bool MakeCurrent(GLSurface* surface) OVERRIDE;
  virtual void ReleaseCurrent(GLSurface* surface) OVERRIDE;
  virtual bool IsCurrent(GLSurface* surface) OVERRIDE;
  virtual void* GetHandle() OVERRIDE;
  virtual void SetSwapInterval(int interval) OVERRIDE;
  virtual std::string GetExtensions() OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(GLContextStub);
};

}  // namespace gfx

#endif  // UI_GFX_GL_GL_CONTEXT_STUB_H_

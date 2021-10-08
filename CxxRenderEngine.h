#pragma once

#include "RenderEngine.h"

#include <memory>

class CxxRenderEngine : public RenderEngine
{
public:
  static auto create() -> std::unique_ptr<CxxRenderEngine>;
};

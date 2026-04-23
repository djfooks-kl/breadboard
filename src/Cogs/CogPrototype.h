#pragma once

#include <glm/fwd.hpp>

#include "CogResourceId.h"
#include "Core/ITransform.h"

namespace xg
{
    struct IRenderableAdder;

    struct CogPrototype
    {
        virtual ~CogPrototype();

        virtual xg::CogResourceId GetResourceId() const = 0;

        virtual glm::ivec2 GetSize() const = 0;

        virtual const std::vector<glm::ivec2>& GetWireNodes() const;

        virtual void AddStaticRenderables(
            const xc::ITransform& transform,
            const xg::IRenderableAdder& renderableAdder) const;
    };
}

#define DECLARE_COG_FUNCTIONS() \
    xg::CogResourceId GetResourceId() const override;                       \
    glm::ivec2 GetSize() const override;                                    \
    const std::vector<glm::ivec2>& GetWireNodes() const override;           \
    void AddStaticRenderables(                                              \
        const xc::ITransform& transform,                                    \
        const xg::IRenderableAdder& renderableAdder) const override;

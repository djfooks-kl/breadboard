#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "GridAttachmentSystem.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::GridAttachmentSystem - " description, "[xg::GridAttachmentSystem]")

namespace
{
    static const xg::CogResourceId s_Size1Cog = xg::CogResourceId::Create("Size1Cog");
    static const xg::CogResourceId s_LongCog = xg::CogResourceId::Create("LongCog");
    static const std::unordered_set<glm::ivec2> s_LongCoreWireNodes{ glm::ivec2(0, 0), glm::ivec2(1, 0), glm::ivec2(0, 2) };

    struct Size1Cog final : public xg::CogPrototype
    {
        xg::CogResourceId GetResourceId() const override { return s_Size1Cog; }

        glm::ivec2 GetSize() const override { return glm::ivec2(1, 1); }
    };

    struct LongCog final : public xg::CogPrototype
    {
        xg::CogResourceId GetResourceId() const override { return s_LongCog; }

        virtual const std::unordered_set<glm::ivec2>& GetWireNodes() const { return s_LongCoreWireNodes; }

        glm::ivec2 GetSize() const override { return glm::ivec2(2, 3); }
    };

    bool CellOnlyHas(const xg::GridAttachmentsMap& map, const glm::ivec2& p, const flecs::entity entity)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return false;

        const auto& list = itr->second;
        if (list.m_Entities.size() != 1)
            return false;

        return list.m_Entities[0] == entity;
    }

    bool CellHasFlags(const xg::GridAttachmentsMap& map, const glm::ivec2& p, const xg::GridAttachments& flags)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return false;

        const auto& attachments = itr->second;
        bool result = true;
        result = result && flags.m_HasNode == attachments.m_HasNode;

        return result;
    }

    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::GridAttachmentsComponent>();
            m_World.ensure<xg::OnStageAddedComponent>();
            m_World.ensure<xg::OnStageRemovedComponent>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<Size1Cog>();
            cogMap.Register<LongCog>();
        }

        void Update()
        {
            xg::GridAttachmentSystem::Update(m_World);

            m_World.get_mut<xg::OnStageAddedComponent>().m_Entities.clear();
            m_World.get_mut<xg::OnStageRemovedComponent>().m_Entities.clear();
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Adding on stage to a cog -> Attach it to the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity);

    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 1);
    auto itr = map.find(glm::ivec2(3, 2));
    REQUIRE(itr != map.end());
    const auto& list = itr->second;
    REQUIRE(list.m_Entities.size() == 1);
    CHECK(list.m_Entities[0] == entity);
}

SYSTEM_TEST_CASE("Removing on stage on a cog -> Deattach it from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity);
    env.Update();

    world.get_mut<xg::OnStageRemovedComponent>().m_Entities.push_back(entity);
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 0);
}

SYSTEM_TEST_CASE("Removing on stage on 1 cog out of 2 -> Deattach only removed cog from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity1 = world.entity();
    {
        auto& cog = entity1.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    flecs::entity entity2 = world.entity();
    {
        auto& cog = entity2.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(13, 12) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity1);
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity2);
    env.Update();

    world.get_mut<xg::OnStageRemovedComponent>().m_Entities.push_back(entity2);
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 1);
    CHECK(CellOnlyHas(map, glm::ivec2(3, 2), entity1));
}

SYSTEM_TEST_CASE("Adding on stage to a long cog -> Attach it to the grid on every cell it occupies and mark the nodes with HasNode")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity);

    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 6);

    CHECK(CellOnlyHas(map, glm::ivec2(3, 2), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 2), entity));

    CHECK(CellOnlyHas(map, glm::ivec2(3, 3), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 3), entity));

    CHECK(CellOnlyHas(map, glm::ivec2(3, 4), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 4), entity));

    CHECK(CellHasFlags(map, glm::ivec2(3, 2), { .m_HasNode = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 2), { .m_HasNode = true }));

    CHECK(CellHasFlags(map, glm::ivec2(3, 3), {}));
    CHECK(CellHasFlags(map, glm::ivec2(4, 3), {}));

    CHECK(CellHasFlags(map, glm::ivec2(3, 4), {}));
    CHECK(CellHasFlags(map, glm::ivec2(4, 4), { .m_HasNode = true }));
}

SYSTEM_TEST_CASE("Removing on stage on a long cog -> Deattach it from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity);
    env.Update();

    world.get_mut<xg::OnStageRemovedComponent>().m_Entities.push_back(entity);
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 0);
}

SYSTEM_TEST_CASE("Adding on stage to a rotated long cog -> Attach it to the grid on every cell it occupies")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2), xc::Rotation90(1) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity);

    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 6);

    CHECK(CellOnlyHas(map, glm::ivec2(3, 2), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 2), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(5, 2), entity));

    CHECK(CellOnlyHas(map, glm::ivec2(3, 1), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 1), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(5, 1), entity));
}

SYSTEM_TEST_CASE("Removing on stage on a rotated long cog -> Deattach it from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2), xc::Rotation90(1) };
    }
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(entity);
    env.Update();

    world.get_mut<xg::OnStageRemovedComponent>().m_Entities.push_back(entity);
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 0);
}
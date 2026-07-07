#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/Range.h"
#include "GridAttachmentsComponent.h"
#include "GridAttachmentSystem.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"
#include "WireComponent.h"
#include "WireFlags.h"

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

    int GetCellEntityCount(const xg::GridAttachmentsMap& map, const glm::ivec2& p)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return 0;

        const auto& list = itr->second;
        return static_cast<int>(list.m_Entities.size());
    }

    bool CellContains(const xg::GridAttachmentsMap& map, const glm::ivec2& p, const flecs::entity entity)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return false;

        const auto& list = itr->second;
        return xc::RangeContains(list.m_Entities, entity);
    }

    bool CellHasFlags(const xg::GridAttachmentsMap& map, const glm::ivec2& p, const xg::GridAttachments& flags)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return false;

        const auto& attachments = itr->second;
        bool result = true;
        result = result && flags.m_HasNode == attachments.m_HasNode;
        result = result && flags.m_HasWireDot == attachments.m_HasWireDot;

        return result;
    }

    xg::TWireDirectionFlags GetCellWireDirection(const xg::GridAttachmentsMap& map, const glm::ivec2& p)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return xg::TWireDirectionFlags{};

        const auto& attachments = itr->second;
        return attachments.m_WireDirectionFlags;
    }

    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::GridAttachmentsComponent>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<Size1Cog>();
            cogMap.Register<LongCog>();
        }

        void Update()
        {
            xg::GridAttachmentSystem::Update(m_World);

            m_World.defer_begin();
            m_World.each([](flecs::entity entity, xg::OnStageAddedComponent)
                {
                    entity.remove<xg::OnStageAddedComponent>();
                });
            m_World.each([](flecs::entity entity, xg::OnStageRemovedComponent)
                {
                    entity.remove<xg::OnStageRemovedComponent>();
                });
            m_World.defer_end();
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
        entity.add<xg::OnStageAddedComponent>();
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }

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
        entity.add<xg::OnStageAddedComponent>();
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    env.Update();

    entity.add<xg::OnStageRemovedComponent>();
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
        entity1.add<xg::OnStageAddedComponent>();
        auto& cog = entity1.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    flecs::entity entity2 = world.entity();
    {
        entity2.add<xg::OnStageAddedComponent>();
        auto& cog = entity2.ensure<xg::CogComponent>();
        cog.m_CogId = s_Size1Cog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(13, 12) };
    }
    env.Update();

    entity2.add<xg::OnStageRemovedComponent>();
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
        entity.add<xg::OnStageAddedComponent>();
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }

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

    CHECK(CellHasFlags(map, glm::ivec2(3, 4), { .m_HasNode = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 4), {}));
}

SYSTEM_TEST_CASE("Removing on stage on a long cog -> Deattach it from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        entity.add<xg::OnStageAddedComponent>();
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2) };
    }
    env.Update();

    entity.add<xg::OnStageRemovedComponent>();
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
        entity.add<xg::OnStageAddedComponent>();
        auto& cog = entity.ensure<xg::CogComponent>();
        // box from 3,2 to 5,1 with nodes at [3,2], [3,1], [5,2]
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2), xc::Rotation90(1) };
    }

    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 6);

    CHECK(CellOnlyHas(map, glm::ivec2(3, 2), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 2), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(5, 2), entity));

    CHECK(CellOnlyHas(map, glm::ivec2(3, 1), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 1), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(5, 1), entity));

    CHECK(CellHasFlags(map, glm::ivec2(3, 2), { .m_HasNode = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 2), {}));
    CHECK(CellHasFlags(map, glm::ivec2(5, 2), { .m_HasNode = true }));

    CHECK(CellHasFlags(map, glm::ivec2(3, 1), { .m_HasNode = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 1), {}));
    CHECK(CellHasFlags(map, glm::ivec2(5, 1), {}));
}

SYSTEM_TEST_CASE("Removing on stage on a rotated long cog -> Deattach it from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        entity.add<xg::OnStageAddedComponent>();
        auto& cog = entity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2), xc::Rotation90(1) };
    }
    env.Update();

    entity.add<xg::OnStageRemovedComponent>();
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 0);
}

SYSTEM_TEST_CASE("Adding on stage to a wire -> Attach it to the grid and set the node and wire direction flags")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        entity.add<xg::OnStageAddedComponent>();
        auto& wire = entity.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(20, 40));
        wire.m_Checkpoints.push_back(glm::ivec2(20, 42)); // North
        wire.m_Checkpoints.push_back(glm::ivec2(22, 44)); // North east
        wire.m_Checkpoints.push_back(glm::ivec2(24, 44)); // East
        wire.m_Checkpoints.push_back(glm::ivec2(26, 42)); // South east
        wire.m_Checkpoints.push_back(glm::ivec2(26, 40)); // South
        wire.m_Checkpoints.push_back(glm::ivec2(24, 38)); // South west
        wire.m_Checkpoints.push_back(glm::ivec2(22, 38)); // West
        wire.m_Checkpoints.push_back(glm::ivec2(20, 40)); // North West
    }

    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 16);
    CHECK(CellOnlyHas(map, glm::ivec2(20, 40), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(20, 41), entity)); // North
    CHECK(CellOnlyHas(map, glm::ivec2(20, 42), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(21, 43), entity)); // North east
    CHECK(CellOnlyHas(map, glm::ivec2(22, 44), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(23, 44), entity)); // East
    CHECK(CellOnlyHas(map, glm::ivec2(24, 44), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(25, 43), entity)); // South east
    CHECK(CellOnlyHas(map, glm::ivec2(26, 42), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(26, 41), entity)); // South
    CHECK(CellOnlyHas(map, glm::ivec2(26, 40), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(25, 39), entity)); // South west
    CHECK(CellOnlyHas(map, glm::ivec2(24, 38), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(23, 38), entity)); // West
    CHECK(CellOnlyHas(map, glm::ivec2(22, 38), entity));
    CHECK(CellOnlyHas(map, glm::ivec2(21, 39), entity)); // North West

    using TFlags = xg::TWireDirectionFlags;
    using EWireDir = xg::EWireDirection;

    CHECK(GetCellWireDirection(map, glm::ivec2(20, 40)) == (TFlags{} | EWireDir::N | EWireDir::SE));
    CHECK(GetCellWireDirection(map, glm::ivec2(20, 41)) == (TFlags{} | EWireDir::N | EWireDir::S));
    CHECK(GetCellWireDirection(map, glm::ivec2(20, 42)) == (TFlags{} | EWireDir::S | EWireDir::NE));
    CHECK(GetCellWireDirection(map, glm::ivec2(21, 43)) == (TFlags{} | EWireDir::NE | EWireDir::SW));
    CHECK(GetCellWireDirection(map, glm::ivec2(22, 44)) == (TFlags{} | EWireDir::SW | EWireDir::E));
    CHECK(GetCellWireDirection(map, glm::ivec2(23, 44)) == (TFlags{} | EWireDir::E | EWireDir::W));
    CHECK(GetCellWireDirection(map, glm::ivec2(24, 44)) == (TFlags{} | EWireDir::W | EWireDir::SE));
    CHECK(GetCellWireDirection(map, glm::ivec2(25, 43)) == (TFlags{} | EWireDir::SE | EWireDir::NW));
    CHECK(GetCellWireDirection(map, glm::ivec2(26, 42)) == (TFlags{} | EWireDir::NW | EWireDir::S));
    CHECK(GetCellWireDirection(map, glm::ivec2(26, 41)) == (TFlags{} | EWireDir::S | EWireDir::N));
    CHECK(GetCellWireDirection(map, glm::ivec2(26, 40)) == (TFlags{} | EWireDir::N | EWireDir::SW));
    CHECK(GetCellWireDirection(map, glm::ivec2(25, 39)) == (TFlags{} | EWireDir::SW | EWireDir::NE));
    CHECK(GetCellWireDirection(map, glm::ivec2(24, 38)) == (TFlags{} | EWireDir::NE | EWireDir::W));
    CHECK(GetCellWireDirection(map, glm::ivec2(23, 38)) == (TFlags{} | EWireDir::W | EWireDir::E));
    CHECK(GetCellWireDirection(map, glm::ivec2(22, 38)) == (TFlags{} | EWireDir::E | EWireDir::NW));
    CHECK(GetCellWireDirection(map, glm::ivec2(21, 39)) == (TFlags{} | EWireDir::NW | EWireDir::SE));

    CHECK(CellHasFlags(map, glm::ivec2(20, 40), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(20, 41), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(20, 42), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(21, 43), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(22, 44), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(23, 44), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(24, 44), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(25, 43), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(26, 42), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(26, 41), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(26, 40), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(25, 39), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(24, 38), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(23, 38), { .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(22, 38), { .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(21, 39), { .m_HasWireDot = false }));
}

SYSTEM_TEST_CASE("Removing on stage on a wire -> Deattach it from the grid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        entity.add<xg::OnStageAddedComponent>();
        auto& wire = entity.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(20, 40));
        wire.m_Checkpoints.push_back(glm::ivec2(20, 42));
        wire.m_Checkpoints.push_back(glm::ivec2(22, 44));
    }

    env.Update();

    entity.add<xg::OnStageRemovedComponent>();
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 0);
}

SYSTEM_TEST_CASE("Add cog and wire overlapping, then remove cog from stage -> Deattach only the cog")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wireEntity = world.entity();
    {
        wireEntity.add<xg::OnStageAddedComponent>();
        auto& wire = wireEntity.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(3, 2));
        wire.m_Checkpoints.push_back(glm::ivec2(3, 5));
    }

    flecs::entity cogEntity = world.entity();
    {
        cogEntity.add<xg::OnStageAddedComponent>();
        auto& cog = cogEntity.ensure<xg::CogComponent>();
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2), xc::Rotation90(1) };
    }

    env.Update();

    cogEntity.add<xg::OnStageRemovedComponent>();
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 4);
    CHECK(CellOnlyHas(map, glm::ivec2(3, 2), wireEntity));
    CHECK(CellOnlyHas(map, glm::ivec2(3, 3), wireEntity));
    CHECK(CellOnlyHas(map, glm::ivec2(3, 4), wireEntity));
    CHECK(CellOnlyHas(map, glm::ivec2(3, 5), wireEntity));

    CHECK(CellHasFlags(map, glm::ivec2(3, 2), { .m_HasNode = false, .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(3, 3), { .m_HasNode = false, .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(3, 4), { .m_HasNode = false, .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(3, 5), { .m_HasNode = false, .m_HasWireDot = true }));

    using TFlags = xg::TWireDirectionFlags;
    using EWireDir = xg::EWireDirection;

    CHECK(GetCellWireDirection(map, glm::ivec2(3, 2)) == (TFlags{} | EWireDir::N));
    CHECK(GetCellWireDirection(map, glm::ivec2(3, 3)) == (TFlags{} | EWireDir::N | EWireDir::S));
    CHECK(GetCellWireDirection(map, glm::ivec2(3, 4)) == (TFlags{} | EWireDir::N | EWireDir::S));
    CHECK(GetCellWireDirection(map, glm::ivec2(3, 5)) == (TFlags{} | EWireDir::S));
}

SYSTEM_TEST_CASE("Add wire A,B overlapping, then remove A from stage -> Leave B attached")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wireEntityA = world.entity();
    {
        wireEntityA.add<xg::OnStageAddedComponent>();
        auto& wire = wireEntityA.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(3, 2));
        wire.m_Checkpoints.push_back(glm::ivec2(3, 5));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 7));
    }

    flecs::entity wireEntityB = world.entity();
    {
        wireEntityB.add<xg::OnStageAddedComponent>();
        auto& wire = wireEntityB.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(3, 2));
        wire.m_Checkpoints.push_back(glm::ivec2(3, 5));
        wire.m_Checkpoints.push_back(glm::ivec2(5, 7));
    }

    env.Update();

    wireEntityA.add<xg::OnStageRemovedComponent>();
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 6);
    CHECK(CellOnlyHas(map, glm::ivec2(3, 2), wireEntityB));
    CHECK(CellOnlyHas(map, glm::ivec2(3, 3), wireEntityB));
    CHECK(CellOnlyHas(map, glm::ivec2(3, 4), wireEntityB));
    CHECK(CellOnlyHas(map, glm::ivec2(3, 5), wireEntityB));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 6), wireEntityB));
    CHECK(CellOnlyHas(map, glm::ivec2(5, 7), wireEntityB));

    CHECK(CellHasFlags(map, glm::ivec2(3, 2), { .m_HasNode = false, .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(3, 3), { .m_HasNode = false, .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(3, 4), { .m_HasNode = false, .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(3, 5), { .m_HasNode = false, .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 6), { .m_HasNode = false, .m_HasWireDot = false }));
    CHECK(CellHasFlags(map, glm::ivec2(5, 7), { .m_HasNode = false, .m_HasWireDot = true }));

    using TFlags = xg::TWireDirectionFlags;
    using EWireDir = xg::EWireDirection;

    CHECK(GetCellWireDirection(map, glm::ivec2(3, 2)) == (TFlags{} | EWireDir::N));
    CHECK(GetCellWireDirection(map, glm::ivec2(3, 3)) == (TFlags{} | EWireDir::N | EWireDir::S));
    CHECK(GetCellWireDirection(map, glm::ivec2(3, 4)) == (TFlags{} | EWireDir::N | EWireDir::S));
    CHECK(GetCellWireDirection(map, glm::ivec2(3, 5)) == (TFlags{} | EWireDir::S | EWireDir::NE));
    CHECK(GetCellWireDirection(map, glm::ivec2(4, 6)) == (TFlags{} | EWireDir::NE | EWireDir::SW));
    CHECK(GetCellWireDirection(map, glm::ivec2(5, 7)) == (TFlags{} | EWireDir::SW));
}

SYSTEM_TEST_CASE("Add wire A,B,C and a cog overlapping, then remove B from stage -> Leave A,C and cog attached")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wireEntityA = world.entity();
    {
        wireEntityA.add<xg::OnStageAddedComponent>();
        auto& wire = wireEntityA.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(3, 2));
        wire.m_Checkpoints.push_back(glm::ivec2(5, 2));
    }

    flecs::entity wireEntityB = world.entity();
    {
        wireEntityB.add<xg::OnStageAddedComponent>();
        auto& wire = wireEntityB.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(4, 2));
        wire.m_Checkpoints.push_back(glm::ivec2(7, 2));
    }

    flecs::entity wireEntityC = world.entity();
    {
        wireEntityC.add<xg::OnStageAddedComponent>();
        auto& wire = wireEntityC.ensure<xg::WireComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(5, 3));
        wire.m_Checkpoints.push_back(glm::ivec2(5, 1));
    }

    flecs::entity cogEntity = world.entity();
    {
        cogEntity.add<xg::OnStageAddedComponent>();
        auto& cog = cogEntity.ensure<xg::CogComponent>();
        // box from 3,2 to 5,1 with nodes at [3,2], [3,1], [5,2]
        cog.m_CogId = s_LongCog;
        cog.m_Transform = xc::ITransform{ glm::ivec2(3, 2), xc::Rotation90(1) };
    }

    env.Update();

    wireEntityB.add<xg::OnStageRemovedComponent>();
    env.Update();

    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    CHECK(map.size() == 7);
    CHECK(GetCellEntityCount(map, glm::ivec2(3, 2)) == 2);
    CHECK(CellContains(map, glm::ivec2(3, 2), cogEntity));
    CHECK(CellContains(map, glm::ivec2(3, 2), wireEntityA));

    CHECK(GetCellEntityCount(map, glm::ivec2(4, 2)) == 2);
    CHECK(CellContains(map, glm::ivec2(4, 2), cogEntity));
    CHECK(CellContains(map, glm::ivec2(4, 2), wireEntityA));

    CHECK(GetCellEntityCount(map, glm::ivec2(5, 2)) == 3);
    CHECK(CellContains(map, glm::ivec2(5, 2), cogEntity));
    CHECK(CellContains(map, glm::ivec2(5, 2), wireEntityA));
    CHECK(CellContains(map, glm::ivec2(5, 2), wireEntityC));

    CHECK(CellOnlyHas(map, glm::ivec2(3, 1), cogEntity));
    CHECK(CellOnlyHas(map, glm::ivec2(4, 1), cogEntity));

    CHECK(GetCellEntityCount(map, glm::ivec2(5, 1)) == 2);
    CHECK(CellContains(map, glm::ivec2(5, 1), cogEntity));
    CHECK(CellContains(map, glm::ivec2(5, 1), wireEntityC));

    CHECK(CellOnlyHas(map, glm::ivec2(5, 3), wireEntityC));

    CHECK(CellHasFlags(map, glm::ivec2(3, 2), { .m_HasNode = true, .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 2), {}));
    CHECK(CellHasFlags(map, glm::ivec2(5, 2), { .m_HasNode = true, .m_HasWireDot = true }));
    CHECK(CellHasFlags(map, glm::ivec2(5, 3), { .m_HasWireDot = true }));
    CHECK(map.contains(glm::ivec2(7, 2)) == false);

    CHECK(CellHasFlags(map, glm::ivec2(3, 1), { .m_HasNode = true }));
    CHECK(CellHasFlags(map, glm::ivec2(4, 1), {}));
    CHECK(CellHasFlags(map, glm::ivec2(5, 1), { .m_HasWireDot = true }));

    using TFlags = xg::TWireDirectionFlags;
    using EWireDir = xg::EWireDirection;

    CHECK(GetCellWireDirection(map, glm::ivec2(3, 2)) == (TFlags{} | EWireDir::E));
    CHECK(GetCellWireDirection(map, glm::ivec2(4, 2)) == (TFlags{} | EWireDir::E | EWireDir::W));
    CHECK(GetCellWireDirection(map, glm::ivec2(5, 2)) == (TFlags{} | EWireDir::N | EWireDir::S | EWireDir::W));
#
    CHECK(GetCellWireDirection(map, glm::ivec2(5, 1)) == (TFlags{} | EWireDir::N));
    CHECK(GetCellWireDirection(map, glm::ivec2(5, 3)) == (TFlags{} | EWireDir::S));
}
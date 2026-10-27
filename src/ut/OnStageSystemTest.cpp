#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandAddGroupComponent.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandDeleteGroupComponent.h"
#include "Command/CommandDeleteWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandMoveGroupComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageComponent.h"
#include "OnStageRemovedComponent.h"
#include "OnStageSystem.h"
#include "UIDragPreviewComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::OnStageSystem - " description, "[xg::OnStageSystem]")

namespace
{
    struct TestEnv
    {
        void Update()
        {
            xg::OnStageSystem::Update(m_World);

            m_World.defer_begin();
            m_World.each([](flecs::entity entity, xg::command::ExecuteComponent)
                {
                    entity.remove<xg::command::ExecuteComponent>();
                });
            m_World.defer_end();
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Executing an add cog command -> "
    "Add the OnStageComponent and OnStageAddedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    env.Update();
    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageAddedComponent>() == false);

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity.has<xg::OnStageAddedComponent>());
}

SYSTEM_TEST_CASE("Executing a delete cog command -> "
    "Remove the OnStageComponent and add OnStageRemovedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity commandDelete = world.entity();
    commandDelete.ensure<xg::command::DeleteCogComponent>().m_Cog = addedEntity;

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandDelete.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageRemovedComponent>());
}

SYSTEM_TEST_CASE("Executing an add wire command -> "
    "Add the OnStageComponent and OnStageAddedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    env.Update();
    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageAddedComponent>() == false);

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity.has<xg::OnStageAddedComponent>());
}

SYSTEM_TEST_CASE("Executing a delete wire command -> "
    "Remove the OnStageComponent and add OnStageRemovedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity commandDelete = world.entity();
    commandDelete.ensure<xg::command::DeleteWireComponent>().m_Wire = addedEntity;

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandDelete.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageRemovedComponent>());
}

SYSTEM_TEST_CASE("OnStageAddedComponent is removed next frame")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();
    env.Update();
    CHECK(addedEntity.has<xg::OnStageAddedComponent>() == false);
}

SYSTEM_TEST_CASE("OnStageRemovedComponent is removed next frame")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity commandDelete = world.entity();
    commandDelete.ensure<xg::command::DeleteWireComponent>().m_Wire = addedEntity;

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandDelete.add<xg::command::ExecuteComponent>();
    env.Update();
    env.Update();
    CHECK(addedEntity.has<xg::OnStageRemovedComponent>() == false);
}

SYSTEM_TEST_CASE("During drag to move an entity -> "
    "Remove the OnStageComponent and add OnStageRemovedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    addedEntity.add<xg::UIDragPreviewComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageRemovedComponent>());
}

SYSTEM_TEST_CASE("Drag to move ends -> "
    "Add the OnStageComponent and add OnStageAddedComponent for moved entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity addedEntity2 = world.entity();
    flecs::entity commandAdd2 = world.entity();
    commandAdd2.ensure<xg::command::AddWireComponent>();
    commandAdd2.ensure<xg::command::EntityComponent>().m_Entity = addedEntity2;

    flecs::entity addedEntity3 = world.entity();
    flecs::entity commandAdd3 = world.entity();
    commandAdd3.ensure<xg::command::AddWireComponent>();
    commandAdd3.ensure<xg::command::EntityComponent>().m_Entity = addedEntity3;

    flecs::entity commandMoveGroup = world.entity();
    {
        auto& moveGroup = commandMoveGroup.ensure<xg::command::MoveGroupComponent>();
        moveGroup.m_Entities.push_back(addedEntity);
        moveGroup.m_Entities.push_back(addedEntity2);
    }

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd2.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd3.add<xg::command::ExecuteComponent>();
    env.Update();

    addedEntity.add<xg::UIDragPreviewComponent>();
    addedEntity2.add<xg::UIDragPreviewComponent>();
    env.Update();

    commandMoveGroup.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity2.has<xg::OnStageComponent>());
    CHECK(addedEntity3.has<xg::OnStageComponent>());

    CHECK(addedEntity.has<xg::OnStageAddedComponent>());
    CHECK(addedEntity2.has<xg::OnStageAddedComponent>());
    CHECK(addedEntity3.has<xg::OnStageAddedComponent>() == false);
}

SYSTEM_TEST_CASE("Move without drag -> "
    "Do not add OnStageAddedComponent for moved entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity addedEntity2 = world.entity();
    flecs::entity commandAdd2 = world.entity();
    commandAdd2.ensure<xg::command::AddWireComponent>();
    commandAdd2.ensure<xg::command::EntityComponent>().m_Entity = addedEntity2;

    flecs::entity addedEntity3 = world.entity();
    flecs::entity commandAdd3 = world.entity();
    commandAdd3.ensure<xg::command::AddWireComponent>();
    commandAdd3.ensure<xg::command::EntityComponent>().m_Entity = addedEntity3;

    flecs::entity commandMoveGroup = world.entity();
    {
        auto& moveGroup = commandMoveGroup.ensure<xg::command::MoveGroupComponent>();
        moveGroup.m_Entities.push_back(addedEntity);
        moveGroup.m_Entities.push_back(addedEntity2);
    }

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd2.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd3.add<xg::command::ExecuteComponent>();
    env.Update();

    commandMoveGroup.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity2.has<xg::OnStageComponent>());
    CHECK(addedEntity3.has<xg::OnStageComponent>());

    CHECK(addedEntity.has<xg::OnStageAddedComponent>() == false);
    CHECK(addedEntity2.has<xg::OnStageAddedComponent>() == false);
    CHECK(addedEntity3.has<xg::OnStageAddedComponent>() == false);
}

SYSTEM_TEST_CASE("Delete group -> Set each entity offstage and add OnStageRemovedComponent for each")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity addedEntity2 = world.entity();
    flecs::entity commandAdd2 = world.entity();
    commandAdd2.ensure<xg::command::AddWireComponent>();
    commandAdd2.ensure<xg::command::EntityComponent>().m_Entity = addedEntity2;

    flecs::entity addedEntity3 = world.entity();
    flecs::entity commandAdd3 = world.entity();
    commandAdd3.ensure<xg::command::AddWireComponent>();
    commandAdd3.ensure<xg::command::EntityComponent>().m_Entity = addedEntity3;

    flecs::entity commandDeleteGroup = world.entity();
    {
        auto& deleteGroup = commandDeleteGroup.ensure<xg::command::DeleteGroupComponent>();
        deleteGroup.m_Entities.push_back(addedEntity);
        deleteGroup.m_Entities.push_back(addedEntity2);
    }

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd2.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd3.add<xg::command::ExecuteComponent>();
    env.Update();

    commandDeleteGroup.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity2.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity3.has<xg::OnStageComponent>());

    CHECK(addedEntity.has<xg::OnStageRemovedComponent>());
    CHECK(addedEntity2.has<xg::OnStageRemovedComponent>());
    CHECK(addedEntity3.has<xg::OnStageRemovedComponent>() == false);
}

SYSTEM_TEST_CASE("Add group -> Set each entity onstage and add OnStageAddedComponent for each")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddWireComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity addedEntity2 = world.entity();
    flecs::entity commandAdd2 = world.entity();
    commandAdd2.ensure<xg::command::AddWireComponent>();
    commandAdd2.ensure<xg::command::EntityComponent>().m_Entity = addedEntity2;

    flecs::entity addedEntity3 = world.entity();
    flecs::entity commandAdd3 = world.entity();
    commandAdd3.ensure<xg::command::AddWireComponent>();
    commandAdd3.ensure<xg::command::EntityComponent>().m_Entity = addedEntity3;

    flecs::entity commandDeleteGroup = world.entity();
    {
        auto& deleteGroup = commandDeleteGroup.ensure<xg::command::DeleteGroupComponent>();
        deleteGroup.m_Entities.push_back(addedEntity);
        deleteGroup.m_Entities.push_back(addedEntity2);
    }

    flecs::entity commandAddGroup = world.entity();
    {
        auto& addGroup = commandAddGroup.ensure<xg::command::AddGroupComponent>();
        addGroup.m_Entities.push_back(addedEntity);
        addGroup.m_Entities.push_back(addedEntity2);
    }

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd2.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAdd3.add<xg::command::ExecuteComponent>();
    env.Update();

    commandDeleteGroup.add<xg::command::ExecuteComponent>();
    env.Update();

    commandAddGroup.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity2.has<xg::OnStageComponent>());
    CHECK(addedEntity3.has<xg::OnStageComponent>());

    CHECK(addedEntity.has<xg::OnStageAddedComponent>());
    CHECK(addedEntity2.has<xg::OnStageAddedComponent>());
    CHECK(addedEntity3.has<xg::OnStageAddedComponent>() == false);
}

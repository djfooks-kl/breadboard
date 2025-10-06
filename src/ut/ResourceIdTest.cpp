#include <catch2/catch_test_macros.hpp>
#include <unordered_set>

#include "Core/ResourceId.h"

namespace
{
    static const char* s_Id1 = "ID_1";
    static const char* s_Id2 = "ID_2";
}

TEST_CASE("Simple Resource tests", "[xc::ResourceId]")
{
    struct TestResourceType {};
    using TestResourceId = xc::ResourceId<TestResourceType>;

    TestResourceId idEmpty;
    TestResourceId id1 = TestResourceId::Create(s_Id1);
    TestResourceId id2 = TestResourceId::Create(s_Id2);

    CHECK(idEmpty.GetName() == nullptr);
    CHECK(id1.GetName() == s_Id1);
    CHECK(id2.GetName() == s_Id2);

    CHECK((bool)idEmpty == false);
    CHECK((bool)id1 == true);
    CHECK((bool)id2 == true);

    CHECK(idEmpty == idEmpty);
    CHECK(id1 == id1);
    CHECK(id2 == id2);

    CHECK(idEmpty != id1);
    CHECK(id1 != idEmpty);
    CHECK(idEmpty != id2);
    CHECK(id2 != idEmpty);

    CHECK(id1 != id2);
    CHECK(id2 != id1);

    TestResourceId id1Copy = id1;
    TestResourceId id2Copy(id2);
    CHECK(id1Copy.GetName() == s_Id1);
    CHECK(id2Copy.GetName() == s_Id2);

    CHECK((bool)id1Copy == true);
    CHECK(id1 == id1Copy);
    CHECK(id1Copy == id1);

    CHECK(id1Copy != id2);
    CHECK(id2 != id1Copy);

    CHECK(id1Copy != idEmpty);
    CHECK(idEmpty != id1Copy);

    CHECK((bool)id2Copy == true);
    CHECK(id1 != id2Copy);
    CHECK(id2Copy != id1);

    CHECK(id2Copy == id2);
    CHECK(id2 == id2Copy);

    CHECK(id2Copy != idEmpty);
    CHECK(idEmpty != id2Copy);

    std::unordered_set<TestResourceId> set;
    set.insert(id1);
    CHECK(set.contains(id1));
}
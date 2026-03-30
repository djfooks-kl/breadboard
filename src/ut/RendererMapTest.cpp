#include <catch2/catch_test_macros.hpp>

#include "Rendering/RendererMap.h"

namespace
{
	struct TestRenderer
	{
		float GetHeight() const { return m_Height; }

		float m_Height;
	};

	static const xg::RenderableResourceId s_RenderableTest1 = xg::RenderableResourceId::Create("Test1");
	static const xg::RenderableResourceId s_RenderableTest2 = xg::RenderableResourceId::Create("Test2");
	static const xg::RenderableResourceId s_RenderableTest3 = xg::RenderableResourceId::Create("Test3");
	static const xg::RenderableResourceId s_RenderableTest4 = xg::RenderableResourceId::Create("Test4");
}

TEST_CASE("RendererMap sorting test", "[xg::RendererMap]")
{
	using TestRendererMap = xg::RendererMap<TestRenderer>;

	TestRendererMap map;

	TestRenderer* test1;
	TestRenderer* test2;
	TestRenderer* test3;
	TestRenderer* test4;

	{
		std::unique_ptr<TestRenderer> test1UPtr = std::make_unique<TestRenderer>();
		test1 = test1UPtr.get();
		test1->m_Height = 1.f;
		map.Register(s_RenderableTest1, std::move(test1UPtr));
	}
	{
		std::unique_ptr<TestRenderer> test2UPtr = std::make_unique<TestRenderer>();
		test2 = test2UPtr.get();
		test2->m_Height = 2.f;
		map.Register(s_RenderableTest2, std::move(test2UPtr));
	}
	{
		std::unique_ptr<TestRenderer> test3UPtr = std::make_unique<TestRenderer>();
		test3 = test3UPtr.get();
		test3->m_Height = 3.f;
		map.Register(s_RenderableTest3, std::move(test3UPtr));
	}
	{
		std::unique_ptr<TestRenderer> test4UPtr = std::make_unique<TestRenderer>();
		test4 = test4UPtr.get();
		test4->m_Height = 4.f;
		map.Register(s_RenderableTest4, std::move(test4UPtr));
	}

	map.SortRenderers();

	CHECK(map.Get(s_RenderableTest1) == test1);
	CHECK(map.Get(s_RenderableTest2) == test2);
	CHECK(map.Get(s_RenderableTest3) == test3);
	CHECK(map.Get(s_RenderableTest4) == test4);

	REQUIRE(map.GetOrder().size() == 4);
	CHECK(map.GetOrder()[0] == test1);
	CHECK(map.GetOrder()[1] == test2);
	CHECK(map.GetOrder()[2] == test3);
	CHECK(map.GetOrder()[3] == test4);
}
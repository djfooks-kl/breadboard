#include "BreadTest.h"

#include "Core/EnumFlags.h"

TEST_CASE("EnumFlag tests", "[xc::EnumFlag]")
{
    enum class ETest : uint16_t
    {
        None = 0,
        A = BIT(0),
        B = BIT(1),
        C = BIT(2),
        D = BIT(3),
        E = BIT(4),
        F = BIT(5),
        G = BIT(6),
        H = BIT(7),
        I = BIT(8),
        J = BIT(9),
    };

    xc::EnumFlags<ETest> flags;
    CHECK(flags.Get() == ETest::None);

    flags.Raise(ETest::A);
    CHECK(flags.Get() == ETest::A);

    flags.Raise(ETest::J);
    CHECK(flags.Get() == static_cast<ETest>(static_cast<uint16_t>(ETest::A) | static_cast<uint16_t>(ETest::J)));

    CHECK(flags.Has(ETest::A));
    CHECK(flags.Has(ETest::B) == false);
    CHECK(flags.Has(ETest::J));

    flags.Clear(ETest::J);
    CHECK(flags.Has(ETest::J) == false);
    CHECK(flags.Has(ETest::A));

    flags.ClearAll();
    CHECK(flags.Has(ETest::A) == false);

    flags.Raise(ETest::B);
    CHECK(flags.GetValue() == BIT(1));

    flags.Raise(ETest::C);
    CHECK(flags.GetValue() == (BIT(1) | BIT(2)));

    flags |= xc::EnumFlags<ETest>(ETest::D) | ETest::F | ETest::H;
    CHECK(flags.HasAll(static_cast<ETest>(
        static_cast<uint16_t>(ETest::B) |
        static_cast<uint16_t>(ETest::C) |
        static_cast<uint16_t>(ETest::D) |
        static_cast<uint16_t>(ETest::F))));

    CHECK(flags.HasAll(static_cast<ETest>(
        static_cast<uint16_t>(ETest::A) |
        static_cast<uint16_t>(ETest::B) |
        static_cast<uint16_t>(ETest::C) |
        static_cast<uint16_t>(ETest::D) |
        static_cast<uint16_t>(ETest::F))) == false);

    CHECK(flags.HasAll(xc::EnumFlags<ETest>() |
        ETest::B |
        ETest::C |
        ETest::D |
        ETest::F) == true);

    CHECK(flags.HasAll(xc::EnumFlags<ETest>() |
        ETest::A |
        ETest::B |
        ETest::C |
        ETest::D |
        ETest::F) == false);

    flags |= ETest::G;
    CHECK(flags.Has(ETest::G));

    xc::EnumFlags<ETest> otherFlags = xc::EnumFlags<ETest>() | ETest::I | ETest::J;
    CHECK(otherFlags.Has(ETest::I));
    CHECK(otherFlags.Has(ETest::J));

    CHECK(flags.HasAny(otherFlags) == false);
    flags.Raise(ETest::I);
    CHECK(flags.HasAny(otherFlags) == true);

    flags.ClearAll();
    otherFlags.ClearAll();
    CHECK(flags.HasAny() == false);
    flags.Raise(ETest::C);
    CHECK(flags.HasAny() == true);

    otherFlags.Raise(ETest::C);
    CHECK(otherFlags == flags);
    otherFlags.Clear(ETest::C);
    CHECK(otherFlags != flags);
}
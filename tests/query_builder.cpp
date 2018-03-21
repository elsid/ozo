#include <GUnit/GTest.h>

#include <ozo/query_builder.h>

#include <boost/hana/size.hpp>

GTEST("ozo::detail::to_string") {
    SHOULD("with 0 returns \"0\"_s") {
        using namespace boost::hana::literals;
        EXPECT_EQ(ozo::detail::to_string<std::size_t(0)>(), "0"_s);
    }

    SHOULD("with one digit number returns string with same digit") {
        using namespace boost::hana::literals;
        EXPECT_EQ(ozo::detail::to_string<std::size_t(7)>(), "7"_s);
    }

    SHOULD("with two digits number returns string with digits in same order") {
        using namespace boost::hana::literals;
        EXPECT_EQ(ozo::detail::to_string<std::size_t(42)>(), "42"_s);
    }
}

GTEST("ozo::query_builder::text") {
    SHOULD("with one text element returns input") {
        using namespace ozo::literals;
        using namespace boost::hana::literals;
        EXPECT_EQ("SELECT 1"_SQL.text(), "SELECT 1"_s);
    }

    SHOULD("with two text elements returns concatenation") {
        using namespace ozo::literals;
        using namespace boost::hana::literals;
        EXPECT_EQ(("SELECT 1"_SQL + " + 1"_SQL).text(), "SELECT 1 + 1"_s);
    }

    SHOULD("with text and int32 param elements returns text with placeholder for param") {
        using namespace ozo::literals;
        using namespace boost::hana::literals;
        EXPECT_EQ(("SELECT "_SQL + std::int32_t(42)).text(), "SELECT $1"_s);
    }

    SHOULD("with text and two int32 params elements returns text with placeholders for each param") {
        using namespace ozo::literals;
        using namespace boost::hana::literals;
        EXPECT_EQ(("SELECT "_SQL + std::int32_t(42) + " + "_SQL + std::int32_t(42)).text(), "SELECT $1 + $2"_s);
    }
}

GTEST("ozo::query_builder::params") {
    SHOULD("with one text element returns empty tuple") {
        using namespace ozo::literals;
        EXPECT_EQ("SELECT 1"_SQL.params(), boost::hana::tuple<>());
    }

    SHOULD("with text and int32 param elements returns tuple with one value") {
        using namespace ozo::literals;
        EXPECT_EQ(("SELECT "_SQL + std::int32_t(42)).params(), boost::hana::make_tuple(std::int32_t(42)));
    }

    SHOULD("with text and not null pointer param elements returns tuple with one value") {
        using namespace ozo::literals;
        const auto ptr = std::make_unique<std::int32_t>(42);
        const auto params = ("SELECT "_SQL + ptr.get()).params();
        EXPECT_EQ(*params[boost::hana::size_c<0>], std::int32_t(42));
    }
}

namespace ozo::testing {

struct some_type {
    std::size_t size() const {
        return 1000;
    }
};

} // namespace ozo::testing

OZO_PG_DEFINE_CUSTOM_TYPE(ozo::testing::some_type, "some_type", dynamic_size)

GTEST("ozo::query_builder::build") {
    namespace hana = boost::hana;

    SHOULD("with one text element returns query with text equal to input") {
        using namespace ozo::literals;
        EXPECT_EQ("SELECT 1"_SQL.build().text, "SELECT 1");
    }

    SHOULD("with one text element returns query without params") {
        using namespace ozo::literals;
        EXPECT_EQ("SELECT 1"_SQL.build().params, hana::tuple<>());
    }

    SHOULD("with text and int32 param elements return query with 1 param") {
        using namespace ozo::literals;
        EXPECT_EQ(("SELECT "_SQL + std::int32_t(42)).build().params, hana::make_tuple(42));
    }

    SHOULD("with text and reference_wrapper param element returns query with 1 param") {
        using namespace ozo::literals;
        const auto value = 42.13f;
        EXPECT_EQ(("SELECT "_SQL + std::cref(value)).build().params, hana::make_tuple(std::cref(value)));
    }

    SHOULD("with text and not null std::unique_ptr param element returns query with 1 param") {
        using namespace ozo::literals;
        const auto ptr = std::make_unique<float>(42.13f);
        const auto params = ("SELECT "_SQL + ptr).build().params;
        EXPECT_EQ(decltype(hana::size(params))::value, 1);
    }

    SHOULD("with text and not null std::shared_ptr param element returns query with 1 param") {
        using namespace ozo::literals;
        const auto ptr = std::make_shared<float>(42.13f);
        const auto params = ("SELECT "_SQL + ptr).build().params;
        EXPECT_EQ(decltype(hana::size(params))::value, 1);
    }

    SHOULD("with text and custom type param element returns query with 1 param") {
        using namespace ozo::literals;
        const auto params = ("SELECT "_SQL + ozo::testing::some_type {}).build().params;
        EXPECT_EQ(decltype(hana::size(params))::value, 1);
    }
}
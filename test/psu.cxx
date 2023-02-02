#include <gtest/gtest.h>
#include <core/Client.hxx>
#include <interfaces/Psu.hxx>

class PsuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        const char *props = std::getenv("PROPS_PATH");
        
        if (props)
            Core::LoadAliasesFromFile(props + std::string("/alias/good.json"));
        else
            Core::LoadAliasesFromFile("alias/good.json");
        client = std::make_unique<Client>("good");
        ASSERT_EQ(client->connect(), 0);
        psu = std::make_unique<Psu>(*client, "psu");
        ASSERT_EQ(psu->init(), 0);
        ASSERT_TRUE(psu->isRunning());
    }

    std::unique_ptr<Client> client;
    std::unique_ptr<Psu> psu;
};

TEST_F(PsuTest, State)
{
    psu->state.value.set("on");
    EXPECT_EQ(psu->state.value.get(), "on");
    psu->state.value.set("off");
    EXPECT_EQ(psu->state.value.get(), "off");
}
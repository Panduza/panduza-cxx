#include <gtest/gtest.h>
#include <pza/core/Client.hxx>
#include <pza/interfaces/Psu.hxx>

using namespace pza;

class PsuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        const char *props = std::getenv("PROPS_PATH");

        Core::RemoveAliases();

        if (props)
            Core::LoadAliasesFromFile(props + std::string("/alias/good.json"));
        else
            Core::LoadAliasesFromFile("alias/good.json");
        client = std::make_shared<Client>("good");
        ASSERT_EQ(client->connect(), 0);
        psu = std::make_unique<Psu>("psu");
        psu->bindToClient(client.get());
        ASSERT_TRUE(psu->isRunning());
    }

    std::shared_ptr<Client> client;
    std::unique_ptr<Psu> psu;
};

TEST_F(PsuTest, Enable)
{
    psu->enable.value.set(true);
    EXPECT_EQ(psu->enable.value.get(), true);
    psu->enable.value.set(false);
    EXPECT_EQ(psu->enable.value.get(), false);
}

TEST_F(PsuTest, VoltsValue)
{
    psu->volts.goal.set(4.2);
    EXPECT_EQ(psu->volts.real.get(), 4.2);
    EXPECT_EQ(psu->volts.goal.get(), 4.2);

    psu->volts.goal.set(8);
    EXPECT_EQ(psu->volts.real.get(), 8);
    EXPECT_EQ(psu->volts.goal.get(), 8);
}

TEST_F(PsuTest, AmpsValue)
{
    psu->amps.goal.set(4.2);
    EXPECT_EQ(psu->amps.real.get(), 4.2);
    EXPECT_EQ(psu->amps.goal.get(), 4.2);

    psu->amps.goal.set(8);
    EXPECT_EQ(psu->amps.real.get(), 8);
    EXPECT_EQ(psu->amps.goal.get(), 8);
}
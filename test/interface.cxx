#include <gtest/gtest.h>
#include <core/Client.hxx>
#include <interfaces/Psu.hxx>

// Using PSU as an example, but this applies to all interfaces

class InterfaceTest : public ::testing::Test
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
    }

    std::unique_ptr<Client> client;
    std::unique_ptr<Psu> psu;
};

TEST_F(InterfaceTest, Init)
{
    EXPECT_EQ(psu->init(), 0);
    EXPECT_TRUE(psu->isRunning());
}

TEST_F(InterfaceTest, Disconnect)
{
    EXPECT_EQ(psu->init(), 0);
    EXPECT_TRUE(psu->isRunning());
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_FALSE(psu->isRunning());
}

TEST_F(InterfaceTest, DisconnectAndConnect)
{
    EXPECT_EQ(psu->init(), 0);
    EXPECT_TRUE(psu->isRunning());
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_FALSE(psu->isRunning());
    EXPECT_EQ(client->connect(), 0);
    EXPECT_TRUE(psu->isRunning());
}

TEST_F(InterfaceTest, Reconnect)
{
    EXPECT_EQ(psu->init(), 0);
    EXPECT_TRUE(psu->isRunning());
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_FALSE(psu->isRunning());
    EXPECT_EQ(client->reconnect(), 0);
    EXPECT_TRUE(psu->isRunning());
}
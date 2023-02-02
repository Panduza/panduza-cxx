#include <gtest/gtest.h>
#include <core/Client.hxx>

class AliasTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        Core::removeAliases();
    }

    void loadAlias(const std::string &json)
    {
        Core::LoadAliases(json);
    }
};

class AliasFail : public AliasTest,
                  public ::testing::WithParamInterface<std::pair<std::string, int>>
{
protected:
    virtual void SetUp()
    {
        AliasTest::SetUp();
        loadAlias(GetParam().first);
    }
};

class AliasFile : public AliasTest,
                  public ::testing::WithParamInterface<std::string>
{
protected:
    virtual void SetUp()
    {
        AliasTest::SetUp();
    }

    void loadFile(const std::string &file)
    {
        const char *props = std::getenv("PROPS_PATH");
        
        if (props)
            Core::LoadAliasesFromFile(props + std::string("/alias/") + file);
        else
            Core::LoadAliasesFromFile("alias/" + file);
    }
};

using AliasFileFail = AliasFile;

class AliasFolder : public AliasTest,
                    public ::testing::WithParamInterface<std::pair<std::string, int>>
{
protected:
    virtual void SetUp()
    {
        AliasTest::SetUp();
    }

    void loadFolder(const std::string &folder)
    {
        const char *props = std::getenv("PROPS_PATH");

        if (props)
            Core::LoadAliasesFromDirectory(props + std::string("/alias/") + folder);
        else
            Core::LoadAliasesFromDirectory("alias/" + folder);
    }
};

TEST_F(AliasTest, AliasSingle)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        }
    })");
    EXPECT_EQ(Core::AliasesCount(), 1);
    ASSERT_TRUE(Core::findAlias("local"));
}

TEST_F(AliasTest, AliasMultiple)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local2": {
            "url": "localhost",
            "port": 1883
        }
    })");
    EXPECT_EQ(Core::AliasesCount(), 2);
    ASSERT_TRUE(Core::findAlias("local"));
    ASSERT_TRUE(Core::findAlias("local2"));
}

TEST_F(AliasTest, AliasDuplicate)
{
    Alias *ptr;

    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local": {
            "url": "newlocalhost",
            "port": 1885
        }
    })");
    EXPECT_EQ(Core::AliasesCount(), 1);
    ASSERT_TRUE(ptr = Core::findAlias("local"));
    EXPECT_EQ(ptr->url, "newlocalhost");
    EXPECT_EQ(ptr->port, 1885);
}

TEST_F(AliasTest, AliasDelete)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local2": {
            "url": "localhost",
            "port": 1883
        }
    })");
    EXPECT_EQ(Core::AliasesCount(), 2);
    ASSERT_TRUE(Core::findAlias("local"));
    Core::removeAlias("local");
    EXPECT_EQ(Core::AliasesCount(), 1);
    EXPECT_FALSE(Core::findAlias("local"));
}

TEST_F(AliasTest, AliasDeleteAll)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local2": {
            "url": "localhost",
            "port": 1883
        }
    })");
    EXPECT_EQ(Core::AliasesCount(), 2);
    ASSERT_TRUE(Core::findAlias("local"));
    ASSERT_TRUE(Core::findAlias("local2"));
    Core::removeAliases();
    EXPECT_EQ(Core::AliasesCount(), 0);
    EXPECT_FALSE(Core::findAlias("local"));
    EXPECT_FALSE(Core::findAlias("local2"));
}

TEST_F(AliasTest, AliasSingleInterface)
{
    std::string buf;

    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test"
            }
        }
    })");
    ASSERT_TRUE(Core::findAlias("local"));
    ASSERT_TRUE(Core::findAlias("local")->hasInterface("test"));
    EXPECT_EQ(Core::findAlias("local")->getInterfaceTopic("test", buf), 0);
    EXPECT_EQ(buf, "pza/machine/driver/test");
    EXPECT_EQ(Core::findAlias("local")->getInterfaceNameFromTopic("pza/machine/driver/test", buf), 0);
    EXPECT_EQ(buf, "test");
}

TEST_F(AliasTest, AliasMultipleInterface)
{
    std::string buf1;
    std::string buf2;

    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test",
                "test2": "pza/machine/driver/test2"
            }
        }
    })");
    ASSERT_TRUE(Core::findAlias("local"));
    ASSERT_TRUE(Core::findAlias("local")->hasInterface("test"));
    ASSERT_TRUE(Core::findAlias("local")->hasInterface("test2"));
    EXPECT_EQ(Core::findAlias("local")->getInterfaceTopic("test", buf1), 0);
    EXPECT_EQ(Core::findAlias("local")->getInterfaceTopic("test2", buf2), 0);
    EXPECT_EQ(buf1, "pza/machine/driver/test");
    EXPECT_EQ(buf2, "pza/machine/driver/test2");
    EXPECT_EQ(Core::findAlias("local")->getInterfaceNameFromTopic("pza/machine/driver/test", buf1), 0);
    EXPECT_EQ(Core::findAlias("local")->getInterfaceNameFromTopic("pza/machine/driver/test2", buf2), 0);
    EXPECT_EQ(buf1, "test");
    EXPECT_EQ(buf2, "test2");
}

TEST_F(AliasTest, AliasDuplicateInterface)
{
    std::string buf;

    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test",
                "test": "pza/machine/driver/test2"
            }
        }
    })");
    ASSERT_TRUE(Core::findAlias("local"));
    ASSERT_TRUE(Core::findAlias("local")->hasInterface("test"));
    EXPECT_EQ(Core::findAlias("local")->getInterfaceTopic("test", buf), 0);
    EXPECT_EQ(buf, "pza/machine/driver/test2");
    EXPECT_EQ(Core::findAlias("local")->getInterfaceNameFromTopic("pza/machine/driver/test2", buf), 0);
    EXPECT_EQ(buf, "test");
}

TEST_F(AliasTest, AliasDuplicateAliasInterface)
{
    std::string buf;

    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test"
            }
        },
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test2"
            }
        }
    })");
    ASSERT_TRUE(Core::findAlias("local"));
    ASSERT_TRUE(Core::findAlias("local")->hasInterface("test"));
    EXPECT_EQ(Core::findAlias("local")->getInterfaceTopic("test", buf), 0);
    EXPECT_EQ(buf, "pza/machine/driver/test2");
    EXPECT_EQ(Core::findAlias("local")->getInterfaceNameFromTopic("pza/machine/driver/test2", buf), 0);
    EXPECT_EQ(buf, "test");
}


TEST_P(AliasFail, BadFormat)
{
    EXPECT_EQ(Core::AliasesCount(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(TestAliasFail, AliasFail, ::testing::Values(
    std::make_pair(R"(
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test",
                "test2": "pza/machine/driver/test2"
            }
        }
    })", 0),
    std::make_pair(R"({
        "local" {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test",
                "test2": "pza/machine/driver/test2"
            }
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost"
            "port": 1883,
            "interfaces": {
                "test": "pza/machine/driver/test",
                "test2": "pza/machine/driver/test2"
            }
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": 
                "test": "pza/machine/driver/test",
                "test2": "pza/machine/driver/test2"
            }
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
                "test": "pza/machine/driver/test",
                "test2": "pza/machine/driver/test2"
            }
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "port": 1883
        }
    })", 0),
    std::make_pair(R"({
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost"
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost"
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local2" {
            "url": "localhost",
            "port": 1883
        }
    })", 0),
    std::make_pair(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local2": {
            "port": 1883
        }
    })", 1)
));

TEST_F(AliasFile, Good)
{
    loadFile("good.json");
    EXPECT_EQ(Core::AliasesCount(), 1);
}

TEST_P(AliasFileFail, BadFile)
{
    loadFile(GetParam());
    EXPECT_EQ(Core::AliasesCount(), 0);
}

INSTANTIATE_TEST_SUITE_P(TestAliasFileFail, AliasFileFail, ::testing::Values(
    "empty.json",
    "doesnotexist.json",
    "permissiondenied.json",
    "fifo.json",
    "folder_empty"
));

TEST_P(AliasFolder, Folder)
{
    loadFolder(GetParam().first);
    EXPECT_EQ(Core::AliasesCount(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(TestAliasFolder, AliasFolder, ::testing::Values(
    std::make_pair("folder_single", 1),
    std::make_pair("folder_multiple", 2),
    std::make_pair("folder_multiple_duplicate", 1),
    std::make_pair("folder_partial_good", 1),
    std::make_pair("fifo.json", 0),
    std::make_pair("good.json", 0),
    std::make_pair("folder_empty", 0),
    std::make_pair("folder_permission_denied", 0)
));
#include <core/Client.hxx>
#include <core/Core.hxx>
#include<unistd.h>

int main(void)
{
    Core::SetLogLevel(Core::LogLevel::Trace);

//    Core::LoadAliases(
//    R"({
//        "local": {
//            "url": "localhost",
//            "port": 1883,
//            "interfaces": {
//                "test" : "pza/mymachine/psu_fake/myPsu"
//            }
//        },
//        "toto": {
//            "url": "localhost",
//            "port": 1883,
//            "interfaces": {
//                "test" : "pza/mymachine/psu_fake/myPsu"
//            }
//        }
//    })");
//
    Core::LoadAliasesFromFile("toto/alias");

    Core::ShowAliases();

    Client *client = new Client("local");
    
    if (client->connect() == -1)
        return 1;

    while (1)
        ;
    return 0;
}
#include <core/Client.hxx>
#include <core/Core.hxx>
#include <interfaces/Psu.hxx>

int main(void)
{
    Core::SetLogLevel(Core::LogLevel::Trace);
    Core::LoadAliases(R"(
    {
        "good": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "psu": "pza/machine/py.psu.fake/My Psu"
            }
        }
    })");
    Client client("good");
    client.connect();
    sleep(1);
    auto psu = std::make_unique<Psu>(client, "psu");
    if (psu->init() != 0)
        return -1;

    psu->volts.goal.set(42);
    printf("%f\n", psu->volts.goal.get());
    printf("%f\n", psu->volts.real.get());

    psu->volts.goal.set(28);

    printf("%f\n", psu->volts.min.get());
    printf("%f\n", psu->volts.real.get());

    psu->amps.goal.set(4);
    printf("%f\n", psu->amps.goal.get());
    printf("%f\n", psu->amps.real.get());

    psu->amps.goal.set(2);

    printf("%f\n", psu->amps.goal.get());
    printf("%f\n", psu->amps.real.get());

    client.disconnect();

    return 0;
}
#include <unistd.h>

#include <core/client.hxx>

#include <interfaces/bps_chan_ctrl.hxx>
#include <interfaces/meter.hxx>

int main()
{
    core::set_log_level(core::log_level::debug);

    client::ptr cli = std::make_shared<client>("localhost", 1883);

    if (cli->connect() < 0)
        return -1;

    while (1)
        ;
}

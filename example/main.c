#include <vkmedia.h>

#include <signal.h>

#include "example.h"

vkm_example *example;

void signal_handler(int signal)
{
    vkm_example_quit(example);
}

int main(int argc, char **argv)
{
    vkm_example_parameters example_parameters = {
        .width = 640,
        .height = 480,
        .fps = 10,
        .device_index = 0,
    };
    if (argc > 1)
    {
        example_parameters.shader_directory = argv[1];
        vkm_example_create(&example_parameters, &example);
    }
    else
    {
        example = NULL;
        puts("no shader directory provided");
    }
    if (example != NULL)
    {
        signal(SIGINT, signal_handler);
        signal(SIGQUIT, signal_handler);

        vkm_example_loop(example);

        vkm_example_destroy(example);
    }
    return 0;
}
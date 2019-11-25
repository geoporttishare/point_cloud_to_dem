#include "ProgramCmdOpts.h"
#include "program.h"

int main(int argc, char** argv)
{
    ProgramCmdOpts opts;

    if (!opts.parse(argc, argv)) return 0;

    program(opts);

    return 0;
}

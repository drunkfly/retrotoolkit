#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv)
{
    const char* resultFile = nullptr;
    if (argc >= 3 && !strcmp(argv[argc - 2], "--resultfile")) {
        resultFile = argv[argc - 1];
        argc -= 2;
    }

    int r = Catch::Session().run(argc, argv);

    if (resultFile) {
        if (r != EXIT_SUCCESS) {
            if (remove(resultFile) != 0 && errno != ENOENT) {
                fprintf(stderr, "unable to remove file \"%s\": %s\n", resultFile, strerror(errno));
                return EXIT_FAILURE;
            }
        } else {
            FILE* f = fopen(resultFile, "wb");
            if (!f) {
                fprintf(stderr, "unable to write file \"%s\": %s\n", resultFile, strerror(errno));
                return EXIT_FAILURE;
            }
            fputs("unsigned char TestsWereSuccessful;\n", f);
            fclose(f);
        }
    }

    return r;
}

#include <stdio.h>
#include <tamtypes.h>
#include <loadfile.h>

extern unsigned char patch_loader_irx[];
extern unsigned int size_patch_loader_irx;

int main()
{

    printf("Loading IRX\n");

    if (SifExecModuleBuffer(patch_loader_irx, size_patch_loader_irx, 0, NULL, NULL) < 0) {
        printf("Failed to load patch loader irx\n");
        return -1;
    }

    printf("Done\n");

    return 0;
}
#include <stdio.h>
#include "include/metadata.h"

int main() {
    printf("Hello, World!\n");

    char sharedFileName[] = "test";
    createMetadataFile(sharedFileName, 5, "trackerUrl", 10);


    return 0;
}

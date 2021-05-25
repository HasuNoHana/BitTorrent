#include <stdio.h>
#include "include/metadata.h"

int main() {
    printf("Hello, World!\n");

    char sharedFileName[] = "test";
    createMetadataFile(sharedFileName, 5);


    return 0;
}

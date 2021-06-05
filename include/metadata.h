//
// Created by zuznanna on 25.05.2021.
//

#ifndef TIN_METADATA_H
#define TIN_METADATA_H

void createMetadataFile(char *sharedFileName, int sharedFileNameSize, char *trackerUrl, int trackerUrlSize );
long long getSharedFileSizeInBytes(char *sharedPath);

#endif //TIN_METADATA_H

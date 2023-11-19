#include "volume.h"

#include <libs.h>

void VoxelVolume::gen_data(int w, int h, int d) {
    data.width = w;
    data.height = h;
    data.depth = d;

    data.voxels = new unsigned char[w * h * d];
    for (int i = 0; i < w * h * d; i++) {
        if (rand() % 16 == 0) {
            data.voxels[i] = rand() % 256;
        } else {
            data.voxels[i] = 0;
        }
    }

    if (data.handle == 0) {
        glGenTextures(1, &data.handle);
        glBindTexture(GL_TEXTURE_3D, data.handle);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 1);
    } else {
        glBindTexture(GL_TEXTURE_3D, data.handle);
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE,
                 data.voxels);
    // glGenerateMipmap(GL_TEXTURE_3D);
}

void VoxelVolume::gen_mipmap() {
    unsigned char level2[(data.width / 2) * (data.height / 2) * (data.depth / 2)] = {};

    for (int z = 0; z < data.depth; z+=2)
    {
        for (int y = 0; y < data.height; y+=2)
        {
            for (int x = 0; x < data.width; x+=2)
            {
                // TODO: Clean up this cursed goofy ah code.
                unsigned int sum = 0u;
                unsigned int div = 1u;
                {
                    unsigned char d = data.voxels[x + data.width * (y + data.depth * z)];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[(x + 1) + data.width * (y + data.depth * z)];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[(x + 1) + data.width * ((y + 1) + data.depth * z)];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[(x + 1) + data.width * ((y + 1) + data.depth * (z + 1))];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[x + data.width * ((y + 1) + data.depth * z)];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[x + data.width * ((y + 1) + data.depth * (z + 1))];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[x + data.width * (y + data.depth * (z + 1))];
                    if (d > 0) div++;
                    sum += d;
                }
                {
                    unsigned char d = data.voxels[(x + 1) + data.width * (y + data.depth * (z + 1))];
                    if (d > 0) div++;
                    sum += d;
                }
                sum /= div;
                level2[(x / 2) + (data.width / 2) * ((y / 2) + (data.depth / 2) * (z / 2))] = static_cast<unsigned char>(sum);
            }
        }
    }
    
    glBindTexture(GL_TEXTURE_3D, data.handle);
    glTexImage3D(GL_TEXTURE_3D, 1, GL_R8, data.width / 2, data.height / 2, data.depth / 2, 0, GL_RED, GL_UNSIGNED_BYTE,
                 level2);
}

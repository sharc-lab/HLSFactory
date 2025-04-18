#include <fstream>
#include <sstream>
#include <stdint.h>

#include "common.h"
#include "tb_common.h"
#include "trianglemesh.h"

using namespace std;

void generateTriangleIndexArr(fixed_t transformNormals[4][4],
    const uint32_t faceIndex[NUM_FACES],
    uint32_t trisIndex[NUM_TRIS * 3],
    const uint32_t vertsIndex[VERTS_INDEX_ARR_SIZE],
    fixed_t normals[VERTS_INDEX_ARR_SIZE][3],
    fixed_t N[NUM_TRIS * 3][3],
    fixed_t texCoordinates[NUM_TRIS * 3][2],
    fixed_t st[VERTS_INDEX_ARR_SIZE][2])
{
    uint32_t l = 0;

    for (uint32_t i = 0, k = 0; i < NUM_FACES; ++i)
    {
        // for each  face
        for (uint32_t j = 0; j < faceIndex[i] - 2; ++j)
        {
            // for each triangle in the face
            trisIndex[l] = vertsIndex[k];
            trisIndex[l + 1] = vertsIndex[k + j + 1];
            trisIndex[l + 2] = vertsIndex[k + j + 2];
            // std::cout << "trisIndex: [" << trisIndex[l] << "," << trisIndex[l+1] << "," << trisIndex[l+2] << "]\n";

            customMultDirMatrix(normals[k], N[l], transformNormals);
            customMultDirMatrix(normals[k + j + 1], N[l + 1], transformNormals);
            customMultDirMatrix(normals[k + j + 2], N[l + 2], transformNormals);

            for (int ii = 0; ii < 3; ++ii)
            {
                customNormalize3(N[l + ii]);
            }

            texCoordinates[l][0] = st[k][0];
            texCoordinates[l][1] = st[k][1];

            texCoordinates[l + 1][0] = st[k + j + 1][0];
            texCoordinates[l + 1][1] = st[k + j + 1][1];

            texCoordinates[l + 2][0] = st[k + j + 2][0];
            texCoordinates[l + 2][1] = st[k + j + 2][1];

            l += 3;
        }
        k += faceIndex[i];
    }
}

void build_mesh(
    triangle_mesh_t &mesh,
    fixed_t o2w[4][4],
    const uint32_t nfaces,
    const uint32_t faceIndex[NUM_FACES],
    const uint32_t vertsIndex[VERTS_INDEX_ARR_SIZE],
    fixed_t verts[VERTS_ARR_SIZE][3],
    fixed_t normals[VERTS_INDEX_ARR_SIZE][3],
    fixed_t st[VERTS_INDEX_ARR_SIZE][2]
)
{
    customCopy44(o2w, mesh.objectToWorld);
    customInverse(o2w, mesh.worldToObject);

    // find out how many triangles we need to create for this mesh
    uint32_t k = 0, maxVertIndex = MAX_VERT_INDEX;
    for (uint32_t i = 0; i < maxVertIndex; ++i)
    {
        // Transforming vertices to world space
        customMultVecMatrix(verts[i], mesh.P[i], mesh.objectToWorld);
    }

    // Generate the triangle index array
    fixed_t transformNormals[4][4];

    customInverse(mesh.worldToObject, transformNormals);

    generateTriangleIndexArr(transformNormals,
        faceIndex,
        mesh.trisIndex,
        vertsIndex,
        normals,
        mesh.N,
        mesh.texCoordinates,
        st);
}

void loadPolyMeshFromFile(triangle_mesh_t &mesh, const char *file, fixed_t o2w[4][4])
{
    std::ifstream ifs;
    uint32_t numFaces = NUM_FACES;
    uint32_t vertsIndexArraySize = VERTS_INDEX_ARR_SIZE;
    uint32_t vertsArraySize = VERTS_ARR_SIZE;
    uint32_t faceIndex[NUM_FACES];
    uint32_t vertsIndex[VERTS_INDEX_ARR_SIZE];

    float verts[VERTS_ARR_SIZE][3];
    fixed_t fixp_verts[VERTS_ARR_SIZE][3];

    float normals[VERTS_INDEX_ARR_SIZE][3];
    fixed_t fixp_normals[VERTS_INDEX_ARR_SIZE][3];

    float st[VERTS_INDEX_ARR_SIZE][2];
    fixed_t fixp_st[VERTS_INDEX_ARR_SIZE][2];

    ifs.open(file);
    if (ifs.fail()) throw;
    std::stringstream ss;
    ss << ifs.rdbuf();
    ss >> numFaces;

    // reading face index array
    for (uint32_t i = 0; i < numFaces; ++i)
    {
        ss >> faceIndex[i];
    }

    // reading vertex index array
    for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
    {
        ss >> vertsIndex[i];
    }

    // reading vertices
    for (uint32_t i = 0; i < vertsArraySize; ++i)
    {
        ss >> verts[i][0] >> verts[i][1] >> verts[i][2];
        //Convert type
        for (uint32_t j = 0; j < 3; j++)
        {
        	fixp_verts[i][j] = (fixed_t) verts[i][j];
        }
    }

    // reading normals
    for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
    {
        ss >> normals[i][0] >> normals[i][1] >> normals[i][2];
        //Convert type
        for (uint32_t j = 0; j < 3; j++)
        {
        	fixp_normals[i][j] = (fixed_t) normals[i][j];
        }
    }

    // reading st coordinates
    for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
    {
        ss >> st[i][0] >> st[i][1];
        //Convert type
        for (uint32_t j = 0; j < 2; j++)
        {
        	fixp_st[i][j] = (fixed_t) st[i][j];
        }
    }

    // return triangle_mesh_t(o2w, numFaces, faceIndex, vertsIndex, verts, normals, st);
    build_mesh(mesh, o2w, numFaces, faceIndex, vertsIndex, fixp_verts, fixp_normals, fixp_st);
}

// In the main function of the program, we create the scene (create objects and lights)
// as well as set the options for the render (image widht and height, maximum recursion
// depth, field-of-view, etc.). We then call the render function().
int main(int argc, char **argv)
{
    // loading geometry
    fixed_t objectToWorld[4][4] = {
        {1.624241, 0, 2.522269, 0},
        {0, 3, 0, 0},
        {-2.522269, 0, 1.624241, 0},
        {0, 0, 0, 1}
    };
    fixed_t backgroundColor[3] = {0.235294, 0.67451, 0.843137};
    fixed_t cameraToWorld[4][4] = {
        {0.931056, 0, 0.364877, 0},
        {0.177666, 0.873446, -0.45335, 0},
        {-0.3187, 0.48692, 0.813227, 0},
        {-41.229214, 81.862351, 112.456908, 1}
    };

    uint32_t frame = 0;
    fixed_t framebuffer[WIDTH * HEIGHT][3];
    triangle_mesh_t mesh;

    loadPolyMeshFromFile(mesh, "./teapot.geo", objectToWorld);

    float frame_width, frame_height, frame_fov;
    frame_width = (float)WIDTH;
    frame_height = (float)HEIGHT;
    frame_fov = (float)FOV;

    fixed_t frame_width_ft, frame_height_ft, frame_fov_ft;
    frame_width_ft = (fixed_t)frame_width;
    frame_height_ft = (fixed_t)frame_height;

    float frame_scale = tan(frame_fov * 0.5 * M_PI / 180);
    fixed_t frame_scale_ft = (fixed_t)frame_scale;

    // Build primitive arrays
    fixed_t PBuffer1[NUM_TRIS][3], PBuffer2[NUM_TRIS][3], PBuffer3[NUM_TRIS][3];
    for (uint32_t i = 0; i < NUM_TRIS; ++i)
    {
        getPrimitive(mesh.P, mesh.trisIndex, &PBuffer1[i][0], &PBuffer2[i][0], &PBuffer3[i][0], i);
    }

    // finally, render
    render(PBuffer1, PBuffer2, PBuffer3, mesh.texCoordinates, framebuffer, cameraToWorld, backgroundColor,
        frame_width_ft, frame_height_ft, frame_scale_ft);

    // save framebuffer to file
    char buff[256];
    #ifdef CSIM_DEBUG
        sprintf(buff, "out.%04d.ppm", frame);
    #else
        sprintf(buff, "out.%04d1.ppm", frame);
    #endif
    std::ofstream ofs;
    ofs.open(buff);
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (uint32_t i = 0; i < HEIGHT * WIDTH; ++i)
    {
        char r = (char)(255 * customClamp(0, 1, framebuffer[i][0]));
        char g = (char)(255 * customClamp(0, 1, framebuffer[i][1]));
        char b = (char)(255 * customClamp(0, 1, framebuffer[i][2]));
        ofs << r << g << b;
    }
    ofs.close();

    return 0;
}

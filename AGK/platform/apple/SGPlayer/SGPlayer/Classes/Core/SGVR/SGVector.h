//
//  SGVector.h
//  SGPlayer
//
//  Created by C.W. Betts on 3/2/20.
//  Copyright © 2020 single. All rights reserved.
//

#ifndef SGVector_h
#define SGVector_h

#include <tgmath.h>
#include <simd/simd.h>

static inline float SGDegreesToRadians(float degrees) { return degrees * (M_PI / 180); };

static inline simd_float4x4 SGMatrix4x4MakeLookAt(float eyeX, float eyeY, float eyeZ,
                                                    float centerX, float centerY, float centerZ,
                                                    float upX, float upY, float upZ)
{
    vector_float3 ev = { eyeX, eyeY, eyeZ };
    vector_float3 cv = { centerX, centerY, centerZ };
    vector_float3 uv = { upX, upY, upZ };
    vector_float3 n = simd_normalize(ev - cv);
    vector_float3 u = simd_normalize(simd_cross(uv, n));
    vector_float3 v = simd_cross(n, u);
    
    simd_float4x4 m = simd_matrix_from_rows(simd_make_float4(u, simd_dot(-u, ev)),
                                              simd_make_float4(v, simd_dot(-v, ev)),
                                              simd_make_float4(n, simd_dot(-n, ev)),
                                              (simd_float4){0,0,0,1});
    
    return m;
}

static inline simd_float4x4 SGMatrix4x4MakePerspective(float fovyRadians, float aspect, float nearZ, float farZ)
{
    float cotan = 1.0f / tan(fovyRadians / 2.0f);
    
    simd_float4x4 m = simd_matrix((simd_float4){cotan / aspect, 0.0f, 0.0f, 0.0f},
                     (simd_float4){0.0f, cotan, 0.0f, 0.0f},
                     (simd_float4){0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f},
                     (simd_float4){0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f});
    
    return m;
}

static inline simd_float4x4 SGMatrix4x4MakeXRotation(float radians)
{
    float cos = cos(radians);
    float sin = sin(radians);
    
    simd_float4x4 m = simd_matrix((simd_float4){ 1.0f, 0.0f, 0.0f, 0.0f},
        (simd_float4){0.0f, cos, sin, 0.0f},
        (simd_float4){0.0f, -sin, cos, 0.0f},
        (simd_float4){0.0f, 0.0f, 0.0f, 1.0f});
    
    return m;
}

static inline simd_float4x4 SGMatrix4x4MakeYRotation(float radians)
{
    float cos = cos(radians);
    float sin = sin(radians);
    
    simd_float4x4 m = simd_matrix((simd_float4){cos, 0.0f, -sin, 0.0f},
                     (simd_float4){0.0f, 1.0f, 0.0f, 0.0f},
                     (simd_float4){sin, 0.0f, cos, 0.0f},
                     (simd_float4){0.0f, 0.0f, 0.0f, 1.0f});
    
    return m;
}

static inline simd_float4x4 SGMatrix4x4MakeZRotation(float radians)
{
    float cos = cos(radians);
    float sin = sin(radians);
    
    simd_float4x4 m = simd_matrix((simd_float4){cos, sin, 0.0f, 0.0f},
        (simd_float4){-sin, cos, 0.0f, 0.0f},
        (simd_float4){0.0f, 0.0f, 1.0f, 0.0f},
        (simd_float4){0.0f, 0.0f, 0.0f, 1.0f});
    
    return m;
}

static inline simd_float4x4 SGMatrix4x4RotateX(simd_float4x4 matrix, float radians)
{
    simd_float4x4 rm = SGMatrix4x4MakeXRotation(radians);
    return matrix_multiply(matrix, rm);
}

static inline simd_float4x4 SGMatrix4x4RotateY(simd_float4x4 matrix, float radians)
{
    simd_float4x4 rm = SGMatrix4x4MakeYRotation(radians);
    return matrix_multiply(matrix, rm);
}

static inline simd_float4x4 SGMatrix4x4RotateZ(simd_float4x4 matrix, float radians)
{
    simd_float4x4 rm = SGMatrix4x4MakeZRotation(radians);
    return matrix_multiply(matrix, rm);
}

#endif /* SGVector_h */

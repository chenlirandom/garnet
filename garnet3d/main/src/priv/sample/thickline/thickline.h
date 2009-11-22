#pragma once

#include <d3d9.h>
#include <xnamath.h>

struct ThickLineVertex
{
    float    x, y, z;
    float    u, v;
    D3DCOLOR color;
};

struct ThickLineParameters
{
    XMMATRIX transformation; // matrix that transform vertex into clip space
    float    width; // line width in pixels in screen space
};

class D3D9ThickLineRenderer
{

public:

    D3D9ThickLineRenderer();
    ~D3D9ThickLineRenderer() { OnDeviceDispose(); OnDeviceDelete(); }

    bool OnDeviceCreate( IDirect3DDevice9 * dev );
    bool OnDeviceRestore();
    void OnDeviceDispose();
    void OnDeviceDelete();

    bool DrawBegin( const ThickLineParameters & parameters );
    void DrawEnd();

    void Line( const ThickLineVertex & v0, const ThickLineVertex & v1 );
    void Line( float x1, float y1, float z1, float x2, float y2, float z2, D3DCOLOR color );
    void LineList( const ThickLineVertex * vertices, size_t numverts );

private:

    struct PrivateParameters : public ThickLineParameters
    {
        float screenWidth, screenHeight; // screen size in pixels
        float endPointHalfWidth, endPointHalfHeight; // size of line end point in clip space.
    };

    struct PrivateVertex
    {
        XMVECTOR position; // position in clip space
        D3DCOLOR color;
        float    u, v;
        UInt32   _; // padding
    };

    struct EndPoint
    {
        float posl, posr, post, posb, posz, posw;
        float texl, texr, text, texb;
        D3DCOLOR color;

        PrivateVertex * TopLeft( PrivateVertex * v );
        PrivateVertex * TopRight( PrivateVertex * v );
        PrivateVertex * BottomLeft( PrivateVertex * v );
        PrivateVertex * BottomRight( PrivateVertex * v );
    };

private:

    IDirect3DDevice9 * m_Device;
    IDirect3DVertexShader9 * m_Vs;
    IDirect3DPixelShader9 * m_Ps;
    IDirect3DVertexDeclaration9 * m_Decl;
    IDirect3DVertexBuffer9 * m_Vb;
    IDirect3DIndexBuffer9 * m_Ib;

    bool m_Drawing;
    PrivateParameters m_Parameters;

    static const size_t MAX_VERTICES = 1024;
    PrivateVertex m_Vertices[MAX_VERTICES];
    UInt16        m_Indices[MAX_VERTICES/6*12]; // 12 indices (4 triangles ) per 6 verices
    UInt32        m_NumVertices;


private:

    void Clear();

    void CalcEndPoint(
        EndPoint              & endpoint,
        const ThickLineVertex & vertex );

    PrivateVertex * NewPolygon6();

    void Flush();
};

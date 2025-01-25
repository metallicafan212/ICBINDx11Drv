#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer CommonBuffer
// {
//
//   int bNoOpacity;                    // Offset:    0 Size:     4
//   int3 Pad3;                         // Offset:    4 Size:    12 [unused]
//
// }
//
// cbuffer FrameVariables
// {
//
//   float4x4 Proj;                     // Offset:    0 Size:    64
//   float Gamma;                       // Offset:   64 Size:     4 [unused]
//   float2 ViewSize;                   // Offset:   68 Size:     8 [unused]
//   int bDoSelection;                  // Offset:   76 Size:     4
//   int bOneXLightmaps;                // Offset:   80 Size:     4 [unused]
//   int bCorrectFog;                   // Offset:   84 Size:     4 [unused]
//   uint FrameShaderFlags;             // Offset:   88 Size:     4 [unused]
//   int GammaMode;                     // Offset:   92 Size:     4 [unused]
//   float HDRExpansion;                // Offset:   96 Size:     4 [unused]
//   float ResolutionScale;             // Offset:  100 Size:     4 [unused]
//   float WhiteLevel;                  // Offset:  104 Size:     4 [unused]
//   int RendMap;                       // Offset:  108 Size:     4 [unused]
//   float DepthDrawLimit;              // Offset:  112 Size:     4 [unused]
//   float GammaOffsetRed;              // Offset:  116 Size:     4 [unused]
//   float GammaOffsetBlue;             // Offset:  120 Size:     4 [unused]
//   float GammaOffsetGreen;            // Offset:  124 Size:     4 [unused]
//
// }
//
// cbuffer PolyflagVars
// {
//
//   int bSelected;                     // Offset:    0 Size:     4
//   float AlphaReject;                 // Offset:    4 Size:     4 [unused]
//   float AltAlphaReject;              // Offset:    8 Size:     4 [unused]
//   float BWPercent;                   // Offset:   12 Size:     4 [unused]
//   float3 SelectedColor;              // Offset:   16 Size:    12
//   float PadSelect;                   // Offset:   28 Size:     4 [unused]
//   uint ShaderFlags;                  // Offset:   32 Size:     4
//   float3 PadFlags;                   // Offset:   36 Size:    12 [unused]
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// FrameVariables                    cbuffer      NA          NA            cb0      1 
// PolyflagVars                      cbuffer      NA          NA            cb2      1 
// CommonBuffer                      cbuffer      NA          NA            cb4      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyzw        0     NONE   float   xyz 
// TEXCOORD                 0   xyzw        1     NONE   float   xy  
// COLOR                    0   xyzw        2     NONE   float   xyzw
// COLOR                    1   xyzw        3     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    2     z         1     NONE   float     z 
// COLOR                    0   xyzw        2     NONE   float   xyzw
// COLOR                    1   xyzw        3     NONE   float   xyzw
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB4[1], immediateIndexed
dcl_constantbuffer CB0[5], immediateIndexed
dcl_constantbuffer CB2[3], immediateIndexed
dcl_input v0.xyz
dcl_input v1.xy
dcl_input v2.xyzw
dcl_input v3.xyzw
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o1.z
dcl_output o2.xyzw
dcl_output o3.xyzw
dcl_temps 3
mov r0.xyz, v0.xyzx
mov r0.w, l(1.000000)
dp4 o0.x, r0.xyzw, cb0[0].xyzw
dp4 o0.y, r0.xyzw, cb0[1].xyzw
dp4 o0.z, r0.xyzw, cb0[2].xyzw
dp4 o0.w, r0.xyzw, cb0[3].xyzw
mov o1.xy, v1.xyxx
mov o1.z, v0.z
movc r0.w, cb4[0].x, l(1.000000), v2.w
and r1.x, l(2), cb2[2].x
mov r0.xyz, v2.xyzx
movc r2.xyzw, r1.xxxx, l(1.000000,1.000000,1.000000,1.000000), r0.xyzw
movc r1.xyzw, r1.xxxx, l(0,0,0,0), v3.xyzw
movc r1.xyzw, cb2[0].xxxx, l(0,0,0,0), r1.xyzw
movc o3.xyzw, cb0[4].wwww, l(0,0,0,0), r1.xyzw
movc r1.xyz, cb2[0].xxxx, cb2[1].xyzx, r2.xyzx
movc r1.w, cb2[0].x, l(1.000000), r2.w
movc o2.xyzw, cb0[4].wwww, r0.xyzw, r1.xyzw
ret 
// Approximately 19 instruction slots used
#endif

const BYTE g_VertShader[] =
{
     68,  88,  66,  67,  86,  58, 
     35,  85, 137,  90, 211,  64, 
    245,  52, 109, 126, 143, 113, 
    230,  12,   1,   0,   0,   0, 
    224,  12,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    232,   7,   0,   0, 112,   8, 
      0,   0,  20,   9,   0,   0, 
     68,  12,   0,   0,  82,  68, 
     69,  70, 172,   7,   0,   0, 
      3,   0,   0,   0, 200,   0, 
      0,   0,   3,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
    254, 255,   0,   1,   0,   0, 
    131,   7,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    156,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 171,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 184,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     70, 114,  97, 109, 101,  86, 
     97, 114, 105,  97,  98, 108, 
    101, 115,   0,  80, 111, 108, 
    121, 102, 108,  97, 103,  86, 
     97, 114, 115,   0,  67, 111, 
    109, 109, 111, 110,  66, 117, 
    102, 102, 101, 114,   0, 171, 
    171, 171, 184,   0,   0,   0, 
      2,   0,   0,   0,  16,   1, 
      0,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 156,   0,   0,   0, 
     16,   0,   0,   0, 196,   1, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 171,   0,   0,   0, 
      8,   0,   0,   0, 188,   5, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  96,   1,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    112,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    148,   1,   0,   0,   4,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0, 160,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  98,  78, 
    111,  79, 112,  97,  99, 105, 
    116, 121,   0, 105, 110, 116, 
      0, 171,   0,   0,   2,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 107,   1, 
      0,   0,  80,  97, 100,  51, 
      0, 105, 110, 116,  51,   0, 
    171, 171,   1,   0,   2,   0, 
      1,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 153,   1, 
      0,   0,  68,   4,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
     84,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    120,   4,   0,   0,  64,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 132,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 168,   4, 
      0,   0,  68,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0, 184,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 220,   4,   0,   0, 
     76,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    112,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    233,   4,   0,   0,  80,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 112,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 248,   4, 
      0,   0,  84,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 112,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   4,   5,   0,   0, 
     88,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     28,   5,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     64,   5,   0,   0,  92,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 112,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  74,   5, 
      0,   0,  96,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  87,   5,   0,   0, 
    100,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    132,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    103,   5,   0,   0, 104,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 132,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 114,   5, 
      0,   0, 108,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 112,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 122,   5,   0,   0, 
    112,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    132,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    137,   5,   0,   0, 116,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 132,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 152,   5, 
      0,   0, 120,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 168,   5,   0,   0, 
    124,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    132,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     80, 114, 111, 106,   0, 102, 
    108, 111,  97, 116,  52, 120, 
     52,   0, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     73,   4,   0,   0,  71,  97, 
    109, 109,  97,   0, 102, 108, 
    111,  97, 116,   0,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    126,   4,   0,   0,  86, 105, 
    101, 119,  83, 105, 122, 101, 
      0, 102, 108, 111,  97, 116, 
     50,   0,   1,   0,   3,   0, 
      1,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 177,   4, 
      0,   0,  98,  68, 111,  83, 
    101, 108, 101,  99, 116, 105, 
    111, 110,   0,  98,  79, 110, 
    101,  88,  76, 105, 103, 104, 
    116, 109,  97, 112, 115,   0, 
     98,  67, 111, 114, 114, 101, 
     99, 116,  70, 111, 103,   0, 
     70, 114,  97, 109, 101,  83, 
    104,  97, 100, 101, 114,  70, 
    108,  97, 103, 115,   0, 100, 
    119, 111, 114, 100,   0, 171, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  21,   5,   0,   0, 
     71,  97, 109, 109,  97,  77, 
    111, 100, 101,   0,  72,  68, 
     82,  69, 120, 112,  97, 110, 
    115, 105, 111, 110,   0,  82, 
    101, 115, 111, 108, 117, 116, 
    105, 111, 110,  83,  99,  97, 
    108, 101,   0,  87, 104, 105, 
    116, 101,  76, 101, 118, 101, 
    108,   0,  82, 101, 110, 100, 
     77,  97, 112,   0,  68, 101, 
    112, 116, 104,  68, 114,  97, 
    119,  76, 105, 109, 105, 116, 
      0,  71,  97, 109, 109,  97, 
     79, 102, 102, 115, 101, 116, 
     82, 101, 100,   0,  71,  97, 
    109, 109,  97,  79, 102, 102, 
    115, 101, 116,  66, 108, 117, 
    101,   0,  71,  97, 109, 109, 
     97,  79, 102, 102, 115, 101, 
    116,  71, 114, 101, 101, 110, 
      0, 171, 171, 171, 252,   6, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 112,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   6,   7,   0,   0, 
      4,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    132,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     18,   7,   0,   0,   8,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 132,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  33,   7, 
      0,   0,  12,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  43,   7,   0,   0, 
     16,   0,   0,   0,  12,   0, 
      0,   0,   2,   0,   0,   0, 
     64,   7,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    100,   7,   0,   0,  28,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 132,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 110,   7, 
      0,   0,  32,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  28,   5,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 122,   7,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
     64,   7,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     98,  83, 101, 108, 101,  99, 
    116, 101, 100,   0,  65, 108, 
    112, 104,  97,  82, 101, 106, 
    101,  99, 116,   0,  65, 108, 
    116,  65, 108, 112, 104,  97, 
     82, 101, 106, 101,  99, 116, 
      0,  66,  87,  80, 101, 114, 
     99, 101, 110, 116,   0,  83, 
    101, 108, 101,  99, 116, 101, 
    100,  67, 111, 108, 111, 114, 
      0, 102, 108, 111,  97, 116, 
     51,   0,   1,   0,   3,   0, 
      1,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  57,   7, 
      0,   0,  80,  97, 100,  83, 
    101, 108, 101,  99, 116,   0, 
     83, 104,  97, 100, 101, 114, 
     70, 108,  97, 103, 115,   0, 
     80,  97, 100,  70, 108,  97, 
    103, 115,   0,  77, 105,  99, 
    114, 111, 115, 111, 102, 116, 
     32,  40,  82,  41,  32,  72, 
     76,  83,  76,  32,  83, 104, 
     97, 100, 101, 114,  32,  67, 
    111, 109, 112, 105, 108, 101, 
    114,  32,  49,  48,  46,  49, 
      0, 171,  73,  83,  71,  78, 
    128,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   7,   0,   0, 
    113,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   3,   0,   0, 
    122,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  15,  15,   0,   0, 
    122,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,  15,   0,   0, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  84,  69,  88, 
     67,  79,  79,  82,  68,   0, 
     67,  79,  76,  79,  82,   0, 
     79,  83,  71,  78, 156,   0, 
      0,   0,   5,   0,   0,   0, 
      8,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 140,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,  12,   0,   0, 149,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      4,  11,   0,   0, 149,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0, 149,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0, 171,  83,  72,  69,  88, 
     40,   3,   0,   0,  80,   0, 
      1,   0, 202,   0,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      4,   0,   0,   0,   1,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   2,   0,   0,   0, 
      3,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      2,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      3,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
     66,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   3,   0, 
      0,   0, 104,   0,   0,   2, 
      3,   0,   0,   0,  54,   0, 
      0,   5, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,  18, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  17,   0,   0,   8, 
     18,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  17,   0, 
      0,   8,  34,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     17,   0,   0,   8,  66,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  17,   0,   0,   8, 
    130,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5,  50,  32,  16,   0, 
      1,   0,   0,   0,  70,  16, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5,  66,  32, 
     16,   0,   1,   0,   0,   0, 
     42,  16,  16,   0,   0,   0, 
      0,   0,  55,   0,   0,  10, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  32,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  58,  16, 
     16,   0,   2,   0,   0,   0, 
      1,   0,   0,   8,  18,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   2,   0, 
      0,   0,  10, 128,  32,   0, 
      2,   0,   0,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  55,   0, 
      0,  12, 242,   0,  16,   0, 
      2,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,  70,  14,  16,   0, 
      0,   0,   0,   0,  55,   0, 
      0,  12, 242,   0,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,  13, 242,   0,  16,   0, 
      1,   0,   0,   0,   6, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,  13, 
    242,  32,  16,   0,   3,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     55,   0,   0,  11, 114,   0, 
     16,   0,   1,   0,   0,   0, 
      6, 128,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
     70, 130,  32,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  55,   0,   0,  10, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  10, 128,  32,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  58,   0, 
     16,   0,   2,   0,   0,   0, 
     55,   0,   0,  10, 242,  32, 
     16,   0,   2,   0,   0,   0, 
    246, 143,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,  19,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};

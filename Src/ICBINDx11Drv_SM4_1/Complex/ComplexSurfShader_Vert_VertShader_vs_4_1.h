#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer FrameVariables
// {
//
//   float4x4 Proj;                     // Offset:    0 Size:    64
//   float Gamma;                       // Offset:   64 Size:     4 [unused]
//   float2 ViewSize;                   // Offset:   68 Size:     8 [unused]
//   int bDoSelection;                  // Offset:   76 Size:     4 [unused]
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
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// FrameVariables                    cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyzw        0     NONE   float   xyz 
// TEXCOORD                 0   xyzw        1     NONE   float   xyzw
// COLOR                    0   xyzw        2     NONE   float   xyzw
// COLOR                    1   xyzw        3     NONE   float   xyzw
// TEXCOORD                 1   xyzw        4     NONE   float   xyzw
// TEXCOORD                 2   xyzw        5     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
// TEXCOORD                 1     zw        1     NONE   float     zw
// TEXCOORD                 2   xy          2     NONE   float   xy  
// TEXCOORD                 3     zw        2     NONE   float     zw
// TEXCOORD                 4   xyzw        3     NONE   float   xyzw
// COLOR                    0   xyzw        4     NONE   float   xyzw
// COLOR                    1   xyzw        5     NONE   float   xyzw
// COLOR                    2   x           6     NONE   float   x   
// COLOR                    3    yz         6     NONE   float    yz 
//
vs_4_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[4], immediateIndexed
dcl_input v0.xyz
dcl_input v1.xyzw
dcl_input v2.xyzw
dcl_input v3.xyzw
dcl_input v4.xyzw
dcl_input v5.xy
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o1.zw
dcl_output o2.xy
dcl_output o2.zw
dcl_output o3.xyzw
dcl_output o4.xyzw
dcl_output o5.xyzw
dcl_output o6.x
dcl_output o6.yz
dcl_temps 1
mov r0.xyz, v0.xyzx
mov r0.w, l(1.000000)
dp4 o0.x, r0.xyzw, cb0[0].xyzw
dp4 o0.y, r0.xyzw, cb0[1].xyzw
dp4 o0.z, r0.xyzw, cb0[2].xyzw
dp4 o0.w, r0.xyzw, cb0[3].xyzw
mov o1.xyzw, v1.xyzw
mov o2.xy, v4.zwzz
mov o2.zw, v5.xxxy
mov o3.xy, v4.xyxx
mov r0.z, v0.z
mov r0.w, l(0)
mov o3.zw, r0.zzzw
mov o6.xyz, r0.zwwz
mad o4.xyz, v3.xyzx, v3.wwww, v2.xyzx
mov o4.w, v2.w
mov o5.xyzw, l(0,0,0,0)
ret 
// Approximately 18 instruction slots used
#endif

const BYTE g_VertShader[] =
{
     68,  88,  66,  67,  64,  95, 
    228,   3,  27, 114, 170, 208, 
     64,  34, 112,  39, 126, 147, 
    176, 114,   1,   0,   0,   0, 
     80,   8,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    108,   3,   0,   0,  36,   4, 
      0,   0,  64,   5,   0,   0, 
    212,   7,   0,   0,  82,  68, 
     69,  70,  48,   3,   0,   0, 
      1,   0,   0,   0,  76,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   1,   4, 
    254, 255,   0,   1,   0,   0, 
      5,   3,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     70, 114,  97, 109, 101,  86, 
     97, 114, 105,  97,  98, 108, 
    101, 115,   0, 171,  60,   0, 
      0,   0,  16,   0,   0,   0, 
    100,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 228,   1, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   2,   0, 
      0,   0, 236,   1,   0,   0, 
      0,   0,   0,   0, 252,   1, 
      0,   0,  64,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0,  20,   2, 
      0,   0,  68,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,  32,   2,   0,   0, 
      0,   0,   0,   0,  48,   2, 
      0,   0,  76,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0,  80,   2, 
      0,   0,  80,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0,  95,   2, 
      0,   0,  84,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0, 107,   2, 
      0,   0,  88,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 124,   2,   0,   0, 
      0,   0,   0,   0, 140,   2, 
      0,   0,  92,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0, 150,   2, 
      0,   0,  96,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 163,   2, 
      0,   0, 100,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 179,   2, 
      0,   0, 104,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 190,   2, 
      0,   0, 108,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0, 198,   2, 
      0,   0, 112,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 213,   2, 
      0,   0, 116,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 228,   2, 
      0,   0, 120,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 244,   2, 
      0,   0, 124,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0,  80, 114, 
    111, 106,   0, 171, 171, 171, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  71,  97, 
    109, 109,  97,   0, 171, 171, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  86, 105, 
    101, 119,  83, 105, 122, 101, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  98,  68, 111,  83, 
    101, 108, 101,  99, 116, 105, 
    111, 110,   0, 171, 171, 171, 
      0,   0,   2,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  98,  79, 
    110, 101,  88,  76, 105, 103, 
    104, 116, 109,  97, 112, 115, 
      0,  98,  67, 111, 114, 114, 
    101,  99, 116,  70, 111, 103, 
      0,  70, 114,  97, 109, 101, 
     83, 104,  97, 100, 101, 114, 
     70, 108,  97, 103, 115,   0, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  71,  97, 
    109, 109,  97,  77, 111, 100, 
    101,   0,  72,  68,  82,  69, 
    120, 112,  97, 110, 115, 105, 
    111, 110,   0,  82, 101, 115, 
    111, 108, 117, 116, 105, 111, 
    110,  83,  99,  97, 108, 101, 
      0,  87, 104, 105, 116, 101, 
     76, 101, 118, 101, 108,   0, 
     82, 101, 110, 100,  77,  97, 
    112,   0,  68, 101, 112, 116, 
    104,  68, 114,  97, 119,  76, 
    105, 109, 105, 116,   0,  71, 
     97, 109, 109,  97,  79, 102, 
    102, 115, 101, 116,  82, 101, 
    100,   0,  71,  97, 109, 109, 
     97,  79, 102, 102, 115, 101, 
    116,  66, 108, 117, 101,   0, 
     71,  97, 109, 109,  97,  79, 
    102, 102, 115, 101, 116,  71, 
    114, 101, 101, 110,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171, 171, 171, 
     73,  83,  71,  78, 176,   0, 
      0,   0,   6,   0,   0,   0, 
      8,   0,   0,   0, 152,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   7,   0,   0, 161,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0, 170,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,  15,   0,   0, 170,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,  15,   0,   0, 161,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
     15,  15,   0,   0, 161,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     15,   3,   0,   0,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0,  67,  79, 
     76,  79,  82,   0,  79,  83, 
     71,  78,  20,   1,   0,   0, 
     10,   0,   0,   0,   8,   0, 
      0,   0, 248,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,   4,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,  12, 
      0,   0,   4,   1,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  12,   3, 
      0,   0,   4,   1,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   3,  12, 
      0,   0,   4,   1,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  12,   3, 
      0,   0,   4,   1,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,  15,   0, 
      0,   0,  13,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,  15,   0, 
      0,   0,  13,   1,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,  15,   0, 
      0,   0,  13,   1,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,   1,  14, 
      0,   0,  13,   1,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,   6,   9, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  67, 
     79,  76,  79,  82,   0, 171, 
     83,  72,  68,  82, 140,   2, 
      0,   0,  65,   0,   1,   0, 
    163,   0,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   2,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   3,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   4,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   5,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 194,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 194,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      3,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      4,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      5,   0,   0,   0, 101,   0, 
      0,   3,  18,  32,  16,   0, 
      6,   0,   0,   0, 101,   0, 
      0,   3,  98,  32,  16,   0, 
      6,   0,   0,   0, 104,   0, 
      0,   2,   1,   0,   0,   0, 
     54,   0,   0,   5, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  18,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  17,   0, 
      0,   8,  18,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     17,   0,   0,   8,  34,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  17,   0,   0,   8, 
     66,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  17,   0, 
      0,   8, 130,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     50,  32,  16,   0,   2,   0, 
      0,   0, 230,  26,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5, 194,  32,  16,   0, 
      2,   0,   0,   0,   6,  20, 
     16,   0,   5,   0,   0,   0, 
     54,   0,   0,   5,  50,  32, 
     16,   0,   3,   0,   0,   0, 
     70,  16,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  42,  16,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 194,  32, 
     16,   0,   3,   0,   0,   0, 
    166,  14,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    114,  32,  16,   0,   6,   0, 
      0,   0, 230,  11,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 114,  32,  16,   0, 
      4,   0,   0,   0,  70,  18, 
     16,   0,   3,   0,   0,   0, 
    246,  31,  16,   0,   3,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      4,   0,   0,   0,  58,  16, 
     16,   0,   2,   0,   0,   0, 
     54,   0,   0,   8, 242,  32, 
     16,   0,   5,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  18,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};

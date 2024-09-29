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
//   float4x4 Proj;                     // Offset:    0 Size:    64 [unused]
//   float Gamma;                       // Offset:   64 Size:     4 [unused]
//   float2 ViewSize;                   // Offset:   68 Size:     8
//   int bDoSelection;                  // Offset:   76 Size:     4 [unused]
//   int bOneXLightmaps;                // Offset:   80 Size:     4 [unused]
//   int bCorrectFog;                   // Offset:   84 Size:     4 [unused]
//   int bHDR;                          // Offset:   88 Size:     4 [unused]
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
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    1     z         1     NONE   float     z 
// COLOR                    2      w        1     NONE   float      w
// COLOR                    0   xyzw        2     NONE   float   xyzw
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
gs_4_0
dcl_constantbuffer CB0[5], immediateIndexed
dcl_input_siv v[2][0].xyzw, position
dcl_input v[2][1].xy
dcl_input v[2][1].z
dcl_input v[2][1].w
dcl_input v[2][2].xyzw
dcl_temps 6
dcl_inputprimitive line 
dcl_outputtopology trianglestrip 
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o1.z
dcl_output o2.xyzw
dcl_output o3.xyzw
dcl_maxout 6
add r0.x, v[0][0].w, v[0][0].z
add r0.y, v[1][0].w, v[1][0].z
lt r0.z, r0.x, l(0.000000)
if_nz r0.z
  lt r0.z, r0.y, l(0.000000)
  if_nz r0.z
    ret 
  endif 
  add r0.z, -r0.x, r0.y
  div r0.z, -r0.x, r0.z
  add r1.xyzw, -v[0][0].xyzw, v[1][0].xyzw
  mad r1.xyzw, r0.zzzz, r1.xyzw, v[0][0].xyzw
else 
  mov r1.xyzw, v[0][0].xyzw
endif 
lt r0.z, r0.y, l(0.000000)
add r0.y, -r0.x, r0.y
div r0.x, -r0.x, r0.y
add r2.xyzw, -r1.xyzw, v[1][0].xyzw
mad r2.xyzw, r0.xxxx, r2.xyzw, r1.xyzw
movc r0.xyzw, r0.zzzz, r2.xyzw, v[1][0].xyzw
div r2.xy, r1.xyxx, r1.wwww
div r2.zw, r0.xxxy, r0.wwww
add r2.xy, -r2.xyxx, r2.zwzz
mul r2.xy, r2.xyxx, cb0[4].yzyy
dp2 r2.z, r2.xyxx, r2.xyxx
rsq r2.z, r2.z
mul r2.xy, r2.zzzz, r2.xyxx
mov r2.z, -r2.y
div r2.xyzw, r2.xyzx, cb0[4].yzyz
mul r3.xy, r2.xyxx, v[0][1].zzzz
mov r3.zw, l(0,0,0,0)
mul r2.xy, r2.zwzz, v[0][1].zzzz
mov r2.zw, l(0,0,0,0)
add r4.xyzw, -r3.xyww, r2.xyww
mad r5.xyzw, r4.xyww, r1.wwww, r1.xyzw
add r2.xyzw, r3.xyzw, r2.xyzw
mad r3.xyzw, r2.xyww, r0.wwww, r0.xyzw
mad r0.xyzw, -r4.xyzw, r0.wwww, r0.xyzw
mad r1.xyzw, -r2.xyzw, r1.wwww, r1.xyzw
mov o0.xyzw, r5.xyzw
mov o1.xyz, v[0][1].xywx
mov o2.xyzw, v[0][2].xyzw
mov o3.xyzw, l(0,0,0,0)
emit 
mov o0.xyzw, r1.xyzw
mov o1.xyz, v[0][1].xywx
mov o2.xyzw, v[0][2].xyzw
mov o3.xyzw, l(0,0,0,0)
emit 
mov o0.xyzw, r3.xyzw
mov o1.xy, v[0][1].xyxx
mov o1.z, v[1][1].w
mov o2.xyzw, v[0][2].xyzw
mov o3.xyzw, l(0,0,0,0)
emit 
mov o0.xyzw, r0.xyzw
mov o1.xy, v[0][1].xyxx
mov o1.z, v[1][1].w
mov o2.xyzw, v[0][2].xyzw
mov o3.xyzw, l(0,0,0,0)
emit 
cut 
ret 
// Approximately 64 instruction slots used
#endif

const BYTE g_GeoShader[] =
{
     68,  88,  66,  67, 118, 201, 
    120,  48,  70, 163, 148, 251, 
    226, 220, 244, 143, 129, 230, 
     50, 194,   1,   0,   0,   0, 
     36,  12,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     80,   3,   0,   0, 244,   3, 
      0,   0, 152,   4,   0,   0, 
    168,  11,   0,   0,  82,  68, 
     69,  70,  20,   3,   0,   0, 
      1,   0,   0,   0,  76,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
     83,  71,   0,   1,   0,   0, 
    233,   2,   0,   0,  60,   0, 
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
     64,   0,   0,   0,   0,   0, 
      0,   0, 236,   1,   0,   0, 
      0,   0,   0,   0, 252,   1, 
      0,   0,  64,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0,  20,   2, 
      0,   0,  68,   0,   0,   0, 
      8,   0,   0,   0,   2,   0, 
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
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0, 112,   2, 
      0,   0,  92,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0, 122,   2, 
      0,   0,  96,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 135,   2, 
      0,   0, 100,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 151,   2, 
      0,   0, 104,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 162,   2, 
      0,   0, 108,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0, 170,   2, 
      0,   0, 112,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 185,   2, 
      0,   0, 116,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 200,   2, 
      0,   0, 120,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   4,   2,   0,   0, 
      0,   0,   0,   0, 216,   2, 
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
      0,  98,  72,  68,  82,   0, 
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
      0,  77, 105,  99, 114, 111, 
    115, 111, 102, 116,  32,  40, 
     82,  41,  32,  72,  76,  83, 
     76,  32,  83, 104,  97, 100, 
    101, 114,  32,  67, 111, 109, 
    112, 105, 108, 101, 114,  32, 
     49,  48,  46,  49,   0, 171, 
    171, 171,  73,  83,  71,  78, 
    156,   0,   0,   0,   5,   0, 
      0,   0,   8,   0,   0,   0, 
    128,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,  15,   0,   0, 
    140,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   3,   3,   0,   0, 
    149,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   4,   4,   0,   0, 
    149,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   8,   8,   0,   0, 
    149,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  15,  15,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0,  67,  79,  76, 
     79,  82,   0, 171,  79,  83, 
     71,  78, 156,   0,   0,   0, 
      5,   0,   0,   0,   8,   0, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0, 140,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,  12, 
      0,   0, 149,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   4,  11, 
      0,   0, 149,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  15,   0, 
      0,   0, 149,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  67, 
     79,  76,  79,  82,   0, 171, 
     83,  72,  68,  82,   8,   7, 
      0,   0,  64,   0,   2,   0, 
    194,   1,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  97,   0,   0,   5, 
    242,  16,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   4,  50,  16,  32,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   0,  95,   0,   0,   4, 
     66,  16,  32,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
     95,   0,   0,   4, 130,  16, 
     32,   0,   2,   0,   0,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   4, 242,  16,  32,   0, 
      2,   0,   0,   0,   2,   0, 
      0,   0, 104,   0,   0,   2, 
      6,   0,   0,   0,  93,  16, 
      0,   1,  92,  40,   0,   1, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  66,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      3,   0,   0,   0,  94,   0, 
      0,   2,   6,   0,   0,   0, 
      0,   0,   0,   9,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     58,  16,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     42,  16,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   9,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     58,  16,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     42,  16,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     49,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  31,   0, 
      4,   3,  42,   0,  16,   0, 
      0,   0,   0,   0,  49,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     21,   0,   0,   1,   0,   0, 
      0,   8,  66,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     14,   0,   0,   8,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,  10, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  30,  32, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  30,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
    166,  10,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70,  30, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  18,   0, 
      0,   1,  54,   0,   0,   6, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  30,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  21,   0,   0,   1, 
     49,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   8,  34,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     14,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  70,  14,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  70,  30,  32,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,  10, 
    242,   0,  16,   0,   0,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     70,  30,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   7,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  14,   0, 
      0,   7, 194,   0,  16,   0, 
      2,   0,   0,   0,   6,   4, 
     16,   0,   0,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
     50,   0,  16,   0,   2,   0, 
      0,   0,  70,   0,  16, 128, 
     65,   0,   0,   0,   2,   0, 
      0,   0, 230,  10,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   8,  50,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
    150, 133,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     15,   0,   0,   7,  66,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      2,   0,   0,   0,  68,   0, 
      0,   5,  66,   0,  16,   0, 
      2,   0,   0,   0,  42,   0, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7,  50,   0, 
     16,   0,   2,   0,   0,   0, 
    166,  10,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   6,  66,   0,  16,   0, 
      2,   0,   0,   0,  26,   0, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  14,   0, 
      0,   8, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
    150, 137,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     56,   0,   0,   8,  50,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0, 166,  26,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  54,   0,   0,   8, 
    194,   0,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   8,  50,   0,  16,   0, 
      2,   0,   0,   0, 230,  10, 
     16,   0,   2,   0,   0,   0, 
    166,  26,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     54,   0,   0,   8, 194,   0, 
     16,   0,   2,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  70,  15,  16, 128, 
     65,   0,   0,   0,   3,   0, 
      0,   0,  70,  15,  16,   0, 
      2,   0,   0,   0,  50,   0, 
      0,   9, 242,   0,  16,   0, 
      5,   0,   0,   0,  70,  15, 
     16,   0,   4,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  70,  15,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16, 128, 
     65,   0,   0,   0,   4,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     54,   0,   0,   6, 114,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  19,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     54,   0,   0,   6, 242,  32, 
     16,   0,   2,   0,   0,   0, 
     70,  30,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     54,   0,   0,   8, 242,  32, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  19,   0,   0,   1, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   6, 
    114,  32,  16,   0,   1,   0, 
      0,   0,  70,  19,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  54,   0,   0,   6, 
    242,  32,  16,   0,   2,   0, 
      0,   0,  70,  30,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  54,   0,   0,   8, 
    242,  32,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  19,   0, 
      0,   1,  54,   0,   0,   5, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   6,  50,  32,  16,   0, 
      1,   0,   0,   0,  70,  16, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   6,  66,  32,  16,   0, 
      1,   0,   0,   0,  58,  16, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   6, 242,  32,  16,   0, 
      2,   0,   0,   0,  70,  30, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   8, 242,  32,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     19,   0,   0,   1,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   6,  50,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  16,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     54,   0,   0,   6,  66,  32, 
     16,   0,   1,   0,   0,   0, 
     58,  16,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     54,   0,   0,   6, 242,  32, 
     16,   0,   2,   0,   0,   0, 
     70,  30,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     54,   0,   0,   8, 242,  32, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  19,   0,   0,   1, 
      9,   0,   0,   1,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,  64,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
      0,   0,  30,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};

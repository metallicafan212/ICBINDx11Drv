CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
# Metallicafan212: TODO! This should support multiple projects....
PROJECT(HP2)

include("../configure.cmake")


SET(
    ICBINDDRV_Inc
	
	Inc/GammaModes.h
	Inc/HP2ShaderDefs.h
	Inc/HP2Shaders.h
	Inc/ICBINDx11Drv.h
	Inc/ShaderFlags.h
	Inc/UnD3DShader.h
	Inc/UnShaderManager.h
	Inc/UnTexCache.h
	Inc/WindowsVersions.h
)

SET(
    ICBINDDRV_Robin_Map
	
	Inc/Robin-Map/robin_growth_policy.h
	Inc/Robin-Map/robin_hash.h
	Inc/Robin-Map/robin_map.h
	Inc/Robin-Map/robin_set.h
)

SET(
    ICBINDDRV_Src
	
    Src/ResScalingShader.cpp
	Src/Un227.cpp
	Src/UnComError.cpp
	Src/UnComplexSurfShader.cpp
	Src/UnComputeShader.cpp
	Src/UnConfig.cpp
	Src/UnGenericShader.cpp
	Src/UnHardcodedShaders.cpp
	Src/UnHitTesting.cpp
	Src/UnHPCustom.cpp
	Src/UnICBINDDx11Drv.cpp
	Src/UnLines.cpp
	Src/UnLineShader.cpp
	Src/UnMeshShader.cpp
	Src/UnMSAAShader.cpp
	Src/UnP8ToRGBAShader.cpp
	Src/UnShader.cpp
	Src/UnShaderManager.cpp
	Src/UnSurf.cpp
	Src/UnTexCache.cpp
	Src/UnTexture.cpp
	Src/UnTiles.cpp
	Src/UnTileShader.cpp
	Src/UnTriangles.cpp
	Src/WindowsHelper.cpp
)

if (WIN32)
    add_library(ICBINDx11Drv ${ICBINDDRV_Inc} ${ICBINDDRV_Robin_Map} ${ICBINDDRV_Src})
    set_target_properties(ICBINDx11Drv PROPERTIES
        FOLDER ICBINDx11Drv
    )

    target_link_libraries(ICBINDx11Drv CoreLib EngineLib EditorLib RenderLib MWindowLib)
    target_link_libraries(ICBINDx11Drv advapi32.lib D3D11.lib D3DCompiler.lib Dwrite.lib D2d1.lib)
    unreal_package_configure(ICBINDx11Drv ICBINDx11Drv)
endif()

source_group("Inc" 				FILES ${ICBINDDRV_Inc})
source_group("Inc/Robin-Map"	FILES ${ICBINDDRV_Robin_Map})
source_group("Src"				FILES ${ICBINDDRV_Src})

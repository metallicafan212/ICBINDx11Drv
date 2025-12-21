CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
# Metallicafan212: TODO! This should support multiple projects....
PROJECT(HP2)

include("../configure.cmake")

# Metallicafan212: Compile options
option(ICBIND_HARDCODED_SHADERS "Hardcode and limit shaders to pre-compiled UT99 definitions. This should be used for any base UE1 game as well." OFF)
option(ICBIND_227_SHADERS 		"Use Unreal 227 definitions in the shader files." OFF)

# Metallicafan212: Vertex shaders
# 				   These all use the same entrypoint of VertShader
SET(
	ICBINDDRV_VERTEX_SHADERS
	Shaders/Complex/ComplexSurfShader_Vert.hlsl
	Shaders/General/GeneralShader_Vert.hlsl
	Shaders/Line/LineShader_Vert.hlsl
	Shaders/Mesh/MeshShader_Vert.hlsl
	Shaders/PostFX/ResScaling_Vert.hlsl
	Shaders/Tile/TileShader_Vert.hlsl
)

# Metallicafan212: Geometry shaders
#				   These all use the same entrypoint of GeoShader
SET(
	ICBINDDRV_GEOMETRY_SHADERS
	Shaders/Line/Line_GeoShader.hlsl
)

# Metallicafan212: Pixel shaders
#				   These all use the same entrypoint of PxShader
SET(
	ICBINDDRV_PIXEL_SHADERS
	Shaders/Complex/ComplexSurfShader_PX.hlsl
	Shaders/General/GeneralShader_PX.hlsl
	Shaders/Line/LineShader_PX.hlsl
	Shaders/Mesh/MeshShader_PX.hlsl
	Shaders/PostFX/ResScaling_PX.hlsl
	Shaders/Tile/TileShader_PX.hlsl
)

# Metallicafan212: Shader include files
SET(
	ICBINDDRV_SHADER_HEADERS
	
	Shaders/Complex/ComplexSurfShader_Common.h
	Shaders/General/GeneralShader_Common.h
	Shaders/Line/Line_Common.h
	Shaders/Mesh/MeshShader_Common.h
	Shaders/PostFX/ResScaling_Common.h
	Shaders/Tile/TileShader_Common.h
	
	# Metallicafan212: Common include header
	Shaders/ShaderGlobals.h
)

# Metallicafan212: Decided to KISS this, this would've still worked
# Metallicafan212: Now combine each shader list into 3 lists
#				   Vertex, Geometry, and Pixel
#LIST(APPEND ICBIND_VERTEX_SHADERS 		${ICBINDDRV_COMPLEX_VERT} ${ICBINDDRV_GENERAL_VERT} ${ICBINDDRV_LINE_VERT} ${ICBINDDRV_MESH_VERT} ${ICBINDDRV_POSTFX_VERT} ${ICBINDDRV_TILE_VERT})
#LIST(APPEND ICBIND_GEOMETRY_SHADERS  	${ICBINDDRV_LINE_GEO})
#LIST(APPEND ICBIND_PIXEL_SHADERS 		${ICBINDDRV_COMPLEX_PX} ${ICBINDDRV_GENERAL_PX} ${ICBINDDRV_LINE_PX} ${ICBINDDRV_MESH_PX} ${ICBINDDRV_POSTFX_PX} ${ICBINDDRV_TILE_PX})

# Metallicafan212: This fucking sucks. CMake, what the fuck is wrong with you? Why would you do this to me?
#				   Semicolon is the fucking LIST SEPARATOR!!
#				   So, until I fucking figure this out, we have to define a macro
#				   Fucking rotten
#SET(SEMICOLON \;)
#SET(SEMICOLON "FUCK_CMAKE")

# Metallicafan212: Lists for includes and function code, so we load the hard-coded shaders
SET(
	HARDCODED_BEGINNING
	"// Metallicafan212:\tThis file contains auto generated code to load hardcoded shaders (compiled from fxc.exe)"
	"\n"
	"#include \"ICBINDx11Drv.h\"" 
	"\n" 
	"\n"  
	"#ifdef DX11_HP2" 
	"\n" 
	"#include \"HP2Shaders.h\"" 
	"\n" 
	"#endif" 
	"\n"  
	"\n"
	"#define CAT(x, y) x##y" 
	"\n" 
	"\n"
	"#define SHADER_STR_INT(x) #x" 
	"\n"
	"\n"
	"#define SHADER_STR(x) SHADER_STR_INT(x)" 
	"\n"
	"\n"
	"#define SHADER_EXPAND(x, y) SHADER_STR(CAT(x, y))" 
	"\n"
	"\n"
)

SET(HARDCODED_FUNCTION
	"void FShaderManager::LoadHardcodedShaders()" 
	"\n" 
	"{" 
	"\n\t"
	"guard(FShaderManager::LoadHardcodedShaders)~" 
	"\n\t" 
	"\n\t" 
	"TArray<BYTE>* Shad = nullptr~" 
	"\n\t" 
	"\n\t"
)

SET(HARDCODED_END
	"\n\t" 
	"unguard~" 
	"\n" 
	"}"
)

# Metallicafan212: If UnHardcodedShaders.cpp doesn't exist, create it now as a blank file so that CMake doesn't error out
#if(NOT EXISTS "Src/UnHardcodedShaders.cpp")
file(WRITE "Src/UnHardcodedShaders.cpp" "")
#endif()

# Metallicafan212: Shader folder variable
SET(SHADERFOLDER "SHADER_FOLDER ")


# Metallicafan212: The function code is this very simple block
#void FShaderManager::LoadHardcodedShaders()
#{
#	guard(FShaderManager::LoadHardcodedShaders);
#
#	// Metallicafan212:	Set the specific slots
#	TArray<BYTE>* Shad = nullptr;
#	Shad = &Bytecode.Set(TEXT("Blah"), TArray<BYTE>());
#
#	// Metallicafan212:	Now copy in
#	Shad->Add(ARRAY_COUNT(Bytes));
#	appMemcpy(Shad->GetData(), Bytes, ARRAY_COUNT(Bytes));
#
#	unguard;
#}

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
    add_library(ICBINDx11Drv ${ICBINDDRV_Inc} ${ICBINDDRV_Robin_Map} ${ICBINDDRV_Src})#${ICBINDDRV_VERTEX_SHADERS} ${ICBINDDRV_PIXEL_SHADERS} ${ICBINDDRV_SHADER_HEADERS})
    set_target_properties(ICBINDx11Drv PROPERTIES
        FOLDER ICBINDx11Drv
    )

    target_link_libraries(ICBINDx11Drv CoreLib EngineLib EditorLib Render MWindow)
    target_link_libraries(ICBINDx11Drv advapi32.lib D3D11.lib D3DCompiler.lib Dwrite.lib D2d1.lib)
    unreal_package_configure(ICBINDx11Drv ICBINDx11Drv)
	
	function(WriteShaderLine Line)
		# Metallicafan212: Replace ~ with ; as CMake doesn't allow semicolons in lists.....
		STRING(REPLACE "~" ";" Line_Fixed ${Line})
		file(APPEND "Src/UnHardcodedShaders.cpp" "${Line_Fixed}")
	endfunction()
	
	# Metallicafan212: Modified heavily from https://stackoverflow.com/questions/71299716/how-to-compile-hlsl-shaders-during-build-with-cmake
	
	# Metallicafan212: Generic shader compile function
	function(CompileShader ShaderFile ShaderModel EntryPoint ExportName ShaderDefines)
		if($<CONFIG:DEBUG>)
			SET(OptimizationMode /Od)
		else()
			SET(OptimizationMode /O1)
		endif()
		
		# Metallicafan212: Start a compile command string
		SET(ShaderCommand fxc.exe)
		list(APPEND ShaderCommand /nologo /I Inc /I Shaders /Zi /E${EntryPoint} /T ${ShaderModel} ${OptimizationMode} /Vn ${ExportName})
	
		# Metallicafan212: Get the filename without the extension
		get_filename_component(FILE_WE ${ShaderFile} NAME_WE)
		get_filename_component(FILE_E ${ShaderFile} NAME)
		
		# Metallicafan212: Get the directory that this shader was in
		get_filename_component(FILE_DIR ${ShaderFile} DIRECTORY)
		get_filename_component(FILE_DIR ${FILE_DIR} NAME)
		
		# Metallicafan212: Assemble the new path to the compiled shader
		SET(Shader_Base_Dir Inc/CompiledShaders/${FILE_DIR})
		SET(Shader_Dir ${Shader_Base_Dir}/${ShaderModel})
		
		# Metallicafan212: Create the directory now as FXC won't do it for us
		file(MAKE_DIRECTORY ${Shader_Base_Dir})
		
		#message(STATUS "Making dir ${Shader_Dir}")
		file(MAKE_DIRECTORY ${Shader_Dir})
		
		SET(Shader_Path ${Shader_Dir}/${FILE_WE}.h)
		
		# Metallicafan212: Keep a running list of the compiled shader headers
		list(APPEND HEADER_SHADER_FILES ${Shader_Path})
		
		# Metallicafan212: Game-specific logic
		if(${ICBIND_HARDCODED_SHADERS})
			LIST(APPEND ShaderCommand /DHARD_CODED_SHADERS=1 /DUSE_GEO_SHADER=1)
		elseif(${ICBIND_227_SHADERS})
			LIST(APPEND ShaderCommand /DDX11_UNREAL_227=1 /DUSE_GEO_SHADER=1)
		else()
			# Metallicafan212: Assume HP2
			LIST(APPEND ShaderCommand /DHP2_HARDCODE=1)
		endif()
		
		# Metallicafan212: Assemble the macro string
		#				   Syntax is /DMACRO_NAME=MACROVALUE
		#				   Also this double variable lookup is CURSED!
		foreach(ShaderMacro ${${ShaderDefines}})
			LIST(APPEND ShaderCommand /D${ShaderMacro})
		endforeach()
		
		# Metallicafan212: Rest of the command
		LIST(APPEND ShaderCommand /Fh ${Shader_Path} /Fd ${CMAKE_BINARY_DIR}/${FILE_WE}.pdb ${ShaderFile})
		
		add_custom_command(
			TARGET ICBINDx11Drv PRE_BUILD
			COMMAND ${ShaderCommand}
			COMMENT "HLSL Shader Model ${ShaderModel} ${ShaderFile}"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM
			# Metallicafan212: TODO! Add in a definition for the local header too....
			DEPENDS ${ShaderFile} "Shaders/ShaderGlobals.h"
		)
		
		SET(GENERATED_NAMESPACE "${FILE_WE}_${EntryPoint}_${ShaderModel}")
		
		#STRING(REPLACE "Inc/" "" Shader_Path_CPP Shader_Path)
		STRING(SUBSTRING ${Shader_Path} 4 -1 Shader_Path_CPP)
		
		# Metallicafan212: Add onto the includes
		set(HARDCODED_BEGINNING ${HARDCODED_BEGINNING}
			"namespace "
			${GENERATED_NAMESPACE}
			"\n{"
			"\n\t"
			"#include \"${Shader_Path_CPP}\"\n}\n\n"
			PARENT_SCOPE
			#"#undef SHADER_HEADER\n"
			#"\n\t#define SHADER_HEADER SHADER_EXPAND("
		)
		
		# Metallicafan212: Generate the filesystem key that the C++ code will use
		STRING(REPLACE "Shaders/" "" GENERATED_FPATH ${ShaderFile})
		STRING(REPLACE "/" "\\\\" GENERATED_FPATH ${GENERATED_FPATH})
		
		message(STATUS "Generated path is ${GENERATED_FPATH}")
		
		SET(GENERATED_KEY
			"${SHADERFOLDER} TEXT(\"${GENERATED_FPATH}:${EntryPoint}:${ShaderModel}\");"
		)
		
		SET(GENERATED_BYTES
			"${GENERATED_NAMESPACE}::g_${EntryPoint}"
		)
		#generatedKey        = shaderFolder + "TEXT(\"" + Shad.file + ":" + Entry.entrypoint + ":" + Lang.target + "\")";
		#generatedBytes      = generatedNamespace + "::g_" + Entry.entrypoint;
		
		# Metallicafan212: Now add onto the function
		SET(HARDCODED_FUNCTION ${HARDCODED_FUNCTION}
			"// Metallicafan212: Load ${GENERATED_NAMESPACE}\n\t"
			"Shad = &Bytecode.Set(${GENERATED_KEY}, TArray<BYTE>())~\n\t"
			"appMemcpy(Shad->GetData(), ${GENERATED_BYTES}, ARRAY_COUNT(${GENERATED_BYTES}))~\n\t\n\t"
			PARENT_SCOPE
		)
	endfunction()
	
	# Metallicafan212: Make the directory tree
	#message(STATUS "Making dir Inc/CompiledShaders")
	file(MAKE_DIRECTORY "Inc/CompiledShaders")

	# Metallicafan212: First, vertex shaders.
	# 				   We want to compile to specific shader models per file
	foreach(FILE ${ICBINDDRV_VERTEX_SHADERS})
		CompileShader(${FILE} "vs_5_0" "VertShader" "g_VertShader" "")
		CompileShader(${FILE} "vs_4_1" "VertShader" "g_VertShader" "")
		CompileShader(${FILE} "vs_4_0" "VertShader" "g_VertShader" "")
	endforeach(FILE)
	
	# Metallicafan212: Now, geo shaders
	foreach(FILE ${ICBINDDRV_GEOMETRY_SHADERS})
		CompileShader(${FILE} "gs_5_0" "GeoShader" "g_GeoShader" "")
		CompileShader(${FILE} "gs_4_1" "GeoShader" "g_GeoShader" "")
		CompileShader(${FILE} "gs_4_0" "GeoShader" "g_GeoShader" "")
	endforeach(FILE)
	
	# Metallicafan212: Lastly, pixel shaders
	foreach(FILE ${ICBINDDRV_PIXEL_SHADERS})
		SET(
			Pixel_Shader_Macros
			"PIXEL_SHADER=1"
		)
		message(STATUS "${Pixel_Shader_Macros}")
		CompileShader(${FILE} "ps_5_0" "PxShader" "g_PxShader" Pixel_Shader_Macros)
		CompileShader(${FILE} "ps_4_1" "PxShader" "g_PxShader" Pixel_Shader_Macros)
		CompileShader(${FILE} "ps_4_0" "PxShader" "g_PxShader" Pixel_Shader_Macros)
	endforeach(FILE)
	
	# Metallicafan212: Write to the file
	foreach(ShaderCompInc ${HARDCODED_BEGINNING})
		WriteShaderLine(${ShaderCompInc})
	endforeach()
	
	foreach(ShaderCompFunc ${HARDCODED_FUNCTION})
		WriteShaderLine(${ShaderCompFunc})
	endforeach()
	
	foreach(ShaderCompEnd ${HARDCODED_END})
		WriteShaderLine(${ShaderCompEnd})
	endforeach()

	# Metallicafan212: Not needed since we embed them into headers
	#add_custom_command(
	#	TARGET ICBINDx11Drv POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E copy ${CSO_SHADER_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>
	#	COMMAND_EXPAND_LISTS
	#)
	
endif()

source_group("Inc" 				FILES ${ICBINDDRV_Inc})
source_group("Inc/Robin-Map"	FILES ${ICBINDDRV_Robin_Map})
source_group("Src"				FILES ${ICBINDDRV_Src})

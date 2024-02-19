import subprocess;
import os;
import sys;
import glob;

# Metallicafan212: Go into the subdir
os.chdir("Shaders");

class Macro:
    def __init__(self, mac, val):
            self.define = mac;
            self.value  = val;

global_macros = [
      Macro("EXTRA_VERT_INFO", "1"),
      Macro("COMPLEX_SURF_MANUAL_UVs", "1"),
      Macro("GM_XOpenGL", "0"),
      Macro("GM_DX9", "1"),
]

# Metallicafan212:  Defined language version
class LanguageTarget:
    def __init__(self, targetName, inMacros = []):
            # Metallicafan212: Use a local list so the globals appear first
            localList = [];
            localList.extend(global_macros);
            localList.extend(inMacros);
            self.target = targetName;
            self.macros = localList;
            #print(self.macros);

# Metallicafan212: TODO! Don't hard-code the indexes
global_langages = {
    "vs_5_0"            : LanguageTarget('vs_5_0',              [Macro("USE_GEO_SHADER", "1")]),
    "vs_4_1"            : LanguageTarget('vs_4_1',              [Macro("USE_GEO_SHADER", "1")]),
    "vs_4_0"            : LanguageTarget('vs_4_0',              [Macro("USE_GEO_SHADER", "1")]),
    #"vs_4_0_level_9_3"  : LanguageTarget('vs_4_0_level_9_3',    [Macro("USE_GEO_SHADER", "0")]),

    "ps_5_0"            : LanguageTarget('ps_5_0',              [Macro("USE_GEO_SHADER", "1")]),
    "ps_4_1"            : LanguageTarget('ps_4_1',              [Macro("USE_GEO_SHADER", "1")]),
    "ps_4_0"            : LanguageTarget('ps_4_0',              [Macro("USE_GEO_SHADER", "1")]),
    #"ps_4_0_level_9_3"  : LanguageTarget('ps_4_0_level_9_3',   [Macro("USE_GEO_SHADER", "0")]),

    "gs_5_0"            : LanguageTarget('gs_5_0',              [Macro("USE_GEO_SHADER", "1")]),
    "gs_4_1"            : LanguageTarget('gs_4_1',              [Macro("USE_GEO_SHADER", "1")]),
    "gs_4_0"            : LanguageTarget('gs_4_0',              [Macro("USE_GEO_SHADER", "1")]),
}

class EntryPoint:
      def __init__(self, ent, languages):
            self.entrypoint = ent;
            self.langs      = languages;

# Metallicafan212: Define a shader
class Shader:
      def __init__(self, key, file, entrypoints):
            self.key            = key;
            self.file           = file;
            self.entrypoints    = entrypoints;

# Metallicafan212: Process all shaders for all specific targets
#                  TODO! I need specific macros defined in each target
ShaderArray = [
    Shader(
          "ComplexSurfShader",
          "ComplexSurfShader.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]),#global_langages["vs_4_0_level_9_3"]]),
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]),#global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "GeneralShader",
          "GeneralShader.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]),#global_langages["vs_4_0_level_9_3"]]),
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]),#global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "LineShader",
          "LineShader.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "Line_GeoShader",
          "Line_GeoShader.hlsl",
          [
              EntryPoint("GeoShader",  [global_langages["gs_5_0"], global_langages["gs_4_1"], global_langages["gs_4_0"]]),
          ]
    ),

    Shader(
          "MeshShader",
          "MeshShader.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "ResScaling",
          "ResScaling.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),
          ]
    ),

    Shader(
          "TileShader",
          "TileShader.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),
          ]
    ),
]

# Metallicafan212: We're going to be writing a function to automatically load the hard-coded shader when needed
hardcoded_cpp   = "..\\Src\\UnHardcodedShaders.cpp"

newline         = "\n";
tabbedLine      = newline + "\t";

includes        = "// Metallicafan212:\tThis file contains auto generated code to load hardcoded shaders (compiled from fxc.exe)" + newline + "#include \"ICBINDx11Drv.h\"" + newline + newline;

# Metallicafan212: Function code
functionCode    = "void FShaderManager::LoadHardcodedShaders()" + newline + "{" + tabbedLine + "guard(FShaderManager::LoadHardcodedShaders);" + tabbedLine + tabbedLine + "TArray<BYTE>* Shad = nullptr;" + tabbedLine + tabbedLine;

endOfFunction   = tabbedLine + "unguard;" + newline + "}"; 

# Metallicafan212: Shader directory
#                  This'll automatically use the macro in the C++ code, or can be redefined here
shaderFolder    = "SHADER_FOLDER ";

'''
void FShaderManager::LoadHardcodedShaders()
{
	guard(FShaderManager::LoadHardcodedShaders);

	// Metallicafan212:	Set the specific slots
	TArray<BYTE>* Shad = nullptr;
	Shad = &Bytecode.Set(TEXT("Blah"), TArray<BYTE>());

	// Metallicafan212:	Now copy in
	Shad->Add(ARRAY_COUNT(Bytes));
	appMemcpy(Shad->GetData(), Bytes, ARRAY_COUNT(Bytes));

	unguard;
}
'''

# Metallicafan212: Now that everything is defined, loop through and compile each shader
#                  Note that the FXC path is hard-coded here because it's not on the path variable
fxc_path = 'C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.22621.0\\x86\\fxc.exe';

for Shad in ShaderArray :
    # Metallicafan212: Process this shader
    for Entry in Shad.entrypoints:
        # Metallicafan212: Now assemble the command
        for Lang in Entry.langs:
            #print(Lang.target);
            args = "";

            # Metallicafan212: Save these so we can assemble a namespace for the CPP file
            generatedNamespace  = Shad.key + "_" + Entry.entrypoint + "_" + Lang.target;
            generatedShader     = generatedNamespace + ".h";

            # Metallicafan212: TODO! Should probably make it so that the code in the driver doesn't expect a real path, and instead relies just on the filename
            generatedKey        = shaderFolder + "TEXT(\"" + Shad.file + ":" + Entry.entrypoint + ":" + Lang.target + "\")";
            generatedBytes      = generatedNamespace + "::g_" + Entry.entrypoint;

            compileCommand = [
                fxc_path, 
                "/T" + Lang.target, 
                "/E" + Entry.entrypoint, 
                "/Fh..\\Inc\\CompiledShaders\\" + generatedShader
            ];

            # Metallicafan212: Now add all the macros
            for Mac in Lang.macros:
                # Metallicafan212: The online documentation says that you do blah=dah, but it's wrong for some reason
                compileCommand.append('/D' + Mac.define + "=" + Mac.value + '');
            compileCommand.append(Shad.file);

            #print(compileCommand);
            process = subprocess.run(compileCommand);

            if(process.returncode == 0):
                # Metallicafan212: Now add onto the includes
                includes += "namespace " + generatedNamespace + newline + "{" + tabbedLine + "#include \"CompiledShaders\\" + generatedShader + "\"" + newline + "}" + newline + newline;

                # Metallicafan212: Add the code to set the data
                functionCode += "// Metallicafan212: Load " + generatedNamespace + tabbedLine;
                functionCode += "Shad = &Bytecode.Set(" + generatedKey + ", TArray<BYTE>());" + tabbedLine;
                functionCode += "Shad->Add(ARRAY_COUNT(" + generatedBytes + "));" + tabbedLine + tabbedLine;
                functionCode += "appMemcpy(Shad->GetData(), " + generatedBytes + ", ARRAY_COUNT(" + generatedBytes + "));"
                functionCode += tabbedLine + tabbedLine;

print(includes);
print(functionCode);
print(endOfFunction);

# Metallicafan212: Write it to a file
with open(hardcoded_cpp, "w+") as file:
    file.write(includes);
    file.write(functionCode);
    file.write(endOfFunction);
    file.close();
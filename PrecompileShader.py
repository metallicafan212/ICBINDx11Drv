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

# Metallicafan212: TODO! Make this automatically synced to the C++ code! Maybe a shared header?
global_macros = [
      Macro("GM_XOpenGL", "0"),
      Macro("GM_DX9", "1"),
      Macro("FIRST_USER_CONSTBUFF", "b4"),
      Macro("SF_AlphaEnabled", "1"),
      Macro("SF_Modulated", "2"),
      Macro("SF_HudSmoothMasked", "4"),
      Macro("SF_MSDFRendering", "8"),
      Macro("SF_BicubicSampling", "16"),
]

# Metallicafan212:  Defined language version
class LanguageTarget:
    def __init__(self, targetName, inMacros = [], shaderMacro = ""):
            # Metallicafan212: Use a local list so the globals appear first
            localList           = [];
            localList.extend(global_macros);
            localList.extend(inMacros);
            self.target         = targetName;
            self.macros         = localList;
            self.shaderMacro    = shaderMacro;
            #print(self.macros);

# Metallicafan212: TODO! Don't hard-code the indexes
global_langages = {
    "vs_5_0"            : LanguageTarget('vs_5_0',              [Macro("USE_GEO_SHADER", "1")], "SM5_0"),
    "vs_4_1"            : LanguageTarget('vs_4_1',              [Macro("USE_GEO_SHADER", "1")], "SM4_1"),
    "vs_4_0"            : LanguageTarget('vs_4_0',              [Macro("USE_GEO_SHADER", "1")], "SM4_0"),
    #"vs_4_0_level_9_3"  : LanguageTarget('vs_4_0_level_9_3',    [Macro("USE_GEO_SHADER", "0")]),
    
    "ps_5_0"            : LanguageTarget('ps_5_0',              [Macro("USE_GEO_SHADER", "1"), Macro("PIXEL_SHADER", "1")], "SM5_0"),
    "ps_4_1"            : LanguageTarget('ps_4_1',              [Macro("USE_GEO_SHADER", "1"), Macro("PIXEL_SHADER", "1")], "SM4_1"),
    "ps_4_0"            : LanguageTarget('ps_4_0',              [Macro("USE_GEO_SHADER", "1"), Macro("PIXEL_SHADER", "1")], "SM4_0"),
    #"ps_4_0_level_9_3"  : LanguageTarget('ps_4_0_level_9_3',   [Macro("USE_GEO_SHADER", "0"), Macro("PIXEL_SHADER", "1")]),

    "gs_5_0"            : LanguageTarget('gs_5_0',              [Macro("USE_GEO_SHADER", "1")], "SM5_0"),
    "gs_4_1"            : LanguageTarget('gs_4_1',              [Macro("USE_GEO_SHADER", "1")], "SM4_1"),
    "gs_4_0"            : LanguageTarget('gs_4_0',              [Macro("USE_GEO_SHADER", "1")], "SM4_0"),
}

class EntryPoint:
      def __init__(self, ent, languages):
            self.entrypoint = ent;
            self.langs      = languages;

# Metallicafan212: Define a shader
class Shader:
      def __init__(self, key, headerPath, file, entrypoints):
            self.key            = key;
            self.headerPath     = headerPath;
            self.file           = file;
            self.entrypoints    = entrypoints;

# Metallicafan212: Process all shaders for all specific targets
#                  TODO! I need specific macros defined in each target
ShaderArray = [
    Shader(
          "ComplexSurfShader_Vert",
          "Complex\\",
          "Complex\\\\ComplexSurfShader_Vert.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]),#global_langages["vs_4_0_level_9_3"]]),
          ]
    ),
    
    Shader(
          "ComplexSurfShader_PX",
          "Complex\\",
          "Complex\\\\ComplexSurfShader_PX.hlsl",
          [
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]),#global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "GeneralShader_Vert",
          "General\\",
          "General\\\\GeneralShader_Vert.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]),#global_langages["vs_4_0_level_9_3"]]),
          ]
    ),
    
    Shader(
          "GeneralShader_PX",
          "General\\",
          "General\\\\GeneralShader_PX.hlsl",
          [
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]),#global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "LineShader_Vert",
          "Line\\",
          "Line\\\\LineShader_Vert.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
          ]
    ),
    
    Shader(
          "LineShader_PX",
          "Line\\",
          "Line\\\\LineShader_PX.hlsl",
          [
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "Line_GeoShader",
          "Line\\",
          "Line\\\\Line_GeoShader.hlsl",
          [
              EntryPoint("GeoShader",  [global_langages["gs_5_0"], global_langages["gs_4_1"], global_langages["gs_4_0"]]),
          ]
    ),

    Shader(
          "MeshShader_Vert",
          "Mesh\\",
          "Mesh\\\\MeshShader_Vert.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
          ]
    ),
    
    Shader(
          "MeshShader_PX",
          "Mesh\\",
          "Mesh\\\\MeshShader_PX.hlsl",
          [
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),  
          ]
    ),

    Shader(
          "ResScaling_Vert",
          "PostFX\\",
          "PostFX\\\\ResScaling_Vert.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
          ]
    ),
    
    Shader(
          "ResScaling_PX",
          "PostFX\\",
          "PostFX\\\\ResScaling_PX.hlsl",
          [
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),
          ]
    ),

    Shader(
          "TileShader_Vert",
          "Tile\\",
          "Tile\\\\TileShader_Vert.hlsl",
          [
              EntryPoint("VertShader",  [global_langages["vs_5_0"], global_langages["vs_4_1"], global_langages["vs_4_0"]]), #global_langages["vs_4_0_level_9_3"]]),
          ]
    ),
    
    Shader(
          "TileShader_PX",
          "Tile\\",
          "Tile\\\\TileShader_PX.hlsl",
          [
              EntryPoint("PxShader",    [global_langages["ps_5_0"], global_langages["ps_4_1"], global_langages["ps_4_0"]]), #global_langages["ps_4_0_level_9_3"]]),
          ]
    ),
]

# Metallicafan212: We're going to be writing a function to automatically load the hard-coded shader when needed
hardcoded_cpp   = "..\\Src\\UnHardcodedShaders.cpp"

newline         = "\n";
tabbedLine      = newline + "\t";

includes        = "// Metallicafan212:\tThis file contains auto generated code to load hardcoded shaders (compiled from fxc.exe)" + newline + "#include \"ICBINDx11Drv.h\"" + newline + newline + "#ifdef DX11_HP2" + newline + "#include \"HP2Shaders.h\"" + newline + "#endif" + newline + newline;

# Metallicafan212: This is a hack for HP2...
includes        += "#ifndef SM4_0" + newline + "#define SM4_0 CompiledShaders/" + newline + newline + "#endif" + newline + newline;
includes        += "#ifndef SM4_1" + newline + "#define SM4_1 CompiledShaders/" + newline + newline + "#endif" + newline + newline;
includes        += "#ifndef SM5_0" + newline + "#define SM5_0 CompiledShaders/" + newline + newline + "#endif" + newline + newline;

includes        += "#define CAT(x, y) x##y" + newline + newline;
includes        += "#define SHADER_STR_INT(x) #x" + newline + newline;
includes        += "#define SHADER_STR(x) SHADER_STR_INT(x)" + newline + newline;
includes        += "#define SHADER_EXPAND(x, y) SHADER_STR(CAT(x, y))" + newline + newline;

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
fxc_path    = 'C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.22621.0\\x86\\fxc.exe';

# Metallicafan212: Default to fxc_path, and run a detection to see if we're currently running within a vs instance
fxc         = fxc_path;

# Metallicafan212: Per AnthraX, check first if we're running from within a VS command prompt, as that'll have the current windows kit on the path variable
#if True:
try:
    check_fxc = subprocess.run("fxc /?", text=True, check=False, capture_output=True);
    #print("Output was " + check_fxc.stdout); 
    # Metallicafan212: TODO! Check if it was actually FXC? This would conflict with "future" replacements if they don't output the same info, for now just assume if we got output that it works
    if (check_fxc.stdout != ""):
        # Metallicafan212: TODO! Reevaluate this, this fucking blows since the env variable adds the \\ onto the end that the folder has... We just want to print the version number.
        #                  Perhaps find a different env var?
        fxc_eval = subprocess.run("where fxc", text=True, check=False, capture_output=True).stdout;#.split('\n')[0];
        
        # Metallicafan212: Check for a newline
        if('\n' in fxc_eval):
            fxc_eval = fxc_eval.split('\n')[0];
        
        # Metallicafan212: Lookup the current windows SDK version (in case it doesn't match the FXC path)
        windows_kit = os.environ["WINDOWSSDKVERSION"];
        
        # Metallicafan212: It has a slash for me, so remove it for clarity sake
        if('\\' in windows_kit):
            windows_kit = windows_kit.split('\\')[0];
        
        print("Found FXC (" + fxc_eval + ") in the current path/path env var, the current windows kit is " + windows_kit);
        fxc = 'fxc.exe';
#else:   
except:
    # Metallicafan212: Wasn't found, so don't override it.
    print("FXC not found on the path variable, defaulting to " + fxc_path);
    fxc = fxc_path;
    
# Metallicafan212: Make sure that the directories exist
if(not os.path.exists("..\\Inc\\CompiledShaders\\")):
    os.mkdir("..\\Inc\\CompiledShaders\\");

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
                fxc, 
                "/T" + Lang.target, 
                "/E" + Entry.entrypoint,
                "/Fh..\\Inc\\CompiledShaders\\" + Shad.headerPath + generatedShader
            ];
            
            # Metallicafan212: Check that the path exists first!
            if(not os.path.exists("..\\Inc\\CompiledShaders\\" + Shad.headerPath)):
                os.mkdir("..\\Inc\\CompiledShaders\\" + Shad.headerPath);

            # Metallicafan212: Now add all the macros
            for Mac in Lang.macros:
                # Metallicafan212: The online documentation says that you do blah=dah, but it's wrong for some reason
                compileCommand.append('/D' + Mac.define + "=" + Mac.value + '');
            compileCommand.append(Shad.file);

            #print(compileCommand);
            process = subprocess.run(compileCommand);

            if(process.returncode == 0):
                # Metallicafan212: Now add onto the includes
                includes += "namespace " + generatedNamespace + newline + "{";
                includes += tabbedLine + "#undef SHADER_HEADER" + newline;
                includes += tabbedLine + "#define SHADER_HEADER SHADER_EXPAND(" + Lang.shaderMacro + ", " + Shad.headerPath + generatedShader + ")" + tabbedLine;
                includes += tabbedLine + "#include SHADER_HEADER" + newline +  "}" + newline + newline;

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
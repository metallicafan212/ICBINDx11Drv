# D3D11Drv
Metallicafan212’s Farm-Raised DirectX 11 Renderer
“It ain’t much, but it’s honest work”

This repository contains a working (albeit unoptimized and a bit messy) DX11 renderer for HP2 New Engine, and in the future, for more UE1 games as well.
Currently in an alpha stage, as I need to implement more user configurables, more features, add a custom MSAA resolve to fix blurry tile rendering, etc.
Parts are based on how the DX9 renderer works, but have been heavily modified by myself. The only part copied "wholesale" from DX9 is the projection setup, as it actually works.
Eventually, I will be recoding the projection setup so that it uses the standard DirectX math libraries, but for now, this works.

## Current state
This renderer works great, but it's about 10% slower than DX9 in the most optimal of scenes, and up to 50% slower than DX9 in the least optimal scenes (tons of actors, for example).
I plan on improving performance in the future, but for games other than HP2, I don't anticipate it running better than DX9 unless the god of FPS bestows onto me some crack-brain scheme.

## What this renderer does
Everything the DX9 renderer does in HP2 (RT textures, distance fog, all standard rendering, on-screen fast string drawing using D2D, etc.).
Full UnrealEd support including all line drawing routines, selection, user-customizable 3D and 2D line thickness options, etc.

## What this renderer does not
Raytracing, external texture loading, support other games (yet).

## What's missing to support other games?
Generally, some features may be missing right now like Gouraud triangle drawing. In HP1/2, Epic updated the base engine to do indexed triangle drawing for meshes, so the interface slightly changed.
Theoretically, the code can work just fine as long as the Gouraud triangles are unfanned (using an index list), but no support has been written as of right now.

## Building
```
                            +&-
                          _.-^-._    .--.
                       .-'   _   '-. |__|
                      /     |_|     \|  |
                     /               \  |
                    /|     _____     |\ |
                     |    |==|==|    |  |
 |---|---|---|---|---|    |--|--|    |  |
 |---|---|---|---|---|    |==|==|    |  |
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
```
Currently, this will NOT be able to be built. It’s written for the HP2 new engine, and none of those headers/libs are included. In the future, when more games are supported, it will be buildable as long as you have the SDK for those games and define the right preprocessor macro.

Shaders are currently configured to point to ..\Shades, next to Sounds, Textures, etc. I did this because I want to eventually support user created HLSL code for HP2, but for ports, this might be annoying. So, in the shader header (UnD3DShader.h), the path can be quickly changed the macro SHADER_FOLDER.

## Contributing
Contributing is welcomed! If you wish to port this to other UE1 games, fork it, make the modifications in your fork, and submit a merge request. I will review and edit as needed.

My only requirement: if you make code comments, please attach your github username to them (like I have for mine). It makes it easier to identify where the code came from to identify a person to ask/”blame” for it (including myself (: ). If this is a problem for people, I can discuss it, I just want to keep it clean, readable, and able to be maintained, as this won’t be a fork, but the main code in the HP2 version.

If you're adding a new rendering function (such as DrawGouraudTriangle), add a new CPP file with the changes. This way, the whole file can be ignored if it's not relevant to the game. Rune, for example, has fog surface drawing.

For this purpose, I have placed this under the MIT license, so that people can come, fork, or even make new versions. I wouldn’t be here today without the excellent open source projects for Unreal, so I want to give back as much as possible.

For other games, I would like to use a standard set of defines. These may be subject to change, but they need to be kept standardized to make building for other games easy.
Let me know here or on discord if these should be changed/added to.
UT99: DX11_UT_99
Old Unreal UT99: DX11_UT_469
Old Unreal 227: DX11_UNREAL_227
HP1: DX11_HP1
Rune: DX11_RUNE
Deus Ex: DX11_DX
HP2: All of these undefined, aka: the default state.


## Donations? DONATIONS?!?!?!?!?!?
No, this isn't required, nor expected. Nowhere will you see me shilling for a Patreon or a Paypal in the code. If you do, then it’s a scam. I’m very against thrusting donation/payment links against the user, especially overwriting the clipboard contents. This is the only place I will be placing a Paypal link since some people have requested it.
This isn't a "pay me to work on this" either; I will work on it regardless, this is basically just a tipbox. If you're financially struggling, please do not donate, I will be very sad.

https://www.paypal.me/metallicafan212

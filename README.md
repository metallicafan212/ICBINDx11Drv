# ICBINDx11Drv: I can't believe it's not D3D11Drv!
Metallicafan212’s Farm-Raised Open Source DirectX 11 Renderer.
![image](https://github.com/metallicafan212/D3D11Drv/assets/5996243/787208bd-fe10-4f51-a7e6-675cc8a5fc3c)

This repository contains a working (albeit a bit messy) DX11 renderer for HP2 New Engine, Old Unreal UT 469, and in the future, for more UE1 games as well.
Currently in an alpha stage, as it does need more testing, but it's largely complete and has a lot of end-user customizable, in the editor and the game.
Parts are based on how the DX9 renderer works, but have been heavily modified by myself. The only part copied "wholesale" from DX9 is the projection setup, as it actually works.
Eventually, I will be recoding the projection setup so that it uses the standard DirectX math libraries, but for now, this works.

## TL;DR, Pls gib latest release
https://github.com/metallicafan212/ICBINDx11Drv/releases/latest

## Current state
This renderer works great, and on specific systems or games, runs 10% to 30% faster than the DX9 renderer. Lots of different settings for the user to change, and mess with.
Exclusive features like line thickness and super resolution (ResolutionScale setting).

## What this renderer does
Everything the DX9 renderer does in HP2 (RT textures, distance fog, all standard rendering, on-screen fast string drawing using D2D, etc.).
Full UnrealEd support including all line drawing routines, selection, user-customizable 3D and 2D line thickness options, etc.
Full UnrealEd browser support, including the texture and mesh browser.

## What this renderer does not
Raytracing, external texture loading, your taxes.

## What's missing to support other games?
The full (base) UE1 rendering interface is implemented and working. UT469, HP2, and 227 specific extensions are also implemented.
Specific game extensions (Rune, DX) are not implemented, but are planned.

## Open source notices
This repo also contains a copy of Robin-map (switchable on and off for standard std::unorderd_map) (https://github.com/Tessil/robin-map), licensed under MIT.
```
MIT License

Copyright (c) 2017 Thibaut Goetghebuer-Planchon <tessil@gmx.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```


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
This will require the Windows 10 SDK for building, but should run under Windows 7 SP1 (with the platform update).

Tools I used: Visual Studio 2022 (for the C++ code), Notepad++ for HLSL. Any syntax errors in the HLSL code is an instant crash in this driver right now, be warned. In the future, I'll handle errors more gracefully.

Currently, this is only able to be built for UT 469 only, other games have a basic UE1 stock implementation provided, but no current build chain support. My recommendation, if you want to help improve it, is to use the UT469 public SDK as a stable development platform.

Shaders are currently configured to point to ..\Shaders, next to Sounds, Textures, etc. I did this because I want to eventually support user created HLSL code for HP2, but for ports, this might be annoying. So, in the shader header (UnD3DShader.h), the path can be quickly changed by modifying the macro SHADER_FOLDER.

## Contributing
Contributing is welcomed! If you wish to port this to other UE1 games, fork it, make the modifications in your fork, and submit a merge request. I will review and edit as needed.

My only requirement: if you make code comments, please attach your github username to them (like I have for mine). It makes it easier to identify where the code came from to identify a person to ask/”blame” for it (including myself 😃). If this is a problem for people, I can discuss it, I just want to keep it clean, readable, and able to be maintained, as this won’t be a fork, but the main code in the HP2 version. That means: anything that breaks HP2 will be removed, reviewed, or put behind a preprocessor macro by myself.

If you're adding a new rendering function (such as DrawGouraudTriangle), add a new CPP file with the changes. This way, the whole file can be ignored if it's not relevant to the game.

For this purpose, I have placed this under the MIT license, so that people can come, fork, or even make new versions. I wouldn’t be here today without the excellent open source projects for Unreal, so I want to give back as much as possible.

For other games, I would like to use a standard set of defines. These may be subject to change, but they need to be kept standardized to make building for other games easy.
Let me know here or on discord if these should be changed/added to. 

| Game | Define |
| ---- | ------ |
| UT99 | DX11_UT_99 |
| OldUnreal UT99 | DX11_UT_469 |
| OldUnreal 227 | DX11_UNREAL_227 |
| HP1 | DX11_HP1 |
| Rune | DX11_RUNE |
| Deus Ex | DX11_DX |
| HP2 | All undefined, aka: the default state. <br />A macro has been made to make explcit testing this possible <br />DX11_HP2 |

## Contact Me
My discord username is @metallicafan212 and I'm in the Old Unreal discord server (https://discord.gg/thURucxzs6) and in the HP Modding server (https://discord.com/invite/KWwBevF). For HP specific questions/problems, it would best to contact me in the HP modding server, otherwise I can take questiosn here or in the Old Unreal server.

## Donations? DONATIONS?!?!?!?!?!?
No, this isn't required, nor expected. Nowhere will you see me shilling for a Patreon or a Paypal in the code. If you do, then it’s a scam. I’m very against thrusting donation/payment links against the user, especially overwriting the clipboard contents. This is the only place I will be placing a Paypal link since some people have requested it.
This isn't a "pay me to work on this" either; I will work on it regardless, this is basically just a tipbox. If you're financially struggling, please do not donate, I will be very sad.

https://www.paypal.me/metallicafan212

@echo off

cd ../..
IF NOT EXIST "third_party_new" (
    mkdir third_party_new
)
cd third_party_new

IF NOT EXIST "glm" (
    PowerShell -Command "& { Invoke-WebRequest https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip -OutFile glm.zip }"
    PowerShell -Command "& { Expand-Archive glm.zip -DestinationPath . }"
    move glm-0.9.9.8 glm
    del glm.zip
)

IF NOT EXIST "SDL" (
    PowerShell -Command "& { Invoke-WebRequest https://www.libsdl.org/release/SDL2-devel-2.30.0-VC.zip -OutFile SDL2.zip }"
    PowerShell -Command "& { Expand-Archive SDL2.zip -DestinationPath . }"
    PowerShell -Command "& { Rename-Item -Path .\SDL2-2.30.0 -NewName SDL }"
    del SDL2.zip
)

IF NOT EXIST "glew-2.1.0" (
    PowerShell -Command "& { Invoke-WebRequest https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip -OutFile glew.zip }"
    PowerShell -Command "& { Expand-Archive glew.zip -DestinationPath . }"
    del glew.zip
)

IF NOT EXIST "freetype-2.11.0" (
    PowerShell -Command "& { Invoke-WebRequest https://download.savannah.gnu.org/releases/freetype/freetype-2.11.0.tar.gz -OutFile freetype.tar.gz }"
    tar -xzf freetype.tar.gz
    del freetype.tar.gz
)

IF NOT EXIST "IL" (
    PowerShell -Command "& { Invoke-WebRequest https://github.com/DentonW/DevIL/archive/refs/tags/v1.8.0.zip -OutFile DevIL.zip }"
    PowerShell -Command "& { Expand-Archive DevIL.zip -DestinationPath . }"
    PowerShell -Command "& { Move-Item -Path .\DevIL-1.8.0\DevIL\include\IL -Destination .\IL }"
    del DevIL.zip
    del DevIL-1.8.0
)

IF NOT EXIST "openal" (
    PowerShell -Command "& { Invoke-WebRequest https://github.com/kcat/openal-soft/releases/download/1.23.1/openal-soft-1.23.1-bin.zip -OutFile openal.zip }"
    PowerShell -Command "& { Expand-Archive openal.zip -DestinationPath . }"
    PowerShell -Command "& { Rename-Item -Path .\openal-soft-1.23.1-bin -NewName .\openal }"
    del openal.zip
)

IF NOT EXIST "SDL2_image-2.0.5" (
    PowerShell -Command "& { Invoke-WebRequest https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.zip -OutFile SDL2_image.zip }"
    PowerShell -Command "& { Expand-Archive SDL2_image.zip -DestinationPath . }"
    del SDL2_image.zip
)

@REM IF NOT EXIST "assimp" (
@REM     git clone https://github.com/assimp/assimp.git
@REM     call build_assimp.bat
@REM )

@REM IF EXIST "assimp" (
@REM     echo here
@REM     call build_assimp.bat
@REM )

pause
cd ../..
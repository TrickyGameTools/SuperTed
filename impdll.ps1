# import dll


#function private:builddate($d){
#        $date = Get-Date
#        $cpp = '
#
#        #pragma once
#        #define BPP_BuildDate "' + $date + '"
#        '
#
#        echo $cpp > $d/builddate.hpp
#}


function private:imp($want) {
        echo "Importing: $want"
        cp "$want" Release
}

echo "DLL Import Script - (c) Jeroen P. Broks 2022"

#builddate
#../../BuildJCR/bin/Release/net6.0/BuildJCR.exe

$drive="E:"

imp "$drive/projects/applications/visualstudio/vc/zlib-msvc14-x64.1.2.11.7795/build/native/bin_release/zlib.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2-2.0.12/lib/x64/SDL2.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_image-2.0.5/lib/x64/SDL2_image.dll"
#imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_image-2.0.5/lib/x64/libpng16-16.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_image-2.0.5/lib/x64/lib*.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_image-2.0.5/lib/x64/LIC*.txt"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_image-2.0.5/lib/x64/zlib1.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_mixer-2.0.4/lib/x64/SDL2_mixer.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_mixer-2.0.4/lib/x64/libogg-0.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_mixer-2.0.4/lib/x64/libvorbis-0.dll"
imp "$drive/projects/applications/visualstudio/vc/3rdparty/SDL2_mixer-2.0.4/lib/x64/libvorbisfile-3.dll"

imp "$drive/Projects/Applications/VisualStudio/VC/3rdparty/Lua/lua-5.4.0/Lua Library/x64/Release/Lua.dll"

echo "
Done

"

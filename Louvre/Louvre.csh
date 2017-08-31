# Author: D. Menasce

setenv Louvre_HOME       "~menasce/Programming/myQtProjects/build-Louvre-Desktop_Qt_5_6_0_clang_64bit-Debug/Louvre.app/Contents/MacOS/"
setenv MAGICK_HOME       "/Applications/ImageMagick-7.0.1"
setenv PATH              "${MAGICK_HOME}/bin:${PATH}"
setenv DYLD_LIBRARY_PATH "${MAGICK_HOME}/lib"

alias  Louvre            "${Louvre_HOME}/Louvre"

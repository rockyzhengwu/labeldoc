
# use to fix link arra on mac

cd /usr/local/lib
mv libjpeg.dylib libjpeg.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libJPEG.dylib libJPEG.dylib
mv libtiff.dylib libtiff.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libTIFF.dylib libTIFF.dylib
mv libpng.dylib libpng.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libPng.dylib libPNG.dylib
mv libgif.dylib libgif.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libGIF.dylib libGIF.dylib

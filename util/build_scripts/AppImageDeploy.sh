#!/bin/bash
RELEASE_DIR="build/Qt/rtklib-appimage-x86_64-2.4.3"

mkdir -p $RELEASE_DIR
rm -rf $RELEASE_DIR/*
linuxdeployqt=$(realpath $1)
single_image() {
    app=$1
    app_qt="${app}_qt"
    appimage_dir=$RELEASE_DIR/"${app^^}_Qt-x86_64"
    icon="${app}_Icon"
    mkdir -p $appimage_dir
    cp -t $appimage_dir app/$app_qt/$app_qt app/$app_qt/"${icon}.ico"
    convert $appimage_dir/"${icon}.ico" $appimage_dir/"${icon}.png"
    if [ $app == 'rtknavi' ]
    then
        mv $appimage_dir/rtknavi_Icon-0.png $appimage_dir/rtknavi_Icon.png
        rm $appimage_dir/rtknavi_Icon-1.png
    fi
    printf  \
"[Desktop Entry]
Name=${app^^}_Qt
Comment=${app^^} Qt version by Emlid
Exec=${app_qt}
Icon=${icon}
Type=Application
Categories=Science;
Terminal=false" > $appimage_dir/"${app_qt}.desktop"
    cd $appimage_dir/
    $linuxdeployqt "${app_qt}.desktop" -appimage
    mv ${app^^}*.AppImage ${app^^}_Qt-x86_64.AppImage
    cd -
}

for app in rtkpost rtkconv rtkplot; do single_image "$app"; done

zipfile="build/Qt/rtklib-qt-appimage_x86_64.zip"
rm $zipfile
zip -j $zipfile $RELEASE_DIR/*/RTKPOST*.AppImage \
                $RELEASE_DIR/*/RTKPLOT*.AppImage \
                $RELEASE_DIR/*/RTKCONV*.AppImage
md5sum $zipfile > $zipfile".md5"
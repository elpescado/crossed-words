SetCompressor /SOLID /FINAL lzma
 
!define PRODUCT_NAME "Crossed Words"
!define VERSION 0.70
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\corssed-words.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
 
!include "MUI.nsh"
!define MUI_ABORTWARNING
;!define MUI_ICON ".\pixmaps\crossed-words.ico"
;!define MUI_UNICON ".\pixmaps\crossed-words.ico"
 
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\crossed-words.exe"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

OutFile "crossed-words-${VERSION}-win32.exe"
Name "${PRODUCT_NAME}"
InstallDir "$PROGRAMFILES\Crossed Words"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show



# Installer
Section Main

  SetOutPath $INSTDIR

  # Main executable
  File bin\Release\crossed-words.exe
  
  # Libraries - DLL files
  File bin\Release\freetype6.dll
  File bin\Release\intl.dll
  File bin\Release\libatk-1.0-0.dll
  File bin\Release\libcairo-2.dll
  File bin\Release\libexpat-1.dll
  File bin\Release\libfontconfig-1.dll
  File bin\Release\libgdk-win32-2.0-0.dll
  File bin\Release\libgdk_pixbuf-2.0-0.dll
  File bin\Release\libgio-2.0-0.dll
  File bin\Release\libglib-2.0-0.dll
  File bin\Release\libgmodule-2.0-0.dll
  File bin\Release\libgobject-2.0-0.dll
  File bin\Release\libgtk-win32-2.0-0.dll
  File bin\Release\libpango-1.0-0.dll
  File bin\Release\libpangocairo-1.0-0.dll
  File bin\Release\libpangoft2-1.0-0.dll
  File bin\Release\libpangowin32-1.0-0.dll
  File bin\Release\libpng12-0.dll
  File bin\Release\zlib1.dll
  
  # Data files
  File bin\Release\gaddag.dag
  File bin\Release\gtkrc
  
  # Language files
  SetOutPath $INSTDIR\lang\pl
  File bin\Release\lang\pl\alphabet.txt
  
  # Images
  SetOutPath $INSTDIR\pixmaps
  File bin\Release\pixmaps\horizontal.png
  File bin\Release\pixmaps\vertical.png
  
  # GTK+ - fonts
  SetOutPath $INSTDIR\etc\fonts
  File bin\Release\etc\fonts\fonts.conf
  File bin\Release\etc\fonts\fonts.dtd
  
  # GTK+ - config
  SetOutPath $INSTDIR\etc\gtk-2.0
  File bin\Release\etc\gtk-2.0\gdk-pixbuf.loaders
  File bin\Release\etc\gtk-2.0\gtk.immodules
  File bin\Release\etc\gtk-2.0\im-multipress.conf
  
  # Pango modules
  SetOutPath $INSTDIR\etc\pango
  File bin\Release\etc\pango\pango.modules
  
  # GTK+ library files
  SetOutPath $INSTDIR\lib\gtk-2.0\2.10.0\engines
  File bin\Release\lib\gtk-2.0\2.10.0\engines\libpixmap.dll
  File bin\Release\lib\gtk-2.0\2.10.0\engines\libwimp.dll
  SetOutPath $INSTDIR\lib\gtk-2.0\2.10.0\loaders
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-ani.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-bmp.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-gif.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-icns.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-ico.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-jpeg.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-pcx.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-png.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-pnm.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-ras.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-tga.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-tiff.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-wbmp.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-xbm.dll
  File bin\Release\lib\gtk-2.0\2.10.0\loaders\libpixbufloader-xpm.dll  
  SetOutPath $INSTDIR\lib\gtk-2.0\include
  File bin\Release\lib\gtk-2.0\include\gdkconfig.h
  SetOutPath $INSTDIR\lib\gtk-2.0\modules
  File bin\Release\lib\gtk-2.0\modules\libgail.dll
  
SectionEnd


# Shortcuts
Section Shortcuts
  SetOutPath $INSTDIR
  CreateShortCut "$SMPROGRAMS\Crossed Words.lnk" "$INSTDIR\crossed-words.exe"
SectionEnd


# Uninstaller
Section Uninstaller

  WriteUninstaller $INSTDIR\uninstall.exe
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\crossed-words.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\scrossed-words.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${VERSION}"

SectionEnd



Section Uninstall

  RMDir /r "$INSTDIR"
  Delete "$SMPROGRAMS\Crossed Words.lnk"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"

SectionEnd

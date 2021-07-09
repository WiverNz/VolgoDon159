@ECHO OFF
SET gfxexport=gfxexport.exe
SET pref_path=:\Program Files (x86)\Crytek\CRYENGINE Launcher\Crytek\CRYENGINE_5.
SET post_path=\Tools\GFxExport\

SET DRIVE_LETTERS=C D E F G H I J K L M N O P Q R S T U V W X Y Z
SET SUB_VERSIONS=9 8 7 6 5 4 3 2 1
SETLOCAL EnableDelayedExpansion
FOR %%D IN (%DRIVE_LETTERS%) DO (
  if exist "%%D:\" (
    REM ECHO Checking drive %%D
    FOR %%A IN (%SUB_VERSIONS%) DO (
	  SET current_path="%%D%pref_path%%%A%post_path%%gfxexport%"
	  if exist !current_path! (
        SET gfx_path=!current_path!
		GOTO FOUND
	  )
    )
  )
)

ECHO %gfxexport% not found
EXIT /b 1

:FOUND

ECHO Found %gfx_path%
rem CALL %gfx_path% -h
rem PAUSE
CALL %gfx_path% -share_images -gsid ./GfxResources -fslist -i dds -d ./GfxResources -o ./GfxResources MainMenuFlash/MainMenu.swf
CALL %gfx_path% -share_images -gsid ./GfxResources -fslist -i dds -d ./GfxResources -o ./GfxResources ShipInterfaceFlash/ShipInterface.swf

ECHO Conversion completed
rem PAUSE
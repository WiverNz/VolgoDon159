@ECHO OFF
SET a7z_exe=7z.exe
SET a7z_folder=C:/Program Files/7-Zip/

SET a7z_path="%a7z_folder%%a7z_exe%"

if not exist %a7z_path% (
  ECHO Can't find 7z path %a7z_path%
  PAUSE
  EXIT /b 1
)

ECHO %a7z_path%

CALL %a7z_path% a -tzip -r -mx0 ships.pak Assets/props/volgo_don

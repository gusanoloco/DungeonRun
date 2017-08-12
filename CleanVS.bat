::Remove any un-needed folder from the projects
for /d /r . %%d in (bin, obj, Debug, ipch, release, build) do @if exist "%%d" rd /s /q "%%d"

:: delete cache files

set filetype=.ncb

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)

set filetype=.aps

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)

set filetype=.opensdf

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)

set filetype=.sdf

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)

set filetype=.cachefile

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)

:: delete user files
set filetype=.suo

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)

set filetype=.user

for /r %%i in (*) do (
   if %%~xi==%filetype% (
      del "%%i"
   )
)
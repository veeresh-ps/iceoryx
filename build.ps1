Write-Host "Staring iceoryx build"
$Env:Path += ";C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\"
Write-Host "Displaying path"

gci env: | where name -like 'Path'
gci env: | where name -like '*proxy*'
cmake --version

Write-Host "Cleaning build folder"
# TODO optionally clean build folder
# rm -Recurse -Force .\build
Write-Host "Staring iceoryx build"
mkdir -p build
cmake -Bbuild -Hiceoryx_meta -DTOML_CONFIG=OFF -Dtest=OFF
cmake --build .\build 

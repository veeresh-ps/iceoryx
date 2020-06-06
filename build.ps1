write-host "Staring iceoryx build"
$Env:Path += ";C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\"

write-host "Displaying path"
gci env: | where name -like 'Path'
gci env: | where name -like '*proxy*'

if(Test-Path build)
{
    write-host("Cleaning build folder")
    rm -Recurse -Force .\build   
}

write-host "Staring iceoryx build"
mkdir -p build
cmake -Bbuild -Hiceoryx_meta -DTOML_CONFIG=ON -Dtest=ON
cmake --build .\build 

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
: run.bat
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: server Command: port
start "" "Debug\RemoteCodePublisher.exe" 8081

start "" "Debug\RemoteCodePublisher.exe" 8082

:: client1
start "" "Debug\Client_one.exe"

:: client2
start "" "Debug\Client2.exe"
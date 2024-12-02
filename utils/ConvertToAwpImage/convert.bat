SET CONV_PATH=%cd%\conv
if not exist "%CONV_PATH%" mkdir "%CONV_PATH%"
for %%a in (*) do (
ConvertToAwpImage.exe %%a "%CONV_PATH%\%%a.awp")
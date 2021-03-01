REM  Generate Rom "ram" code

for %%f in (*.gb) do (
    echo %%~nf

	
	python ..\Python\bin2c.py ./%%~nf.gb %%~nfCode >"./%%~nf.c"

)

pause

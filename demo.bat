@echo off
echo ==================================================
echo   5 KOROS AUTOMATIZALT BENCHMARK INDITASA
echo   (OpenMP vs OpenCL 1D vs OpenCL 2D)
echo ==================================================
echo.

echo 0. Program forditas...
mingw32-make
echo.

REM --- 1. KÖR ---
echo ==================================================
echo   1. MERESI KOR INDUL - Kepek kimentesevel...
echo ==================================================
del data\results\performance.csv 2>nul
REM --- ITT A VALTOZAS: 4 Oszlop a fejlecben! ---
echo PixelSzam,OpenMP_Ido,OpenCL_1D_Ido,OpenCL_2D_Ido > data\results\performance.csv

.\szurke_program.exe data\samples\input1.jpg data\samples\output1.png >nul
.\szurke_program.exe data\samples\input2.jpg data\samples\output2.png >nul
.\szurke_program.exe data\samples\input3.jpg data\samples\output3.png >nul
.\szurke_program.exe data\samples\input4.jpg data\samples\output4.png >nul
.\szurke_program.exe data\samples\input5.jpg data\samples\output5.png >nul
.\szurke_program.exe data\samples\input6.jpg data\samples\output6.png >nul
.\szurke_program.exe data\samples\input7.jpg data\samples\output7.png >nul

copy data\results\performance.csv data\results\performance_1.csv >nul
python plot_results.py 1
echo.

REM --- 2-5. KÖR ---
FOR /L %%i IN (2,1,5) DO (
    echo ==================================================
    echo   %%i. MERESI KOR INDUL - Gyorsitott mod...
    echo ==================================================

    del data\results\performance.csv 2>nul
    echo PixelSzam,OpenMP_Ido,OpenCL_1D_Ido,OpenCL_2D_Ido > data\results\performance.csv

    .\szurke_program.exe data\samples\input1.jpg none >nul
    .\szurke_program.exe data\samples\input2.jpg none >nul
    .\szurke_program.exe data\samples\input3.jpg none >nul
    .\szurke_program.exe data\samples\input4.jpg none >nul
    .\szurke_program.exe data\samples\input5.jpg none >nul
    .\szurke_program.exe data\samples\input6.jpg none >nul
    .\szurke_program.exe data\samples\input7.jpg none >nul

    copy data\results\performance.csv data\results\performance_%%i.csv >nul
    python plot_results.py %%i
    echo.
)

echo MIND AZ 5 MERES KESZ!
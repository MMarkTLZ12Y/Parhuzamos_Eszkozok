import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

os.makedirs('data/plots', exist_ok=True)

run_id = "1"
if len(sys.argv) > 1:
    run_id = sys.argv[1]

try:
    df = pd.read_csv('data/results/performance.csv')
    df.columns = [col.strip() for col in df.columns]
except FileNotFoundError:
    print("Hiba: Nem talalom a performance.csv fajlt.")
    exit()

df = df.sort_values(by='PixelSzam')
df['Megapixel'] = df['PixelSzam'] / 1000000

plt.figure(figsize=(10, 6))

# HÁROM VONAL RAJZOLÁSA
plt.plot(df['Megapixel'], df['OpenMP_Ido'], marker='o', linestyle='-', color='blue', label='OpenMP (Processzor)', linewidth=2, markersize=8)
plt.plot(df['Megapixel'], df['OpenCL_1D_Ido'], marker='s', linestyle='-', color='red', label='OpenCL (GPU 1D)', linewidth=2, markersize=8)
plt.plot(df['Megapixel'], df['OpenCL_2D_Ido'], marker='^', linestyle='--', color='green', label='OpenCL (GPU 2D)', linewidth=2, markersize=8)

plt.title(f'Szürkeárnyalatos konverzió: Architektúrák összehasonlítása (Mérés: {run_id}/5)', fontsize=15, fontweight='bold')
plt.xlabel('Képméret (Megapixel) - Logaritmikus skála', fontsize=12)
plt.ylabel('Futási idő (Másodperc)', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=12)

plt.xscale('log')
plt.gca().xaxis.set_major_formatter(ScalarFormatter())

plot_path = f'data/plots/performance_plot_{run_id}.png'
plt.savefig(plot_path, dpi=300, bbox_inches='tight')
plt.close()
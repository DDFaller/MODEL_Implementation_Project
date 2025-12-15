import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def plot_timings(csv_file='timings.csv'):
    os.makedirs('plots', exist_ok=True)
    df = pd.read_csv(csv_file)
    
    # Group by poly_size and function_name, then calculate mean for duplicates
    df_grouped = df.groupby(['poly_size', 'function_name'])['time_seconds'].mean().reset_index()
    
    # Pivot data for easier plotting
    pivot_df = df_grouped.pivot(index='poly_size', columns='function_name', values='time_seconds')
    
    # 1. Linear scale comparison
    plt.figure(figsize=(10, 6))
    for method in pivot_df.columns:
        plt.plot(pivot_df.index, pivot_df[method], marker='o', label=method, linewidth=2)
    plt.xlabel('Polynomial Size')
    plt.ylabel('Time (seconds)')
    plt.title('Algorithm Performance Comparison - Linear Scale')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('plots/timing_linear.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # 2. Log-log scale
    plt.figure(figsize=(10, 6))
    for method in pivot_df.columns:
        plt.loglog(pivot_df.index, pivot_df[method], marker='o', label=method, linewidth=2)
    plt.xlabel('Polynomial Size')
    plt.ylabel('Time (seconds)')
    plt.title('Algorithm Performance Comparison - Log-Log Scale')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('plots/timing_loglog.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # 3. Bar chart comparison
    plt.figure(figsize=(12, 6))
    x = np.arange(len(pivot_df.index))
    width = 0.25
    
    for i, method in enumerate(pivot_df.columns):
        plt.bar(x + i*width, pivot_df[method], width, label=method, alpha=0.8)
    
    plt.xlabel('Polynomial Size')
    plt.ylabel('Time (seconds)')
    plt.title('Algorithm Performance Comparison - Bar Chart')
    plt.xticks(x + width, pivot_df.index)
    plt.legend()
    plt.yscale('log')
    plt.savefig('plots/timing_bars.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # 4. Speedup ratios
    if 'naive' in pivot_df.columns:
        plt.figure(figsize=(10, 6))
        
        if 'karatsuba' in pivot_df.columns:
            speedup_k = pivot_df['naive'] / pivot_df['karatsuba']
            plt.plot(pivot_df.index, speedup_k, marker='o', label='Karatsuba vs Naive', linewidth=2)
        
        if 'tom3' in pivot_df.columns:
            speedup_t3 = pivot_df['naive'] / pivot_df['tom3']
            plt.plot(pivot_df.index, speedup_t3, marker='s', label='Toom-Cook 3 vs Naive', linewidth=2)
        
        plt.axhline(y=1, color='red', linestyle='--', alpha=0.7, label='No speedup')
        plt.xlabel('Polynomial Size')
        plt.ylabel('Speedup Factor')
        plt.title('Algorithm Speedup over Naive Implementation')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.savefig('plots/speedup_ratios.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    print("Gráficos salvos em plots/:")
    print("- timing_linear.png")
    print("- timing_loglog.png") 
    print("- timing_bars.png")
    print("- speedup_ratios.png")

if __name__ == "__main__":
    plot_timings()
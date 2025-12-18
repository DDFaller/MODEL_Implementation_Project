import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def plot_errors(csv_file='cumulative_error.csv'):
    os.makedirs('plots', exist_ok=True)
    df = pd.read_csv(csv_file)
    
    # Group by poly_size and function_name, then calculate mean for duplicates
    df_grouped = df.groupby(['poly_size', 'function_name'])['error'].mean().reset_index()
    
    # Pivot data for easier plotting
    pivot_df = df_grouped.pivot(index='poly_size', columns='function_name', values='error')
    
    # Remove naive column since it's always 0
    if 'naive' in pivot_df.columns:
        pivot_df = pivot_df.drop('naive', axis=1)
    
    # 1. Linear scale error comparison
    plt.figure(figsize=(10, 6))
    for method in pivot_df.columns:
        plt.plot(pivot_df.index, pivot_df[method], marker='o', label=method, linewidth=2)
    plt.xlabel('Polynomial Size')
    plt.ylabel('Cumulative Error vs Naive')
    plt.title('Algorithm Error Comparison vs Naive - Linear Scale')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('plots/error_linear.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # 2. Log scale error comparison
    plt.figure(figsize=(10, 6))
    for method in pivot_df.columns:
        plt.semilogy(pivot_df.index, pivot_df[method], marker='o', label=method, linewidth=2)
    plt.xlabel('Polynomial Size')
    plt.ylabel('Cumulative Error vs Naive (log scale)')
    plt.title('Algorithm Error Comparison vs Naive - Log Scale')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('plots/error_log.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    print("Gráficos de erro salvos em plots/:")
    print("- error_linear.png")
    print("- error_log.png")

if __name__ == "__main__":
    plot_errors()
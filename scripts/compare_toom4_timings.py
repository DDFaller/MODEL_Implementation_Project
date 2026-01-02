#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def load_and_process_data(file_path, label):
    """Load CSV and calculate mean timing per polynomial size"""
    df = pd.read_csv(file_path)
    # Group by polynomial size and calculate mean
    grouped = df.groupby('poly_size')['time_seconds'].agg(['mean', 'std']).reset_index()
    grouped['label'] = label
    return grouped

def main():
    # File paths
    timings_k_path = "../csvs/timings.csv"
    timings_toom4_path = "../csvs/timings_toom4_full.csv"
    
    # Load data
    data_k = load_and_process_data(timings_k_path, "Toom-4 (Skipping Karatsuba)")
    data_toom4 = load_and_process_data(timings_toom4_path, "Toom-4")
    
    # Create the plot
    plt.figure(figsize=(12, 8))
    
    # Plot both datasets
    plt.errorbar(data_k['poly_size'], data_k['mean'], yerr=data_k['std'], 
                marker='o', label='Toom-4 (Skip Karatsuba)', capsize=5, linewidth=2)
    plt.errorbar(data_toom4['poly_size'], data_toom4['mean'], yerr=data_toom4['std'], 
                marker='s', label='Toom-4', capsize=5, linewidth=2)
    
    # Formatting
    plt.xlabel('Polynomial Size')
    plt.ylabel('Time (seconds)')
    plt.title('Performance Comparison: Karatsuba vs Toom-4 Multiplication')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.xscale('log')
    plt.yscale('log')
    
    # Save the plot
    plt.tight_layout()
    plt.savefig('../plots/toom4_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # Print summary statistics
    print("Performance Summary:")
    print("=" * 50)
    for size in sorted(set(data_k['poly_size']).intersection(set(data_toom4['poly_size']))):
        k_time = data_k[data_k['poly_size'] == size]['mean'].iloc[0]
        t4_time = data_toom4[data_toom4['poly_size'] == size]['mean'].iloc[0]
        speedup = k_time / t4_time
        print(f"Size {size:6d}: Karatsuba {k_time:.6f}s, Toom-4 {t4_time:.6f}s, Speedup: {speedup:.2f}x")

if __name__ == "__main__":
    main()
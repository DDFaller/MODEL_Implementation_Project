# MACHADO CARNEIRO FALLER Daniel (21400117)
# LY Laura (21500152)
# CRIVELLARO Federico (21502450)

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import argparse

METHOD_FLAGS = {
    "naive": "naive",
    "karatsuba": "karatsuba",
    "tom3": "tom3",
    "tom4": "tom4",
}

def plot_timings(csv_file="../csvs/timings.csv", selected_methods=None, out_dir="../plots"):
    os.makedirs(out_dir, exist_ok=True)

    df = pd.read_csv(csv_file)

    # Average duplicates
    df_grouped = df.groupby(["poly_size", "function_name"])["time_seconds"].mean().reset_index()

    # Pivot
    pivot_df = df_grouped.pivot(index="poly_size", columns="function_name", values="time_seconds")
    pivot_df = pivot_df.sort_index()

    # Filter methods
    if selected_methods is None:
        selected_methods = list(METHOD_FLAGS.values())

    selected_methods = [m for m in selected_methods if m in pivot_df.columns]
    if not selected_methods:
        raise ValueError("None of the selected methods exist in the CSV columns.")

    pivot_df = pivot_df[selected_methods]

    # 1) Linear scale
    plt.figure(figsize=(10, 6))
    for method in pivot_df.columns:
        plt.plot(pivot_df.index, pivot_df[method], marker="o", label=method, linewidth=2)
    plt.xlabel("Polynomial Size")
    plt.ylabel("Time (seconds)")
    plt.title("Algorithm Performance Comparison - Linear Scale")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig(os.path.join(out_dir, "timing_linear.png"), dpi=300, bbox_inches="tight")
    plt.close()

    # 2) Log-log scale
    plt.figure(figsize=(10, 6))
    for method in pivot_df.columns:
        plt.loglog(pivot_df.index, pivot_df[method], marker="o", label=method, linewidth=2)
    plt.xlabel("Polynomial Size")
    plt.ylabel("Time (seconds)")
    plt.title("Algorithm Performance Comparison - Log-Log Scale")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig(os.path.join(out_dir, "timing_loglog.png"), dpi=300, bbox_inches="tight")
    plt.close()

    # 3) Bar chart
    plt.figure(figsize=(12, 6))
    x = np.arange(len(pivot_df.index))
    width = 0.8 / len(pivot_df.columns)  # auto width based on number of methods

    for i, method in enumerate(pivot_df.columns):
        plt.bar(x + i * width, pivot_df[method], width, label=method, alpha=0.8)

    plt.xlabel("Polynomial Size")
    plt.ylabel("Time (seconds)")
    plt.title("Algorithm Performance Comparison - Bar Chart")
    plt.xticks(x + width * (len(pivot_df.columns) - 1) / 2, pivot_df.index)
    plt.legend()
    plt.yscale("log")
    plt.savefig(os.path.join(out_dir, "timing_bars.png"), dpi=300, bbox_inches="tight")
    plt.close()

    # 4) Speedup ratios (only if naive is in selected)
    if "naive" in pivot_df.columns:
        plt.figure(figsize=(10, 6))

        for method in pivot_df.columns:
            if method == "naive":
                continue
            speedup = pivot_df["naive"] / pivot_df[method]
            plt.plot(pivot_df.index, speedup, marker="o", label=f"{method} vs naive", linewidth=2)

        plt.axhline(y=1, color="red", linestyle="--", alpha=0.7, label="No speedup")
        plt.xlabel("Polynomial Size")
        plt.ylabel("Speedup Factor")
        plt.title("Algorithm Speedup over Naive Implementation")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.savefig(os.path.join(out_dir, "speedup_ratios.png"), dpi=300, bbox_inches="tight")
        plt.close()

    print(f"Plots saved in {out_dir}/:")
    print("- timing_linear.png")
    print("- timing_loglog.png")
    print("- timing_bars.png")
    if "naive" in pivot_df.columns and len(pivot_df.columns) > 1:
        print("- speedup_ratios.png")

def parse_args():
    parser = argparse.ArgumentParser(description="Plot timings from timings.csv")
    parser.add_argument("--csv", default="../csvs/timings.csv", help="Path to timings.csv")
    parser.add_argument("--out", default="../plots", help="Output directory for plots")

    parser.add_argument("-n", "--naive", action="store_true", help="Include naive")
    parser.add_argument("-k", "--karatsuba", action="store_true", help="Include karatsuba")
    parser.add_argument("-t3", "--tom3", action="store_true", help="Include tom3")
    parser.add_argument("-t4", "--tom4", action="store_true", help="Include tom4")
    parser.add_argument("--all", action="store_true", help="Include all methods")

    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()

    flags = {
        "naive": args.naive,
        "karatsuba": args.karatsuba,
        "tom3": args.tom3,
        "tom4": args.tom4,
    }

    if args.all or not any(flags.values()):
        selected = list(METHOD_FLAGS.values())
    else:
        selected = [METHOD_FLAGS[name] for name, enabled in flags.items() if enabled]

    plot_timings(csv_file=args.csv, selected_methods=selected, out_dir=args.out)


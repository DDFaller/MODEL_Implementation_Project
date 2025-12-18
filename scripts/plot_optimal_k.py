import pathlib
import pandas as pd
import matplotlib.pyplot as plt

# Project structure assumption:
#   csvs/timings_k.csv
#   plots/Saved_plots/
#   scripts/plot_avg_time_vs_k.py  (this file)
BASE = pathlib.Path(__file__).resolve().parents[1]
CSV_DEFAULT = BASE / "csvs" / "timings_k.csv"

# Fallback (useful if you run it from elsewhere or if the CSV is in the repo root)
ALT_CSV = BASE / "timings_k.csv"

PLOTS_DIR = BASE / "plots"
PLOTS_DIR.mkdir(parents=True, exist_ok=True)

POLY_MAX = 4096

csv_path = CSV_DEFAULT if CSV_DEFAULT.exists() else ALT_CSV
if not csv_path.exists():
    raise FileNotFoundError(f"Could not find timings_k.csv at: {CSV_DEFAULT} or {ALT_CSV}")

df = pd.read_csv(csv_path)

# Filter poly size (<4096 requested). If empty, fall back to <=4096 to avoid producing nothing.
df_filt = df[df["poly_size"] < POLY_MAX].copy()
filter_note = f"Filtered: poly_size < {POLY_MAX}"
if df_filt.empty:
    df_filt = df[df["poly_size"] <= POLY_MAX].copy()
    filter_note = f"Fixed polynomial size ≤ {POLY_MAX} (data contains: {sorted(df['poly_size'].unique())})"

# Clean numeric columns
df_filt["function_name"] = df_filt["function_name"].astype(str)
df_filt["k"] = pd.to_numeric(df_filt["k"], errors="coerce")
df_filt["time_seconds"] = pd.to_numeric(df_filt["time_seconds"], errors="coerce")
df_filt = df_filt.dropna(subset=["k", "time_seconds"])

poly_sizes = sorted(df_filt["poly_size"].unique())
methods = sorted(df_filt["function_name"].unique())

for poly in poly_sizes:
    d = df_filt[df_filt["poly_size"] == poly].copy()
    if d.empty:
        continue

    # Average time for each method at each k
    agg = (
        d.groupby(["k", "function_name"], as_index=False)["time_seconds"]
         .mean()
         .rename(columns={"time_seconds": "mean_time_seconds"})
         .sort_values(["function_name", "k"])
    )

    # Plot all methods on one figure
    fig, ax = plt.subplots(figsize=(10, 6))
    for m in methods:
        sub = agg[agg["function_name"] == m]
        if sub.empty:
            continue
        ax.plot(sub["k"].values, sub["mean_time_seconds"].values, marker="o", label=m)

    ax.set_xlabel("k_size")
    ax.set_ylabel("Average time (seconds)")
    ax.set_title("Average time vs k (all methods)", fontsize=12)
    fig.suptitle("Timing averages", fontsize=14)
    fig.text(0.5, 0.94, f"{filter_note} | poly_size = {poly}", ha="center", va="top", fontsize=10)
    ax.legend()
    ax.grid(True, linestyle="--", linewidth=0.5)
    fig.tight_layout(rect=[0, 0, 1, 0.90])

    out_png = PLOTS_DIR / f"avg_time_vs_k_poly{poly}.png"
    fig.savefig(out_png, dpi=200)
    plt.close(fig)

    # Save the averaged data too (handy for reports)
    out_csv = PLOTS_DIR / f"avg_time_vs_k_poly{poly}.csv"
    agg.to_csv(out_csv, index=False)

print(f"Saved plots to: {PLOTS_DIR}")

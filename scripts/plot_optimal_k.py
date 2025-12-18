import pandas as pd
import matplotlib.pyplot as plt

# --- load your csv ---
# change this to your filename/path
csv_path = "../csvs/timings_k.csv"
df = pd.read_csv(csv_path)

# --- keep only the 3 algorithms we want ---
wanted = {"karatsuba_k", "toom3_k", "toom4_k"}
df = df[df["function_name"].isin(wanted)].copy()

# ensure numeric
df["k"] = pd.to_numeric(df["k"], errors="coerce")
df["time_seconds"] = pd.to_numeric(df["time_seconds"], errors="coerce")
df = df.dropna(subset=["k", "time_seconds"])

# --- average over repeated runs with same k (and same function) ---
avg = (
    df.groupby(["function_name", "k"], as_index=False)["time_seconds"]
      .mean()
      .sort_values("k")
)

# --- plot 3 lines on the same graph ---
plt.figure()
for fn, g in avg.groupby("function_name"):
    plt.plot(g["k"], g["time_seconds"], marker="o", label=fn.replace("_k", ""))

plt.xlabel("k")
plt.ylabel("time (seconds)")
plt.title("Average time vs k")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig("../plots/optimal_k.png")

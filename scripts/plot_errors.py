# MACHADO CARNEIRO FALLER Daniel (21400117)
# LY Laura (21500152)
# CRIVELLARO Federico (21502450)

import pathlib
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

BASE = pathlib.Path(__file__).resolve().parents[1]
CSV_DEFAULT = BASE / "csvs" / "errors.csv"
PLOTS_DIR = BASE / "plots"
PLOTS_DIR.mkdir(parents=True, exist_ok=True)

def _format_sci(ax):
    fmt = ScalarFormatter(useMathText=True)
    fmt.set_powerlimits((-3, 3))  # força notação científica fora dessa faixa
    ax.yaxis.set_major_formatter(fmt)

def plot_errors(csv_path=CSV_DEFAULT):
    df = pd.read_csv(csv_path)

    required_cols = {"function_name", "max_error", "l2_error"}
    if not required_cols.issubset(df.columns):
        raise ValueError(f"CSV precisa conter colunas {required_cols}, encontrado: {df.columns}")

    has_poly = "poly_size" in df.columns
    if has_poly:
        # agrega por tamanho do polinômio e função
        agg = (
            df.groupby(["poly_size", "function_name"], as_index=False)[["max_error", "l2_error"]]
              .mean()
              .sort_values(["function_name", "poly_size"])
        )
    else:
        # adiciona índice de progresso por função
        df = df.copy()
        df["sample_idx"] = df.groupby("function_name").cumcount()
        agg = df.sort_values(["function_name", "sample_idx"])

    for metric in ["max_error", "l2_error"]:
        fig, ax = plt.subplots(figsize=(10, 6))
        x_col = "poly_size" if has_poly else "sample_idx"

        for fname, sub in agg.groupby("function_name"):
            ax.plot(sub[x_col], sub[metric], marker="o", label=fname)

        ax.set_xlabel("Polynomial size" if has_poly else "Sample index")
        ax.set_ylabel(metric)
        ax.set_title(f"Evolução de {metric} por função")
        ax.grid(True, linestyle="--", linewidth=0.6, alpha=0.5)
        ax.legend()
        _format_sci(ax)
        fig.tight_layout()

        out = PLOTS_DIR / f"errors_progress_{metric}.png"
        fig.savefig(out, dpi=200)
        plt.close(fig)
        print(f"Salvo: {out}")

if __name__ == "__main__":
    plot_errors()


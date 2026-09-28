"""
Genera todos los graficos a partir de los CSV producidos por ./main
Uso:  python3 graficar.py        (requiere pandas, matplotlib, numpy)
Salida: carpeta graficos/
"""
import os
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

os.makedirs("graficos", exist_ok=True)
COLAS = ["binomial", "fibonacci"]
NOMBRE = {"binomial": "Cola binomial", "fibonacci": "Cola de Fibonacci"}


def ajustar_c(y, f):
    """Constante c que minimiza sum (y - c f)^2."""
    y, f = np.asarray(y, float), np.asarray(f, float)
    return float((y * f).sum() / (f * f).sum())


# ---------------------------------------------------------------
# 6.3.1 Costo total
# cotas:  binomial  O((v+e) log v)   |   Fibonacci  O(e + v log v)
# ---------------------------------------------------------------
def cota_total(cola, v, e):
    lg = np.log2(v)
    return (v + e) * lg if cola == "binomial" else e + v * lg


def graficos_631():
    if not os.path.exists("resultados_631.csv"):
        return
    df = pd.read_csv("resultados_631.csv")
    df["s"] = df.tiempo_ns / 1e9
    prom = df.groupby(["cola", "serie", "i", "j", "v", "e"], as_index=False).s.mean()

    for serie in ["A", "B"]:
        sub = prom[prom.serie == serie]
        if sub.empty:
            continue
        ymax = 0
        cs = {}
        for cola in COLAS:
            d = sub[sub.cola == cola].sort_values(["i", "j"])
            f = cota_total(cola, d.v.values, d.e.values)
            cs[cola] = ajustar_c(d.s.values, f)
            ymax = max(ymax, d.s.max(), (cs[cola] * f).max())
        for cola in COLAS:
            d = sub[sub.cola == cola].sort_values(["i", "j"])
            f = cota_total(cola, d.v.values, d.e.values)
            x = d.j.values if serie == "A" else d.i.values
            etiqueta_x = "j  (e = 2^j, v fijo)" if serie == "A" else "i  (v = 2^i, e fijo)"
            plt.figure(figsize=(6, 4))
            plt.plot(x, d.s.values, "o-", label="Tiempo medido (promedio)")
            plt.plot(x, cs[cola] * f, "s--",
                     label=f"Cota teorica x c  (c = {cs[cola]:.2e})")
            plt.ylim(0, ymax * 1.1)
            plt.xticks(x)
            plt.xlabel(etiqueta_x)
            plt.ylabel("Tiempo total (s)")
            plt.title(f"6.3.1  {NOMBRE[cola]}  -  Serie {serie}")
            plt.grid(alpha=.3)
            plt.legend()
            plt.tight_layout()
            plt.savefig(f"graficos/631_{cola}_serie{serie}.png", dpi=150)
            plt.close()


# ---------------------------------------------------------------
# 6.3.2 Costo amortizado (acumulado vs llamadas a decreaseKey)
# cotas:  binomial  O(k log v)   |   Fibonacci  O(k)   (k = llamadas)
# ---------------------------------------------------------------
def cota_dk(cola, k, v):
    return k * np.log2(v) if cola == "binomial" else k.astype(float)


def graficos_632():
    if not os.path.exists("curvas_632.csv"):
        return
    cv = pd.read_csv("curvas_632.csv")
    paso = cv.llamadas[cv.llamadas > 0].min()
    cv = cv[cv.llamadas % paso == 0]          # descarta puntos sin checkpoint comun

    medidas = {
        "tiempo": ("tiempo_ns", 1e6, "Tiempo acumulado (ms)"),
        "operaciones": ("operaciones", 1, None),
    }

    for serie in ["C", "D"]:
        for medida, (col, esc, ylab) in medidas.items():
            datos = {}
            for cola in COLAS:
                sub = cv[(cv.cola == cola) & (cv.serie == serie)]
                curvas = {}
                #for (i, j), g in sub.groupby(["i", "j"]):
                #    m = g.groupby("llamadas")[col].mean() / esc
                #    curvas[(i, j)] = m
                for (i, j), g in sub.groupby(["i", "j"]):
                    por_k = g.groupby("llamadas")[col]
                    m = por_k.mean() / esc
                    cuenta = por_k.count()
                    m = m[cuenta == cuenta.max()]   # solo checkpoints alcanzados por TODAS las semillas
                    curvas[(i, j)] = m
                datos[cola] = curvas
            if not any(datos[c] for c in COLAS):
                continue

            # constante c y limites comunes a ambas colas (misma escala)
            cs, xmax, ymax = {}, 0, 0
            for cola in COLAS:
                ys, fs = [], []
                for (i, j), m in datos[cola].items():
                    k = m.index.values
                    ys.append(m.values); fs.append(cota_dk(cola, k, 2 ** i))
                if ys:
                    cs[cola] = ajustar_c(np.concatenate(ys), np.concatenate(fs))
                for (i, j), m in datos[cola].items():
                    k = m.index.values
                    xmax = max(xmax, k.max())
                    ymax = max(ymax, m.max(), (cs[cola] * cota_dk(cola, k, 2 ** i)).max())

            for cola in COLAS:
                if not datos[cola]:
                    continue
                if ylab is None:
                    yl = ("Intercambios acumulados" if cola == "binomial"
                          else "Cortes acumulados")
                else:
                    yl = ylab
                plt.figure(figsize=(6.5, 4.2))
                for idx, ((i, j), m) in enumerate(sorted(datos[cola].items())):
                    k = m.index.values
                    color = f"C{idx}"
                    plt.plot(k, m.values, "-", color=color, label=f"i={i}, j={j}")
                    plt.plot(k, cs[cola] * cota_dk(cola, k, 2 ** i), "--",
                             color=color, alpha=.7)
                plt.plot([], [], "k--", label=f"Cota x c (c = {cs[cola]:.2e})")
                plt.xlim(0, xmax * 1.02)
                plt.ylim(0, ymax * 1.1)
                plt.xlabel(r"$k$ = Llamadas a decreaseKey") #<--
                plt.ylabel(yl)
                cota = "k log v" if cola == "binomial" else "k"
                plt.title(f"6.3.2  {NOMBRE[cola]}  -  Serie {serie}  ({medida})\n"
                          f"cota teorica: O({cota})")
                plt.grid(alpha=.3)
                plt.legend(fontsize=8)
                plt.tight_layout()
                plt.savefig(f"graficos/632_{medida}_{cola}_serie{serie}.png", dpi=150)
                plt.close()


graficos_631()
graficos_632()
print("Graficos guardados en la carpeta graficos/")

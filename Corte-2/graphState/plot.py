import pandas as pd
import matplotlib.pyplot as plt

palette={"primary":"#F7F38D9F",
         "background": "#252525",
         "primary_chart":"#F1F1F1",
         "text_color": "#7F7F7F"}

# Graficar
plt.rcParams["axes.spines.bottom"] = True
plt.rcParams["axes.spines.left"] = False
plt.rcParams["axes.spines.right"] = False
plt.rcParams["axes.spines.top"] = False
plt.rcParams["axes.grid"] = True
plt.rcParams['grid.linestyle'] = '--'
plt.rcParams['grid.linewidth'] = 1
plt.rcParams["axes.grid.axis"] = 'y'
plt.rcParams["grid.color"] = "#404040"
plt.rcParams["figure.facecolor"] = palette["background"]
plt.rcParams["axes.facecolor"] = palette["background"]
plt.rcParams["savefig.facecolor"] = palette["background"]
plt.rcParams['axes.labelcolor']= palette["text_color"]

fig, ax = plt.subplots()

ax.spines['bottom'].set_color("#FFFFFF")
ax.tick_params(axis='x', colors="#FFFFFF")
ax.tick_params(axis='y', colors="#FFFFFF")



def drawChart():
    # Leer CSV
    df = pd.read_csv("data.csv")
    dfHeuristic = pd.read_csv("heuristic.csv")
    dfLS = pd.read_csv("ls.csv")
    dfOptimal = pd.read_csv("optimal.csv")

    plt.plot(df["solution_id"], df["beneficio"], c=palette['primary'], linewidth=0.8)
    plt.xlabel("Solution ID", c="gray")
    plt.ylabel("Beneficio", c="gray")
    plt.title("Landscape Knapsack", c="gray")
    plt.grid(True)

    heuristicSolution = dfHeuristic.iloc[0, 0]
    heuristicValue = dfHeuristic.iloc[0, 1]

    plt.scatter(heuristicSolution, heuristicValue, 10, marker='D', c="#FFD000", alpha=0.7, zorder=2)
    plt.annotate(
        "Heuristic",
        (heuristicSolution, heuristicValue),
        xytext=(1, 6),
        textcoords="offset points",
        fontsize=8,
        color= "gray"
    )

    optimalSolution = dfOptimal.iloc[0, 0]
    optimalValue = dfOptimal.iloc[0, 1]

    plt.scatter(optimalSolution, optimalValue, 10, marker='D', c="#97FFFD", alpha=0.7, zorder=4)
    plt.annotate(
        "Optimal",
        (optimalSolution, optimalValue),
        xytext=(1, 6),
        textcoords="offset points",
        fontsize=8,
        color= "gray"
    )


    plt.plot(dfLS["Solution"], dfLS["Value"], marker="o", markersize=1.5, color="#D9AD26", linewidth=0.8)

    pathLS_x = dfLS["Solution"]
    pathLS_y = dfLS["Value"]

    for i in range(len(pathLS_x) - 1):
        plt.annotate(
            "",
            xy = (pathLS_x[i+1], pathLS_y[i+1]),
            xytext = (pathLS_x[i], pathLS_y[i]),
            arrowprops=dict(arrowstyle="->", color="#D9AD26", linewidth=1),
            zorder=3
        )

    plt.show()

drawChart()
    
import pandas as pd
import matplotlib
matplotlib.use('Agg') 
import matplotlib.pyplot as plt


plt.style.use('seaborn-darkgrid')
plt.rcParams.update({'figure.figsize': (10,6), 'font.size': 12})

csv_path = "ocupacao_buffer.csv"
df = pd.read_csv(csv_path)

x = df['operacao']
y = df['ocupacao']

plt.figure()
plt.plot(x, y, linewidth=1, color='tab:blue')
plt.xlabel('Operação')
plt.ylabel('Ocupação do buffer')
plt.title('Ocupação do buffer ao longo das operações')
plt.grid(True)
plt.tight_layout()
plt.savefig('ocupacao_buffer_line.png', dpi=150)
print("Salvo: ocupacao_buffer_line.png")

plt.show()
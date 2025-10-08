import subprocess
import statistics
import matplotlib
matplotlib.use('Agg') 
import matplotlib.pyplot as plt

# Casos de teste
Ns = [1, 10, 100, 1000]
combinacoes = [(1,1), (1,2), (1,4), (1,8), (2,1), (4,1), (8,1)]
repeticoes = 10

resultados = {N: [] for N in Ns}

for N in Ns:
    print(f"\n=== Testando buffer N = {N} ===")
    for (Np, Nc) in combinacoes:
        tempos = []
        print(f"  Rodando (Np={Np}, Nc={Nc}) ...")
        for _ in range(repeticoes):
            # Executa o programa com entrada automática
            cmd = f"./prodcons"
            entrada = f"{N}\n{Np}\n{Nc}\n"
            saida = subprocess.run(cmd, input=entrada.encode(), capture_output=True)
            
            # Extrai o tempo do stdout
            for linha in saida.stdout.decode().splitlines():
                if "Tempo total de execucao" in linha:
                    tempo = float(linha.split(":")[1].split()[0])
                    tempos.append(tempo)
                    break
        
        media = statistics.mean(tempos)
        resultados[N].append(media)
        print(f"    Tempo médio: {media:.4f} s")

# === Gera o gráfico ===
labels = [f"{p}/{c}" for (p, c) in combinacoes]

plt.figure(figsize=(10,6))
for N in Ns:
    plt.plot(labels, resultados[N], marker='o', label=f"N={N}")

plt.xlabel("Threads Produtor/Consumidor (Np/Nc)")
plt.ylabel("Tempo médio de execução (s)")
plt.title(f"Desempenho do modelo Produtor/Consumidor")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("tempo_medio.png")
plt.show()

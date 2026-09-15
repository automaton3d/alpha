# Harness de quantização dinâmica de carga

Testa a hipótese do atrator N* (Γ_cap vs Γ_esc) a partir de **fluxos por
evento**, não de níveis de população — o erro que inviabilizou os runs
`basic32_seed_1` (partição fixa, Γ_esc ≡ 0, OLS trivial dN = 0·N).

## Uso

```bash
python3 quantize.py run1.csv run2.csv --bins 12 --surrogates 200 --out relatorio.txt
```

CSV de entrada: uma linha por (ilha, frame) com colunas
`run,island,frame,N,captures,escapes`. Se o logger só grava população,
use `--derive-fluxes` (recurso limitado: não distingue turnover de estase).

## O que o relatório responde

1. **Sanidade** — detecta na hora o modo de falha antigo (zero escapes,
   N constante por ilha).
2. **Fluxos binados** — Γ_cap(N) e Γ_esc(N) por faixa de população; o
   atrator exige cruzamento de sinal do drift (+ → −).
3. **Ajuste de drift** — dN = a + b·N com N* = −a/b, IC bootstrap por
   ilha e checagem AR(1) dos resíduos. Atrator só se b < 0.
4. **Surrogates** — embaralha dN dentro de cada ilha; p pequeno afasta o
   artefato de regressão à média.
5. **Veredito** — PASS / FAIL / INCONCLUSIVE explícito.

## Validação

`make_synth.py` gera dois cenários: `attractor.csv` (reversão à média em
N*=30 → recuperado como 30.20, IC [29.79, 30.53], p=0.01) e
`partition.csv` (o modo de falha basic32_seed_1 → INCONCLUSIVE com o
aviso correto).

## Próximos passos no simulador

- Logar `captures`/`escapes` por ilha por frame (hoje só há N agregado).
- Rodar a ablação EXCLUSION on/off e passar os dois CSVs ao mesmo harness:
  a conclusão falsificável é "a quantização exige exclusão de identidade".
- Para N*/L² constante: mais frames por ilha, não mais ilhas.

## Status da ligação ao simulador (14 Sep 2026) -- ver `FINDINGS.md`

- **Os fluxos por ilha por frame já existem**: `constituents.csv` de qualquer
  corrida do census tem `frame,chief_w,constituents,captures,escapes` (capturas e
  escapes *exatos*, medidos de snapshots de membership).  O primeiro item da lista
  acima está, portanto, satisfeito -- faltava apenas ligá-lo ao harness.
- `flux_from_census.py` faz essa conversão (`N = constituents + 1`) para o formato
  `run,island,frame,N,captures,escapes`; o `--derive-fluxes` deixa de ser
  necessário em dados reais.
- **numpy/pandas não estão instalados** neste ambiente, por isso `quantize_stdlib.py`
  é o mesmo método portado para a stdlib (OLS, bootstrap por ilha, AR(1),
  surrogates, veredito) com um modo extra `--by-run` que lê `N*` por corrida
  (a comparação `N*` vs `L` que a alegação de quantização exige).
  `make_synth_stdlib.py` reproduz os dois cenários sintéticos em stdlib: o
  atrator é recuperado (`N*=30.38`, p=0.005 -> PASS) e a partição fixa devolve
  INCONCLUSIVE.
- Resultado em seis conjuntos reais (tabela completa em `FINDINGS.md`): quatro
  corridas **congeladas** (escapes = 0: referência em L=9 e L=15, `EXCLUSION` e
  `ADDRESS_TARGET`) -> INCONCLUSIVE pelo próprio harness; e as duas corridas do
  canal direcional, que **têm** turnover, dão deriva **anti-restauradora**
  (`b = +0.49`, estável entre 20 e 64 jornadas) -> FAIL, sem `N*` em nenhum caso.


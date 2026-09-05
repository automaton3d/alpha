# Experimento minimo: um propeller e um K

**STATUS: os resultados abaixo sao historicos, da variante com voo imposto.
A regra exclusiva foi retirada. O build atual usa o kernel comum e o teste
de velocidade precisa passar novamente. Veja [a auditoria](PROPELLER_K_AUDIT.md).
Os CSVs, PNG e GIF anteriores nao certificam o AC local.**

Existem exatamente tres camadas: `w=0` contem o cacique K; `w=1` e `w=2`
contem as duas metades reciprocas de **um unico** par P. As metades ficam
coincidentes e o par conserva sua identidade e seu vetor m.

A condicao sob teste e explicita: **P avanca uma celula por quadro de luz**,
inclusive antes do contato e nas fases de raio zero. Essa velocidade e uma
regra estipulada para este ensaio, nao uma velocidade emergente demonstrada.
O programa nao reescreve posicoes dentro do loop de medicao: o deslocamento
passa pela resolucao de fontes e pela translacao de camadas do simulador.

## Isolamento

O build usa `PROPELLER_K_FLIGHT_EXPERIMENT`, com objetos separados em
`build/propeller_k/`. A variante substitui a reciclagem do P por voo
continuo e permite empurroes por contato tambem no raio zero.
A aplicacao e os ensaios de ilha, compilados sem essa macro, mantem
a regra anterior. Assim nao se decide antecipadamente a dinamica de toda
uma ilha a partir deste ensaio minimo.

A resposta do K continua sendo a regra de contato existente: frentes ativas
de mesma afinidade registram contato; ha no maximo um empurrao por quadro,
inclusive na fase de raio zero quando ha contato real. Antes, esse contato
era descartado, introduzindo uma parada periodica artificial no K.
O voo de P nao depende de haver empurrao.
Nao ha delegados, coesao K-D/D-D nem escolha entre varios alvos.

Quando os centros coincidem e os relogios estao sincronizados, as frentes
continuam coincidentes em todas as fases. O contato renova o empurrao a
cada quadro; ambos recebem o mesmo passo, mantendo a coincidencia.
Nao foi acrescentada memoria de captura nem movimento do K sem contato.
Essa e uma verificacao da regra de referencia, ainda coordenada pelo host;
nao demonstra uma implementacao estritamente local nem fases independentes.

## Executar

No diretorio `E:\alpha`:

```bat
experiments\build_propeller_k.bat
build\propeller_k\propeller_k.exe experiments\pk_right.csv 72 8 right same 51
```

Sintaxe:

```text
propeller_k CSV [FRAMES=32] [SEP=8] [right|left] [same|foreign] [LX=51] [PHASE=0]
```

O tubo e `LX x 7 x 7`, periodico, com `RMAX=3`. Por padrao o P nasce em
`x=2`, o K em `x=10`, e ambos compartilham a afinidade zero. P tem
`m=(3,0,0)`; isso indica sua direcao, nao um salto de tres celulas. A
velocidade maxima e medida em celulas por **quadro de luz**, nao por tick
interno nem por segundo de execucao.
`SEP=0` testa o conjunto inicialmente coincidente; `PHASE=0..5` altera
a fase inicial comum das tres fontes, mantendo os relogios sincronizados.

Controles:

```bat
build\propeller_k\propeller_k.exe experiments\pk_left.csv 120 8 left same 17
build\propeller_k\propeller_k.exe experiments\pk_foreign.csv 120 8 right foreign 17
```

O segundo controle atravessa a borda do toro; P deve continuar a um passo
por quadro e o K de outra afinidade deve ficar parado.

Para gerar um grafico dos dados sem abrir janela:

```powershell
powershell -ExecutionPolicy Bypass -File experiments\plot_propeller_k.ps1
```

## Medidas e verificacoes

O CSV inclui o estado inicial (quadro zero), as posicoes das duas metades,
as trajetorias desenroladas no toro, os passos de K e P, a fase de P apos
a atualizacao, os contatos internos de cada metade com K, os empurroes e
as passagens pela borda. Os contatos sao os de mesma afinidade registrados
durante o quadro; nao sao contagens de todos os voxels nem de sobreposicoes
geometricas com afinidades diferentes. As metades sao contadas como um P.

O executavel falha com codigo nao zero se P parar, saltar mais de uma
celula, perder uma metade, mudar afinidade/m, se K perder a identidade ou
se K se deslocar sem contato interno. Essas verificacoes nao dependem de
`assert` nem desaparecem com `NDEBUG`.
Para a mesma afinidade, tambem falha se P ultrapassar K ou se, depois da
coincidencia, o conjunto perder velocidade ou se separar. Ensaios com pelo
menos `6*SEP+12` quadros exigem captura e seis quadros de movimento conjunto.
Esse limite e um criterio de regressao do ensaio, nao uma lei do modelo.
`experiments\test_propeller_k.bat` recompila e executa nove casos cobrindo
os dois sentidos, seis fases comuns, separacoes de 0, 1, 2, 3, 5 e 8 e a
afinidade diferente, durante
120 quadros em `17 x 7 x 7`, com varias voltas no toro.

Todas as amostras sao mantidas, inclusive a formacao inicial das frentes.
O resumo da resposta de K descreve este transiente de contato, nao uma
estatistica de equilibrio nem uma lei macroscopica de inercia.

## Resultados verificados

| CSV | LX | Quadros | Media P | Media K | Captura | Quadros conjuntos seguintes |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| pk_right.csv | 51 | 72 | +1 | +0.888889 | 19 | 53 |
| pk_left.csv | 17 | 120 | -1 | -0.933333 | 19 | 101 |
| pk_foreign.csv | 17 | 120 | +1 | 0 | nenhuma | 0 |

Unidades: celulas por quadro de luz. As medias incluem a aproximacao.
Depois da captura, K e P movem-se a exatamente uma celula em cada quadro,
incluindo raio zero e bordas periodicas. O primeiro contato ocorreu no
quadro 4 nos dois ensaios de mesma afinidade.

Os nove casos de regressao tambem passaram, cada um com 120 quadros.
Na semente coincidente, K e P mantiveram velocidade +1 desde o primeiro
quadro. Nos outros casos de mesma afinidade, a captura ocorreu entre os
quadros 3 e 19 e persistiu ate o fim. O controle de outra afinidade
manteve K parado durante sete voltas de P no toro.

O grafico de `pk_right.csv` esta em [pk_right.png](pk_right.png).

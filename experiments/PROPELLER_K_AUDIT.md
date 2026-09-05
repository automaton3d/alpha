# Auditoria do experimento P + K

O sucesso anterior foi obtido com uma regra exclusiva do ensaio. Nao e
evidencia de transporte por regras locais do AC. A macro de voo foi removida
do kernel e do build; o executavel agora exige o resultado do kernel comum.
Uma falha desse teste e uma falha de aceitacao, nao deve ser convertida em PASS.

Resultado observado: a semente coincidente com fase zero falhou no primeiro
quadro (tick 271, tubo 17x7x7, W=3): dx_K=dx_P1=dx_P2=0, quando o teste
exigia dx_P1=dx_P2=1. A matriz de 36 combinacoes de fase P/K (0..5), com
janela de 12 quadros, falhou em todos os casos, com codigo de saida 2.
O comando `experiments\audit_propeller_k.bat` reproduz a matriz e retorna
falha se qualquer caso falhar. Logs e resumo ficam em build/propeller_k.

## Atalhos ainda presentes no kernel comum

| Caminho | Dependencia nao local | Trabalho necessario |
| --- | --- | --- |
| simulation.cpp: update_pulsating_wavefront | le lcenters para calcular incrementos e reafirma r2=0 | propagar geometria pelo estado dos vizinhos, sem consultar centro remoto |
| simulation.cpp: phase_step | consulta o relogio remoto da fonte para ativar cada voxel | transportar/sincronizar fase por transicoes locais |
| interaction.cpp: beginSourceTick, encounter | snapshots globais das fontes; contato escreve na fonte remota | registrar contato no voxel e propagar a resposta ate a fonte |
| interaction.cpp: resolveInternalContacts | lista global de contatos, centros e arbitragem | resolver interacoes com estado e comunicacao locais |
| simulation.cpp: applyMomentum | desloca toda a camada com a decisao da fonte | executar transporte por vizinhos, com sinais locais e latencia explicita |
| simulation.cpp: contato W | recupera estado atual por partner.w | validar o transporte W e sua temporizacao, sem acesso remoto por identidade |

Iterar a grade na CPU e usar buffers sincronos nao e por si um atalho.
O problema e a dependencia de estados fora da vizinhanca permitida.
Remover essas dependencias exige implementar as transicoes correspondentes;
simplesmente desliga-las nao implementa a mecanica proposta.

## Condicoes e verificacoes

Preparar um K e um par reciproco e uma condicao inicial autorizada pelo
experimento; nao demonstra sua formacao espontanea. Afinidade ja preenchida
na camada e relogios sincronizados tambem sao preparacoes, nao resultados.
O argumento opcional PHASE_K permite testar dessincronizacao sem alterar
a evolucao. As duas metades de P continuam inicialmente sincronizadas.

O observador agora conta fontes r2=0 na grade inteira e verifica unicidade
e concordancia com o cache lcenters, sem reparar o estado. Isso fecha uma
lacuna de medicao, mas nao transforma o transporte do kernel em regra local.

Antes de aceitar o mecanismo ainda faltam testes de vizinhanca causal por
tick, permutacao das camadas W, seis direcoes espaciais, estados iniciais
transladados, fases independentes, conservacao do estado transportado e
remocao de P apos a captura. A velocidade por quadro nao substitui a
verificacao da propagacao causal por tick. Cada PASS deve especificar o
que verificou; nenhum teste cinematico certifica localidade sozinho.

Os CSVs pk_right/left/foreign, o PNG e o GIF existentes sao registros
historicos da variante retirada. Nao sao resultados do kernel comum atual.

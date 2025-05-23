# Across the bullets

Across the bullets não é somente um jogo, é uma experiência. Foi nessa linha de reflexão que desenvolvemos um survival game interativo, com uma trilha sonora fantástica, elementos visuais bem definidos e uma rica gama de efeitos sonoros, em que você, jogador, será posto na linha de frente em uma terra desconhecida, aparentemente serena, porém, imersa no caos de um fogo cruzado sem fim! Sobreviva o máximo que puder! Construa barricadas em busca de uma mísera esperança de vida, colete porções para ganhar mais tempo. Surpreenda seus amigos provando que você consegue sobreviver em Across The Bullets <strong>o maior tempo possível!</strong>


<details>
<summary><h2>Equipe</h2></summary>


* [Amanda Luz Chaves](https://github.com/amandaaluzc) - alc2@cesar.school  
* [Lucas Menezes Santana](https://github.com/ucasmenezes08) - lms4@cesar.school  
* [João Carlos Melo Brennand de Souza Mendes](https://github.com/joaocm1804) - jcmbsm@cesar.school  

</details>






<details>
<summary><h2>Como Funciona</h2></summary>



<strong>Informações gerais: </strong>


- O jogo consiste em sobreviver pelo maior tempo possível.

- No contexto geral, parece simples, porém você, jogador, será lançado ao meio do mapa, tendo que desviar das balas emergentes do canto externo do mapa.

- A quantidade de balas geradas irá aumentar com o decorrer do tempo. Além disso, conforme o andamento do jogo, a velocidade das balas tambem será incrementada.





<strong>Informações Necessárias: </strong>



- O jogador contém *3 vidas* , que serão consumidas a cada colisão com a bala.

- O jogador contém *5 barricadas*, que podém ser usadas para bloquear as balas. (Recomendamos utilizar para fins de emergencia, no caso para coletar vidas extras ou ganhar mais tempo para fuga). Vale ressaltar que a madeira será gerada após o jogador pressionar a tecla *SPACE*, sendo posicionada exatamente no local onde o jogador apertou.

- As barricadas possuem tempo de duração após a ativação de 10 segundos até o desaparecimento.

- As barricadas conseguem cobrir até somente uma colisão.

- Após utilizada, não é possivel recuperar a barricada gasta, use somente se necessário.

- O jogador pode coletar *Porções de vida* distribuidas pelo mapa, que fornecem ,ao serem coletadas, vida extra para o jogador.

- As porções só fornecem vida extra, *se o jogador estiver com menos de 3 vidas*, caso o jogador esteja com as 3 vidas, *a porção somente irá mudar de posição*.

- É importante ressaltar que as vidas extras são geradas em uma posição aleatória do mapa, e *Desaparecem após 15 segundos* se não coletadas.

- Caso a porção seja coletada, ela irá reaparecer após 10 segundos. Dica: Se a porção aparece em um lugar muito extremo no inicio do game, é recomendado coletar para que reapareça em algum lugar mais favorável. (No decorrer do jogo, será difícil coletar na extremidade).

</details>





<details>
<summary><h2>Instruções de instalação</h2></summary>




### Instalar Raylib
Linux:

    sudo apt install build-essential git
    

Ubuntu:

    sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
    

Fedora: 

    sudo dnf install alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel libatomic


MacOS:

    brew install raylib


### Comando para compilação e funcionamento do código

Execute esses comandos no terminal:

    git clone https://github.com/joaocm1804/across-the-bullets.git
    
Depois:
    
    cd across-the-bullets
    
E então:
    
    make run
    

Caso ao executar o jogo der erro de permissão, execute: 
    
    chmod +x main

</details>




<details>
<summary><h2>Detalhes de implementação</h2></summary>


### Lista de funções

- Main(): Função principal onde roda o jogo;

- InitGame(): Onde todas as imagens, sons e outras variáveis do jogo são carregadas;

- DrawGame(): Possui a lógica das inicializações do que deve ser desenhado no jogo, assim como    
suas dimensões e posições;

- reiniciar(): Ocorre o reinício de todas as variáveis após uma partida acabar;

- UnloadGame(): Todos os arquivos são descarregados após fechamento do jogo

- add_inicio(): insere os dados do jogador numa lista encadeada;

- ordenar(): ordena a lista encadeada com os dados dos jogadores;

- salvarRanking(): escreve os dados da lista ordenada num arquivo “.txt”;

- carregarRanking(): limpa a lista de ranking e lê os dados anteriormente já armazenados no “.txt”;

- printarLeaderboard(): organiza o draw do ranking e imprime os dados dos jogadores;

- limparRanking(): limpa os dados da lista encadeada do ranking para não haver duplicidade.



### Principais dificuldades enfrentadas.


- Conciliar o tempo para o desenvolvimento do jogo com outras matérias e projetos;

- Entender funcionamento da Raylib e de como desenvolver um jogo;

- Funcionamento do surgimento aleatório das balas.

- Geração de assets para o jogo, pois queríamos fazer todos únicos.

- Animação de movimentação do player

- Ajustar zona de hitbox dos assets da partida

- Ajustar dimensões dos assets



</details>

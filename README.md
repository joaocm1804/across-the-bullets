# Across the bullets

Across the bullets não é somente um jogo, é uma experiência. Foi nessa linha de reflexão que desenvolvemos um survival game interativo, com uma trilha sonóra fantástica,elementos visuais bem definidos e uma rica gama de efeitos sonoros , em que você , jogador, será posto na linha de frente em uma terra desconhecida aparente serena , porém imersa no caos de um fogo cruzado sem fim! Sobreviva o máximo que puder! Construa barricadas em busca de uma mísera esperança de vida, colete porções para ganhar mais tempo. Surpreenda seus amigos provando que você consegue sobreviver Across The Bullets *o maior tempo possível!*


<details>
<summary><strong>Como Funciona</strong></summary>

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
<summary><strong>Instruções de instalação</strong></summary>

### Instalar Raylib
Linux:

    1. sudo apt install build-essential git
    

Ubuntu:

    1. sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
    

Fedora: 

    1. sudo dnf install alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel libatomic


MacOS:

    1. brew install raylib


### Comando para compilação e funcionamento do código
    
Execute esse comando no terminal:
    
    1. make run
    

Caso ao executar o jogo der erro de permissão, execute: 
    
    1. chmod +x main

</details>


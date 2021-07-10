# Processo Seletivo - ISI

Pre-requisites (the output below shows the versions used during testing):

    gcc

    $ gcc --version
gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    make

    $ make --version
GNU Make 4.2.1
Built for x86_64-pc-linux-gnu
Copyright (C) 1988-2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
    

Definir variável de ambiente FREERTOS_PATH apontando para o diretório onde o freeRTOS está instalado na sua máquina.

1. Execute _make_ a partir do diretório base

2. Executar em /build/executavel.exe a partir do diretório base.


##Funcionamento 
- Tarefa_1, simule a leitura de uma senóide com frequência de 60Hz por um ADC, a amostragem deste sinal deve ser realizada a cada 1ms, e salva em um buffer. O buffer de armazenamento deve possuir 1000 posições. 

- Tarefa_2, a cada 100ms processe o sinal salvo pela tarefa_1, multiplique o valor amostrado por 3.141592 e guarde esse valor em outro buffer de 1000 posições. Uma amostra nunca deve ser multiplicada por 3.141592 mais de uma vez.

- Tarefa_3, disponibilize uma interface serial para o usuário, quando ele digitar "obter" recebe os dados processados, e quando digitar "zerar" ele limpa os buffers.

tarefa_4 não foi implementada.

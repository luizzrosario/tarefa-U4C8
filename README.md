# **Controle de Joystick, LED RGB e Display OLED**

Este projeto implementa o controle de um **Joystick**, um **LED RGB** e um **Display OLED SSD1306** usando um **Raspberry Pi Pico**. O código realiza a leitura do joystick para mover um quadrado no display OLED e controla o brilho dos LEDs RGB com base nos valores dos eixos do joystick.

## 🔧 **Sobre o Projeto**
- O código lê os valores do **Joystick** (eixos X e Y) conectados aos pinos **GP26** e **GP27**, e controla um quadrado no display OLED de **128x64 pixels**.
- O código também controla um **LED RGB** (vermelho, verde e azul) usando **PWM**. O brilho do LED é ajustado com base nas leituras dos eixos do joystick.
- O **botão A** alterna o estado do PWM e o **botão do joystick** (GP22) alterna o LED verde, além de contar quantos vezes o botão foi pressionado.
- O display OLED exibe um quadrado que se move de acordo com a posição do joystick.

## 🛠 **Funcionalidades**
- **Joystick**: Leitura dos eixos X e Y para movimentar um quadrado no display OLED.
- **LED RGB**: Controle do brilho dos LEDs com base nas leituras do joystick (eixos X e Y).
- **Botões**: Alternância entre os estados do PWM e LED verde ao pressionar os botões.

## 🧑‍💻 **Instruções de Instalação**

1. **Clonar o repositório**:
   ```bash
   git clone https://github.com/luizzrosario/tarefa-U4C8.git
   ```

2. **Compilar o código**:
   Utilize o CMake para compilar o código para o **RP2040**.

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Carregar o código no RP2040**:
   Copie o arquivo compilado para a memória flash do microcontrolador.

4. **Conectar via USB**:
   Conecte a Raspberry Pi Pico ao computador via cabo USB e abra um terminal serial para visualizar as saídas.

## 🛠 **Configuração no Wokwi**

Para simular o projeto no **Wokwi**, siga estes passos:

1. Acesse o site do Wokwi: [https://wokwi.com](https://wokwi.com)
2. Crie um novo projeto para **Raspberry Pi Pico**.
3. Adicione os seguintes componentes:
   - **Joystick** com os pinos **GP22 (Botão)**, **GP26 (Eixo Y)** e **GP27 (Eixo X)**.
   - **LED RGB** com os pinos **GP13 (Vermelho)**, **GP11 (Verde)** e **GP12 (Azul)**.
   - **Display OLED** conectado ao **I2C (GP14 SDA e GP15 SCL)**.
4. Copie e cole o código do projeto.
5. Clique em **Run** para iniciar a simulação.

---

## 📝 **Esquema de Conexão**

- **Joystick**:
  - Pino X: **GP27**
  - Pino Y: **GP26**
  - Botão: **GP22**

- **LED RGB**:
  - Vermelho: **GP13**
  - Verde: **GP11**
  - Azul: **GP12**

- **Display OLED**:
  - SDA: **GP14**
  - SCL: **GP15**

---

## 🔄 **Alterações Possíveis**
- **PWM**: O código pode ser modificado para ajustar as frequências ou a intensidade do PWM para os LEDs.
- **Joystick**: Adicionar mais funcionalidades ou ajustar a área de movimento do quadrado no display OLED.
- **Display**: Pode-se adicionar mais elementos visuais ou modificar o conteúdo exibido no display OLED.

---

## 📺 **Demonstração em Vídeo**
Confira um vídeo demonstrativo no YouTube: [[Vídeo Tarefa - U4C8](https://youtu.be/sS76mvEhUtw)]

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Definições de pinos
#define JOYSTICK_BUTTON_PIN 22 // Pino do botão do joystick
#define JOYSTICK_X_PIN 27      // Pino do eixo X do joystick
#define JOYSTICK_Y_PIN 26      // Pino do eixo Y do joystick
#define LED_R_PIN 13           // Pino do LED Vermelho
#define LED_G_PIN 11           // Pino do LED Verde
#define LED_B_PIN 12           // Pino do LED Azul
#define BUTTON_A_PIN 5         // Pino do botão A
#define I2C_PORT i2c1          // Porta I2C para comunicação com o display
#define I2C_SDA 14             // Pino SDA para I2C
#define I2C_SCL 15             // Pino SCL para I2C
#define DISPLAY_ADDR 0x3C      // Endereço do display OLED

// Tamanho do display OLED
#define WIDTH 128
#define HEIGHT 64

// Estrutura para o display SSD1306
ssd1306_t ssd;

// Variáveis de controle
volatile bool led_green_on = false; // Controle do LED verde
volatile bool pwm_enabled = true;   // Habilitação do PWM
static int apertos = 0;             // Contador de apertos do botão do joystick

// Funções para ler valores do joystick
// Lê o valor do eixo X do joystick
uint16_t read_joystick_x()
{
    adc_select_input(1);
    return adc_read();
}

// Lê o valor do eixo Y do joystick
uint16_t read_joystick_y()
{
    adc_select_input(0);
    return adc_read();
}

// Configuração dos LEDs RGB
void init_rgb_led()
{
    // Configura os pinos para PWM
    gpio_set_function(LED_R_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_G_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_B_PIN, GPIO_FUNC_PWM);

    // Define o limite máximo do PWM (255)
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_R_PIN), 255);
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_G_PIN), 255);
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_B_PIN), 255);

    // Habilita o PWM para os LEDs
    pwm_set_enabled(pwm_gpio_to_slice_num(LED_R_PIN), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(LED_G_PIN), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(LED_B_PIN), true);
}

// Ajusta o brilho do LED
void set_led_brightness(uint pin, uint16_t value)
{
    uint8_t brightness = (value > 1940 && value < 2200) ? 0 : abs(value - 2048) / 16;
    if (!pwm_enabled)
        brightness = 0; // Desliga o PWM se pwm_enabled for falso
    pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin), brightness);
}

// Desenha e move um quadrado no display OLED
void move_square(ssd1306_t *ssd, int x, int y)
{
    static int prev_x = WIDTH / 2 - 4;  // Posição inicial X do quadrado
    static int prev_y = HEIGHT / 2 - 4; // Posição inicial Y do quadrado

    // Apaga o quadrado anterior
    ssd1306_rect(ssd, prev_y, prev_x, 8, 8, false, true);

    // Atualiza a posição do quadrado
    prev_x = x;
    prev_y = y;

    // Desenha o quadrado na nova posição
    ssd1306_rect(ssd, prev_y, prev_x, 8, 8, true, true);
    ssd1306_send_data(ssd);
}

// Interrupção dos botões
void button_isr(uint gpio, uint32_t events)
{
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // Evita debounce nos botões
    if (current_time - last_time < 500)
        return;
    last_time = current_time;

    if (gpio == BUTTON_A_PIN)
    {
        // Alterna o estado do PWM
        pwm_enabled = !pwm_enabled;
        printf("PWM %s\n", pwm_enabled ? "Ligado" : "Desligado");
    }
    else if (gpio == JOYSTICK_BUTTON_PIN)
    { // Corrigido para o botão do joystick
        // Alterna o estado do LED verde
        led_green_on = !led_green_on;
        pwm_set_chan_level(pwm_gpio_to_slice_num(LED_G_PIN), pwm_gpio_to_channel(LED_G_PIN), led_green_on ? 255 : 0);
        printf("Botão do Joystick pressionado! LED Verde: %s\n", led_green_on ? "Aceso" : "Apagado");
        apertos++; // Incrementa o contador de apertos
        printf("Número de apertos do joystick: %d\n", apertos);
    }
}

// Inicializa os botões com interrupções
void init_buttons()
{
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);

    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);
}

int main()
{
    // Inicializa todos os componentes
    stdio_init_all();
    init_rgb_led();
    init_buttons();

    // Configura os pinos dos joysticks e ADCs
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Inicializa a comunicação I2C para o display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    while (1)
    {
        // Desenha a borda no display caso o botão do joystick seja pressionado
        ssd1306_rect(&ssd, 1, 1, 125, 63, led_green_on, !led_green_on);

        // Lê os valores do joystick
        uint16_t x_value = read_joystick_x();
        uint16_t y_value = read_joystick_y();

        // Ajusta o brilho dos LEDs com base no valor do joystick
        set_led_brightness(LED_R_PIN, x_value);
        set_led_brightness(LED_B_PIN, y_value);

        // Calcula as posições para o movimento do quadrado
        int square_x = (x_value * (WIDTH - 8)) / 4095;
        int square_y = HEIGHT - 8 - (y_value * (HEIGHT - 8)) / 4095;

        // Move o quadrado no display
        move_square(&ssd, square_x, square_y);

        // Espera 10ms antes de atualizar novamente
        sleep_ms(10);
    }
}

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Definições de pinos
#define JOYSTICK_BUTTON_PIN 22
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BUTTON_A_PIN 5
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDR 0x3C

#define WIDTH 128
#define HEIGHT 64

ssd1306_t ssd;
volatile bool led_green_on = false;
volatile bool pwm_enabled = true;

// Funções para ler valores do joystick
uint16_t read_joystick_x() {
    adc_select_input(1);
    return adc_read();
}

uint16_t read_joystick_y() {
    adc_select_input(0);
    return adc_read();
}

// Configuração dos LEDs RGB
void init_rgb_led() {
    gpio_set_function(LED_R_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_G_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_B_PIN, GPIO_FUNC_PWM);

    pwm_set_wrap(pwm_gpio_to_slice_num(LED_R_PIN), 255);
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_G_PIN), 255);
    pwm_set_wrap(pwm_gpio_to_slice_num(LED_B_PIN), 255);

    pwm_set_enabled(pwm_gpio_to_slice_num(LED_R_PIN), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(LED_G_PIN), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(LED_B_PIN), true);
}

void set_led_brightness(uint pin, uint16_t value) {
    uint8_t brightness = (value > 1940 && value < 2200) ? 0 : abs(value - 2048) / 16;
    if (!pwm_enabled) brightness = 0;
    pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin), brightness);
}

void move_square(ssd1306_t *ssd, int x, int y) {
    static int prev_x = WIDTH / 2 - 4;
    static int prev_y = HEIGHT / 2 - 4;

    ssd1306_rect(ssd, prev_y, prev_x, 8, 8, false, true);
    prev_x = x;
    prev_y = y;
    ssd1306_rect(ssd, prev_y, prev_x, 8, 8, true, true);
    ssd1306_send_data(ssd);
}

void button_isr(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_time < 200) return;  // Evita debounce
    last_time = current_time;

    if (gpio == BUTTON_A_PIN) {
        pwm_enabled = !pwm_enabled;
        printf("PWM %s\n", pwm_enabled ? "Ligado" : "Desligado");
    } else if (gpio == JOYSTICK_BUTTON_PIN) {  // Corrigido para tratar o botão do joystick
        led_green_on = !led_green_on;
        pwm_set_chan_level(pwm_gpio_to_slice_num(11), pwm_gpio_to_channel(11), led_green_on ? 255 : 0);
        printf("Botão do Joystick pressionado! LED Verde: %s\n", led_green_on ? "Aceso" : "Apagado");
    }
}

void init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);

    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);
}

int main() {
    stdio_init_all();
    init_rgb_led();
    init_buttons();

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    while (1) {
        uint16_t x_value = read_joystick_x();
        uint16_t y_value = read_joystick_y();
        printf("Pino do botão joystick: %d\n", gpio_get(JOYSTICK_BUTTON_PIN));

        set_led_brightness(LED_R_PIN, x_value);
        set_led_brightness(LED_B_PIN, y_value);

        int square_x = (x_value * (WIDTH - 8)) / 4095;
        int square_y = HEIGHT - 8 - (y_value * (HEIGHT - 8)) / 4095;
        move_square(&ssd, square_x, square_y);

        sleep_ms(10);
    }
}

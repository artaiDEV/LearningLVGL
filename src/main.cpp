#include <Arduino.h>

// Inclusións
#include <lvgl.h> 
#include <TFT_eSPI.h>
#include "CST820.h" // Driver do touch CST820

// ==========================================================
// CONFIGURACIÓN DO DISPLAY E DO TOUCH
// ==========================================================

static const uint16_t screenWidth = 240; // 320x240 nativo, pero o LVGL quere 320x240
static const uint16_t screenHeight = 320; 

// Pins do Touch CST820 (Confirmados polo teu código)
#define I2C_SDA 33
#define I2C_SCL 32
#define TP_RST 25
#define TP_INT 21

// Drivers
TFT_eSPI tft = TFT_eSPI();                      /* TFT instance */
CST820 touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT); /* Touch instance */

// Buffers de LVGL
#define LVGL_BUFFER_SIZE (screenWidth * 200) // Usamos o tamaño do teu exemplo
static lv_color_t *buf1;
static lv_color_t *buf2;

// Obxecto Display de LVGL v9.4
static lv_display_t *disp;


// ==========================================================
// DRIVERS DE LVGL (FLUSH e READ)
// ==========================================================

/* Display Flush (Refresco) Callback para LVGL v9.4 */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // TFT_eSPI usa pushImage para enviar o buffer
    tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
    
    // Notificar a LVGL de que o baleirado rematou (CRUCIAL)
    lv_display_flush_ready(disp);
}

/* Touch Read (Lectura do Touchpad) Callback para LVGL v9.4 */
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    uint8_t gesture;
    bool touched = touch.getTouch(&touchX, &touchY, &gesture);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL; // Estado de solta (Released)
    }
    else
    {
        data->state = LV_INDEV_STATE_PR; // Estado de premido (Pressed)

        // Se a túa TFT está en Rotación 1 (320x240), axusta as coordenadas se é necesario
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

// ==========================================================
// SETUP
// ==========================================================

void setup()
{
    Serial.begin(115200);

    lv_init();

    // 0. Inicialización do Hardware
    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH); // Backlight ON

    tft.begin();        
    tft.setRotation(1); // Rotación 1 (horizontal: 320x240)
    touch.begin(); 

    // 1. Inicialización dos Buffers
    buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_BUFFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    // buf2 non é obrigatorio se usamos LV_DISPLAY_RENDER_MODE_PARTIAL (o máis eficiente)
    // Se o teu exemplo usaba dous buffers:
    // buf2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_BUFFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    // 2. Creación do Obxecto Display (v9.4)
    disp = lv_display_create(screenWidth, screenHeight); 
    
    // 3. Asignación de buffers (v9.4)
    lv_display_set_buffers(disp, buf1, NULL, LVGL_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL); 
    
    // 4. Configuración do Display Driver (v9.4)
    lv_display_set_flush_cb(disp, my_disp_flush);
    // Nota: O Tick/Temporizador xa non é estritamente necesario na v9.x se loop() é chamado rápido,
    // pero engadiremos lv_tick_inc(5) en loop() para a compatibilidade total.

    // 5. Inicialización do Input Driver (Touch) (v9.4)
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    // 6. Creación do Widget de Proba (LVGL v9.4)
    lv_obj_t *scr = lv_display_get_screen_active(disp); 
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "LVGL v9.4 con TFT_eSPI!");
    lv_obj_center(label);

    Serial.println("Setup done. Verificando LVGL...");
}

void loop()
{
    // O Tick de LVGL (5ms)
    lv_tick_inc(5); 

    // Manexo de Tarefas de LVGL
    lv_timer_handler(); 
    delay(5); // Retardo para aforrar CPU
}
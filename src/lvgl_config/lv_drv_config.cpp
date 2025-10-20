#include "lv_drv_config.h"

// ==========================================================
// 1. OBXECTOS GLOBAIS
// ==========================================================

// Obxectos drivers
TFT_eSPI tft = TFT_eSPI(); 
CST820 touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT); 

// Buffers de LVGL
#define LVGL_BUFFER_SIZE (screenWidth * 40) 
static lv_color_t *buf1;

// ==========================================================
// 2. DISPLAY FLUSH (BALDEIRADO)
// ==========================================================

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
    lv_display_flush_ready(disp);
}

// ==========================================================
// 3. TOUCHPAD READ (LECTURA DO TÁCTIL)
// ==========================================================

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    uint8_t gesture;
    bool touched = touch.getTouch(&touchX, &touchY, &gesture);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL; 
    }
    else
    {
        data->state = LV_INDEV_STATE_PR; 
        
        // Mapeo do Touch (mantido do último paso)
        data->point.x = touchX;
        data->point.y = touchY;
        
        // Imprimir as coordenadas cando se detecta unha presión
        Serial.print("Touch Detectado: X=");
        Serial.print(touchX);
        Serial.print(", Y=");
        Serial.println(touchY);
        
    }
}

// ==========================================================
// 4. FUNCIÓN DE INICIALIZACIÓN PRINCIPAL
// ==========================================================

void lvgl_driver_init(void)
{
    // --- 1. Inicialización do Hardware TFT_eSPI ---
    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH); // Backlight ON

    tft.begin();        
    tft.setRotation(1);     // Rotación 0 (Modo Horizontal, 320x240)
    tft.setSwapBytes(false); // Xa corrixido en lv_conf.h

    // --- 2. Inicialización do Hardware CST820 ---
    touch.begin(); 
    
    // --- 3. Inicialización dos Buffers de LVGL ---
    buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_BUFFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!buf1) { Serial.println("Fallo ao asignar buf1!"); while(1); }

    lv_display_t *disp = lv_display_create(screenWidth, screenHeight); 
    lv_display_set_buffers(disp, buf1, NULL, LVGL_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL); 
    lv_display_set_flush_cb(disp, my_disp_flush);

    // --- 4. Inicialización do Input Driver (Touch) ---
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);
}
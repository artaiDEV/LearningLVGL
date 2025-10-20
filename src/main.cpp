#include <Arduino.h>
#include <lvgl.h> 
#include "lvgl_config/lv_drv_config.h" // O noso novo ficheiro de configuración

// ==========================================================
// SETUP
// ==========================================================

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando aplicación LVGL v9.4...");

    lv_init();

    // Chamamos ao noso inicializador de drivers de display e input
    lvgl_driver_init(); 
    
    // Creación do Widget de Proba
    lv_obj_t *scr = lv_display_get_screen_active(NULL); 
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "LVGL / TFT_eSPI / CST820 OK!");
    lv_obj_center(label);

    Serial.println("LVGL listo. Podes interactuar coa pantalla.");
}

// ==========================================================
// LOOP
// ==========================================================

void loop()
{
    // O Tick de LVGL (5ms)
    lv_tick_inc(5); 

    // Manexo de Tarefas de LVGL
    lv_timer_handler(); 
    delay(5); 
}
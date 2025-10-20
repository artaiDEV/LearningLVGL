#ifndef LV_DRV_CONFIG_H
#define LV_DRV_CONFIG_H

#include <lvgl.h>
#include <TFT_eSPI.h>
#include "CST820.h" // Incluímos o driver táctil aquí

// ==========================================================
// 1. CONFIGURACIÓN DE PINS E DIMENSIÓNS
// ==========================================================

// Pins do Touch CST820 (Confirmados)
#define I2C_SDA 33
#define I2C_SCL 32
#define TP_RST 25
#define TP_INT 21

// Dimensións sincronizadas (320x240)
static const uint16_t screenWidth = 320; 
static const uint16_t screenHeight = 240; 

// ==========================================================
// 2. DECLARACIÓN DE FUNCIÓNS DE LVGL
// ==========================================================

// Función principal de inicialización de Display e Input
void lvgl_driver_init(void);

// Callback para o display (Flush)
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

// Callback para o táctil (Read)
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);

#endif
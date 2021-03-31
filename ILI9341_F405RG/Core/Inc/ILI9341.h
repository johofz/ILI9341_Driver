/*
 * ILI9341.h
 *
 *  Created on: Mar 22, 2021
 *      Author: jh
 */

#ifndef INC_ILI9341_H_
#define INC_ILI9341_H_

#include "main.h"
#include "ILI9341_Instructions.h"


#define RST_PIN     GPIO_PIN_12
#define RST_PORT    GPIOA
#define CS_PIN      GPIO_PIN_11
#define CS_PORT     GPIOA
#define RS_PIN      GPIO_PIN_10
#define RS_PORT     GPIOA
#define WR_PIN      GPIO_PIN_9
#define WR_PORT     GPIOA
#define RD_PIN      GPIO_PIN_8
#define RD_PORT     GPIOA

#define BUS_PORT    GPIOC
#define BUS_OFFSET  4

#define SELECT()    CS_PORT->BSRR = (uint32_t) CS_PIN << 16U
#define DESELECT()  CS_PORT->BSRR = CS_PIN
#define COMMAND()   RS_PORT->BSRR = (uint32_t) RS_PIN << 16U
#define DATA()      RS_PORT->BSRR = RS_PIN
#define WRITE()     WR_PORT->BSRR = (uint32_t) WR_PIN << 16U;\
		            WR_PORT->BSRR = WR_PIN

namespace ILI9341
{
static uint16_t _width = 240;
static uint16_t _height = 320;
static uint32_t _pixelCount = _width * _height;

void SendCommand(uint8_t command);
void SendData(uint8_t data);
void SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void Init();
void Reset();
void SetRotation(uint8_t _rotation);

void DrawPixel(uint16_t x, uint16_t y, uint16_t color);

void Fill(uint16_t color);

void DrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void FillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);


void DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
        uint16_t color);
void FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
        uint16_t color);

void DrawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size);
void Print(char text[], int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size);

void Image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data, uint32_t size);
}

#endif /* INC_ILI9341_H_ */

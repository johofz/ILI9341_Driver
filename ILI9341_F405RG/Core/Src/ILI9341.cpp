/*
 * ILI9341.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: jh
 */

#include "ILI9341.h"

void ILI9341::SendCommand(uint8_t command)
{
    SELECT();
    BUS_PORT->BSRR = (uint32_t) (0xFF << BUS_OFFSET) << 16U;
    BUS_PORT->BSRR = command << BUS_OFFSET;

    COMMAND();
    WRITE();
    DESELECT();
}

void ILI9341::SendData(uint8_t data)
{
    SELECT();
    BUS_PORT->BSRR = (uint32_t) (0xFF << BUS_OFFSET) << 16U;
    BUS_PORT->BSRR = data << BUS_OFFSET;

    DATA();
    WRITE();
    DESELECT();
}

void ILI9341::SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    SendCommand(ILI9341_COLUMN_ADDR);
    SendData(x1 >> 8);
    SendData(x1 & 0xFF);
    SendData(x2 >> 8);
    SendData(x2 & 0xFF);

    SendCommand(ILI9341_PAGE_ADDR);
    SendData(y1 >> 8);
    SendData(y1 & 0xFF);
    SendData(y2 >> 8);
    SendData(y2 & 0xFF);

    SendCommand(ILI9341_GRAM);
}

void ILI9341::Init()
{
    SendCommand(ILI9341_RESET);    // software reset comand
    HAL_Delay(100);
    SendCommand(ILI9341_DISPLAY_OFF);    // display off
    //------------power control------------------------------
    SendCommand(ILI9341_POWER1);    // power control
    SendData(0x26);    // GVDD = 4.75v
    SendCommand(ILI9341_POWER2);    // power control
    SendData(0x01);    // AVDD=VCIx2, VGH=VCIx7, VGL=-VCIx3
    //--------------VCOM-------------------------------------
    SendCommand(ILI9341_VCOM1);    // vcom control
    SendData(0x35);    // Set the VCOMH voltage (0x35 = 4.025v)
    SendData(0x3e);    // Set the VCOML voltage (0x3E = -0.950v)
    SendCommand(ILI9341_VCOM2);    // vcom control
    SendData(0xbe);

    //------------memory access control------------------------
    SendCommand(ILI9341_MAC);    // memory access control
    SendData(0x48);

    SendCommand(ILI9341_PIXEL_FORMAT);    // pixel format set
    SendData(0x55);    // 16bit /pixel

    SendCommand(ILI9341_FRC);
    SendData(0);
    SendData(0x1F);
    //-------------ddram ----------------------------
    SendCommand(ILI9341_COLUMN_ADDR);    // column set
    SendData(0x00);    // x0_HIGH---0
    SendData(0x00);    // x0_LOW----0
    SendData(0x00);    // x1_HIGH---240
    SendData(0xEF);    // x1_LOW----240
    SendCommand(ILI9341_PAGE_ADDR);    // page address set
    SendData(0x00);    // y0_HIGH---0
    SendData(0x00);    // y0_LOW----0
    SendData(0x01);    // y1_HIGH---320
    SendData(0x3F);    // y1_LOW----320

    SendCommand(ILI9341_TEARING_OFF);    // tearing effect off
    //LCD_write_cmd(ILI9341_TEARING_ON); // tearing effect on
    //LCD_write_cmd(ILI9341_DISPLAY_INVERSION); // display inversion
    SendCommand(ILI9341_Entry_Mode_Set);    // entry mode set
    // Deep Standby Mode: OFF
    // Set the output level of gate driver G1-G320: Normal display
    // Low voltage detection: Disable
    SendData(0x07);
    //-----------------display------------------------
    SendCommand(ILI9341_DFC);    // display function control
    //Set the scan mode in non-display area
    //Determine source/VCOM output in a non-display area in the partial display mode
    SendData(0x0a);
    //Select whether the liquid crystal type is normally white type or normally black type
    //Sets the direction of scan by the gate driver in the range determined by SCN and NL
    //Select the shift direction of outputs from the source driver
    //Sets the gate driver pin arrangement in combination with the GS bit to select the optimal scan mode for the module
    //Specify the scan cycle interval of gate driver in non-display area when PTG to select interval scan
    SendData(0x82);
    // Sets the number of lines to drive the LCD at an interval of 8 lines
    SendData(0x27);
    SendData(0x00);    // clock divisor

    SendCommand(ILI9341_SLEEP_OUT);    // sleep out
    HAL_Delay(100);
    SendCommand(ILI9341_DISPLAY_ON);    // display on
    HAL_Delay(100);
    SendCommand(ILI9341_GRAM);    // memory write
    HAL_Delay(5);
}

void ILI9341::Reset()
{
    RST_PORT->BSRR = (uint32_t) RST_PIN << 16U;    // RST Low
    HAL_Delay(100);
    RST_PORT->BSRR = RST_PIN;    // RST High
}

void ILI9341::SetRotation(uint8_t _rotation)
{
    uint8_t rotation = _rotation % 4;

    SendCommand(ILI9341_MAC);

    switch (rotation) {
        case 0:
            SendData(0x40 | 0x08);
            _width = 240;
            _height = 320;
            break;
        case 1:
            SendData(0x20 | 0x08);
            _width = 320;
            _height = 240;
            break;
        case 2:
            SendData(0x80 | 0x08);
            _width = 240;
            _height = 320;
            break;
        case 3:
            SendData(0x40 | 0x80 | 0x20 | 0x08);
            _width = 320;
            _height = 240;
            break;
    }
}

void ILI9341::DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    SetCursorPosition(x, y, x, y);
    SendData(color >> 8);
    SendData(color & 0xFF);
}

void ILI9341::Fill(uint16_t color)
{
    FillRect(0, 0, _width - 1, _height - 1, color);
}

void ILI9341::DrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t w = x1 - x0;
    uint16_t h = y1 - y0;

    DrawFastHLine(x0, y0, w, color);
    DrawFastHLine(x0, y1, w, color);
    DrawFastVLine(x0, y0, h, color);
    DrawFastVLine(x1, y0, h, color);
}

void ILI9341::FillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint32_t n = ((x1 + 1) - x0) * ((y1 + 1) - y0);

    if (n > _pixelCount)
        n = _pixelCount;

    SetCursorPosition(x0, y0, x1, y1);

    while (n)
    {
        n--;
        SendData(color >> 8);
        SendData(color & 0xff);
    }
}

void ILI9341::DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
}

void ILI9341::FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
}

void ILI9341::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
}

void ILI9341::DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    SetCursorPosition(x, y, x + w, y);

    uint32_t n = w;

    while (n)
    {
        n--;
        SendData(color >> 8);
        SendData(color & 0xff);
    }
}

void ILI9341::DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    SetCursorPosition(x, y, x, y + h);

    uint32_t n = h;

    while (n)
    {
        n--;
        SendData(color >> 8);
        SendData(color & 0xff);
    }
}

void ILI9341::DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
        uint16_t color)
{
}

void ILI9341::FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
        uint16_t color)
{
}

void ILI9341::DrawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{
    if ((x >= _width) ||    // Clip right
            (y >= _height) ||    // Clip bottom
            ((x + 6 * size - 1) < 0) ||    // Clip left
            ((y + 8 * size - 1) < 0))    // Clip top
        return;

    if (c >= 176)
        c++;    // Handle 'classic' charset behavior

    for (int8_t i = 0; i < CHARACTER_BASE_WIDTH; i++)
    {
        uint8_t line;
        if (i == 5)
            line = 0x0;
        else
            line = pgm_read_byte(font1 + (c * 5) + i);
        for (int8_t j = 0; j < CHARACTER_BASE_HEIGHT; j++)
        {
            if (line & 0x1)
            {
                if (size == 1)    // default size
                    DrawPixel(x + i, y + j, color);
                else
                {    // big size
                    FillRect(x + (i * size), y + (j * size), size + x + (i * size),
                            size + 1 + y + (j * size), color);
                }
            }
            else if (bg != color)
            {
                if (size == 1)    // default size
                    DrawPixel(x + i, y + j, bg);
                else
                {    // big size
                    FillRect(x + i * size, y + j * size, size + x + i * size,
                            size + 1 + y + j * size, bg);
                }
            }
            line >>= 1;
        }
    }
}

void ILI9341::Print(char text[], int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size)
{
    int16_t offset = size * CHARACTER_BASE_WIDTH;

    for (uint16_t i = 0; i < 40 && text[i] != NULL; i++)
    {
        DrawChar(x + (offset * i), y, text[i], color, bg, size);
    }
}

void ILI9341::Image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data, uint32_t size)
{
    uint32_t n = size;

    SetCursorPosition(x, y, w + x - 1, h + y - 1);

    for (uint32_t i = 0; i < n; i++)
    {
        SendData(data[i] >> 8);
        SendData(data[i] & 0xff);
    }
}

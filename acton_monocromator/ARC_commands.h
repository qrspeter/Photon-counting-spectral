// ARC_commands.h - строковые команды управления монохроматором.

#ifndef __ARC_COMMANDS_H
#define __ARC_COMMANDS_H

#include "stdafx.h"

const char ARC_INPUT_BUFF_SIZE=16;


//-----------байтовые КОМАНДЫ счетчика.-----------------
// очистка буфера перед каждой командой.
static	char ARC_CR[]={0x0D};  //  0x20 - возврат каретки.
static	char ARC_ANSWER[]={0x20,0x20,0x6F,0x6B,0x0D,0x0A}; // Стандартный ответ счетчика.
static	char ARC_SET_WAVELENGHT[]={0x20,'G','O','T','O',0x0D}; // команда перехода на определенную длину волны.
static	char ARC_SET_GRATING[]={0x20,'G','R','A','T','I','N','G',0x0D}; //установка определенной решетки.
static	char ARC_SET_MIRROR_FRONT[]={'F','R','O','N','T',0x0D};
static	char ARC_SET_MIRROR_SIDE[]={'S','I','D','E',0x0D};
static	char ARC_SELECT_FRONT_SLIT[]={'F','R','O','N','T','-','E','X','I','T','-','S','L','I','T',0x0D}; // выбор щели.
static	char ARC_SELECT_SIDE_SLIT[]={'S','I','D','E','-','E','X','I','T','-','S','L','I','T',0x0D}; // боковая щель.
static	char ARC_SET_SLIT_WIDTH[]={'M','I','C','R','O','N','S',0x0D};


// Массивы для вывода данных в виде строк (или их по функциям распихать???):
// 1. Количество циклов сканирования.
static	char _waveLenght[]={'0','0','0','.','0','0','0'}; // Длина волны в нанометрах, по три знака до и после запятой.

// Длина волны лазеров.
static double LASER_WAVELENGHT_RED_633=632.7;
static double LASER_WAVELENGHT_GREEN_532=532.1;



#endif
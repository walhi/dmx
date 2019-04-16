#define WORK_ST           0  // Основной режим работы (реакция реле на состояния DMX каналов)
#define TO_WORK_ST        1  // Подготовка к основному режиму работы

#define TO_CFG_ST         2  // Подготовка к конфигурации

#define ERROR_ADR_ST      3  // Ошибка адреса (не используется)

#define SAVE_ST           5  // Сохранение настроек в EEPROM

#define TO_CH1_CFG_ST     10 // Вывод номера канала (реле)
#define TO_CH1_ADR_CFG_ST 11 // Вывод сообщения (редактирование DMX адреса)
#define CH1_ADR_CFG_ST    12 // Редактирование DMX адреса
#define TO_CH1_VAL_CFG_ST 13 // Вывод сообщения (редактирование порогового значения)
#define CH1_VAL_CFG_ST    14 // Редактирование порогового значения

#define TO_CH2_CFG_ST     20
#define TO_CH2_ADR_CFG_ST 21
#define CH2_ADR_CFG_ST    22
#define TO_CH2_VAL_CFG_ST 23
#define CH2_VAL_CFG_ST    24

#define TO_CH3_CFG_ST     30
#define TO_CH3_ADR_CFG_ST 31
#define CH3_ADR_CFG_ST    32
#define TO_CH3_VAL_CFG_ST 33
#define CH3_VAL_CFG_ST    34

#define TO_CH4_CFG_ST     40
#define TO_CH4_ADR_CFG_ST 41
#define CH4_ADR_CFG_ST    42
#define TO_CH4_VAL_CFG_ST 43
#define CH4_VAL_CFG_ST    44

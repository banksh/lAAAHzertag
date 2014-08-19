#ifndef __MAIN_H
#define	__MAIN_H

#define FLASH_CONFIG 0x7D0
#define FLASH_HITLIST 0x7E0

typedef struct {
    uint16_t id;
    uint16_t power;
    uint16_t health;
    uint16_t shield;
    uint16_t respawn_delay;
    uint16_t fire_threshold;
    uint16_t fire_cheating;
    uint16_t fire_holdoff;
} config_t;

#define CONFIG_SIZE (sizeof(config_t)/2)

typedef uint16_t hitlist_t[16];

#define HITLIST_SIZE (sizeof(hitlist_t)/2)

extern config_t config;
extern hitlist_t hitlist;

#endif


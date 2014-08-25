CONFIG_ID=0
CONFIG_POWER=1
CONFIG_HEALTH=2
CONFIG_SHIELD=3
CONFIG_RESPAWN_DELAY=4
CONFIG_FIRE_THRESHOLD=5
CONFIG_FIRE_CHEATING=6
CONFIG_FIRE_HOLDOFF=7
CONFIG_DEATH_PERIOD=8

FLASH_CONFIG = 0x7F0
FLASH_HITLIST = 0x7E0

ID_UNINIT = 0x80

class Config(object):
    def __init__(self, gun_id=0x80, power=0, health=10, shield=2, respawn_delay=100,
                 fire_threshold=500, fire_cheating=1000, fire_holdoff=1000,
                 death_period=3000):
        self.gun_id = gun_id
        self.power = power
        self.health = health
        self.shield = shield
        self.respawn_delay = respawn_delay
        self.fire_threshold = fire_threshold
        self.fire_cheating= fire_cheating
        self.fire_holdoff= fire_holdoff
        self.death_period= death_period

    def page(self):
        cfg = [0x3FFF] * 16
        cfg[CONFIG_ID]= self.gun_id
        cfg[CONFIG_POWER] = self.power
        cfg[CONFIG_HEALTH]= self.health
        cfg[CONFIG_SHIELD]= self.shield
        cfg[CONFIG_RESPAWN_DELAY] = self.respawn_delay
        cfg[CONFIG_FIRE_THRESHOLD] = self.fire_threshold
        cfg[CONFIG_FIRE_CHEATING] = self.fire_cheating
        cfg[CONFIG_FIRE_HOLDOFF] = self.fire_holdoff
        cfg[CONFIG_DEATH_PERIOD] = self.death_period
        return cfg

    def dump(self):
        attrs = ["gun_id", "power", "health", "shield", "respawn_delay", "fire_threshold", "fire_cheating", "fire_holdoff", "death_period"]
        output = {}
        for at in attrs:
            output[at] = getattr(self, at)
        return output

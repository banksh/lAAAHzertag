#!/usr/bin/python
import db
import gun
import os
import IPython

DATABASE = os.path.join(os.path.dirname(os.path.abspath(__file__)),'guns.db')
d=db.Database(DATABASE)

def upperclassman(athena):
        gun_id = d.get_gun_from_name(athena)
        if gun_id is None:
                return "No gun found"
        raw_config = d.read_config(gun_id)
        if raw_config is None:
                return "No config found"
        config = gun.Config(**raw_config)
        config.power = 15
        config.fire_cheating = 1025
        d.set_config(gun_id, config.dump())
        return "Upperclassman-mode set!"

def list_guns():
        results = d.c.execute("SELECT active, gun_id, athena FROM guns").fetchall()
        for active, gun_id, athena in results:
                print active, hex(gun_id), athena

def main():
    variables = locals()
    variables.update(globals())
    IPython.start_ipython(user_ns=variables)

if __name__ == "__main__":
    main()

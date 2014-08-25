#!/usr/bin/env python
import cPickle as pickle
import json
import sqlite3 as sq

def db_from_pickle(pickle_file, db_file):
    db = Database(filename=db_file)
    raw_data = pickle.load(open(pickle_file))
    for gun_id, name, hits in raw_data:
        db.confirm_id(gun_id)
        db.add_name(gun_id, name)
        for hit in hits:
            db.add_hit(gun_id, hit)
    db.close()

class Database:
    def __init__(self, filename="guns.db"):
        self.conn = sq.connect(filename)
        self.c = self.conn.cursor()
        self.setup_db()

    def setup_db(self):
        self.c.execute('''CREATE TABLE IF NOT EXISTS guns (
                    id integer PRIMARY KEY ASC,
                    gun_id integer,
                    athena text,
                    config text,
                    time_last_seen timestamp DEFAULT CURRENT_TIMESTAMP,
                    time_created timestamp DEFAULT CURRENT_TIMESTAMP,
                    active boolean
                  )''')
        self.c.execute('''CREATE TABLE IF NOT EXISTS hits (
                    id integer PRIMARY KEY ASC,
                    shooter_id integer,
                    victim_id integer,
                    time timestamp DEFAULT CURRENT_TIMESTAMP
                  )''')
    def new_gun_id(self):
        m = self.c.execute('SELECT gun_id FROM guns').fetchall()
	ids = {x[0] for x in m}
        v = 0x81
        while v in ids:
            v += 1
        assert v < 0xFF
        return v 
    def confirm_id(self, val, config=None):
        if config is None:
            config = {}
        json_config = json.dumps(config)
        self.c.execute("INSERT INTO guns (gun_id, config, active)  VALUES(?, ?, 1)", (int(val), json_config))
        self.conn.commit()
    def set_inactive(self, gun_id):
        self.c.execute("UPDATE guns SET active=0 WHERE gun_id=?", (gun_id,))
        self.conn.commit()
    def set_active(self, gun_id):
        self.c.execute("UPDATE guns SET active=1 WHERE gun_id=?", (gun_id,))
        self.conn.commit()
    def add_name(self, gun_id, name):
        self.c.execute("UPDATE guns SET athena=? WHERE gun_id=?", (name, gun_id,))
        self.conn.commit()
    def get_name_from_gun(self, gun_id):
        result = self.c.execute("SELECT athena FROM guns WHERE gun_id=?", (gun_id,)).fetchone()
        if result is None:
            return result
        return result[0]
    def get_gun_from_name(self, athena):
        result = self.c.execute("SELECT gun_id FROM guns WHERE athena=?", (athena,)).fetchone()
        if result is None:
            return result
        return result[0]
    def read_config(self, gun_id):
        p = self.c.execute("SELECT config from guns WHERE gun_id=?",(gun_id,)).fetchone()
        if result is None:
            return result
        return json.loads(result[0])
    def set_config(self, gun_id, config):
        self.c.execute("UPDATE guns SET config=? WHERE gun_id=?", (json.dumps(config),gun_id,))
        self.conn.commit()
    def read_config(self, gun_id):
        p = self.c.execute("SELECT config_to_write from guns WHERE gun_id=?",(gun_id,)).fetchone()
        if result is None:
            return result
        return json.loads(result[0])
    def read_hits(self, gun_id):
        return self.c.execute("""SELECT guns.gun_id, guns.athena, COUNT(hits.victim_id) AS score
                                 FROM guns LEFT OUTER JOIN hits ON hits.victim_id = guns.gun_id 
                                 WHERE hits.shooter_id=? 
                                 GROUP BY hits.victim_id 
                                 ORDER BY score DESC""",(gun_id,)).fetchall()
    def read_hits_by(self, gun_id):
        return self.c.execute("""SELECT guns.gun_id, guns.athena, COUNT(hits.shooter_id) AS score 
                                 FROM guns LEFT OUTER JOIN hits ON hits.shooter_id = guns.gun_id 
                                 WHERE hits.victim_id=? 
                                 GROUP BY hits.shooter_id 
                                 ORDER BY score DESC""",(gun_id,)).fetchall()
    def add_hit(self, shooter_id, victim_id):
        self.c.execute("INSERT INTO hits (shooter_id, victim_id)  VALUES(?, ?)", (shooter_id, victim_id))
        self.conn.commit()
    def high_scores(self):
        return self.c.execute("""SELECT guns.gun_id, guns.athena, COUNT(hits.shooter_id) AS score 
                                 FROM guns LEFT OUTER JOIN hits ON hits.shooter_id = guns.gun_id 
                                 GROUP BY hits.shooter_id 
                                 ORDER BY score DESC""").fetchall()
    def most_hit(self):
        return self.c.execute("""SELECT guns.gun_id, guns.athena, COUNT(hits.victim_id) AS score 
                                 FROM guns LEFT OUTER JOIN hits ON hits.victim_id = guns.gun_id 
                                 GROUP BY hits.victim_id 
                                 ORDER BY score DESC""").fetchall()
    def close(self):
        self.conn.close()

class RAMDatabase:
    def __init__(self):
        self.gun_id=0x81
        self.names={}

    def new_gun_id(self):
        return self.gun_id

    def confirm_new_gun_id(self):
        self.gun_id += 1

    def add_new_name(self,gun_id,name):
        self.names[gun_id]=name

    def get_name_from_gun(self,gun_id):
        if gun_id in self.names:
            return self.names[gun_id]
        else:
            return None
if __name__ == '__main__':
    db = Database()
    db.blacklist.append(149)

    n = db.new_gun_id()
    db.confirm_id(n)
    db.add_name(n, "maxj")
    db.set_config(n, {"PowerLevel":9000})
    db.set_hits(n, [0x81])

    print db.read_config(n)
    print db.read_hits(n)
    print db.get_name_from_gun(n)

    db.close()
    

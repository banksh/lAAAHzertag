#!/usr/bin/env python
from __future__ import absolute_import

import cPickle as pickle
import db
import os

from flask import Flask, request, render_template, g, abort
app = Flask(__name__)

DATABASE = os.path.join(os.path.dirname(os.path.abspath(__file__)),'guns.db')

def get_db():
    gdb = getattr(g, '_database', None)
    if gdb is None:
        gdb = g._database = db.Database(DATABASE)
    return gdb

# Web Interfaces
@app.route("/")
def index():
    gdb = get_db()
    high_scores = gdb.high_scores()
    most_hit = gdb.most_hit()
    return render_template("index.html", **locals())

@app.route("/instructions.html")
def instructions():
    return render_template("instructions.html")

@app.route("/gun/id/<int:gun_id>")
def gun_by_id(gun_id, athena=None):
    gdb = get_db()
    if athena is None:
        athena = gdb.get_name_from_gun(gun_id)
    if athena is None:
        abort(404)
    hits = gdb.read_hits(gun_id)
    hit_by = gdb.read_hits_by(gun_id)
    return render_template("gun.html", **locals())

@app.route("/gun/name/<athena>")
def gun_by_athena(athena):
    gdb = get_db()
    gun_id = gdb.get_gun_from_name(athena)
    if gun_id is None:
        abort(404)
    return gun_by_id(gun_id, athena=athena)


if __name__ == "__main__":
    app.run(debug=True,host='')

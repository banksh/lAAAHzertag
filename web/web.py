#!/usr/bin/env python
from __future__ import absolute_import

import cPickle as pickle
import db

from flask import Flask, request, render_template
app = Flask(__name__)

# Database Functions
class :tab
def get_raw_gun_data():
    return pickle.load(open("gun_data.p"))

# Web Interfaces
@app.route("/")
def index():
    return render_template("index.html")

@app.route("/instructions.html")
def instructions():
    return render_template("instructions.html")


if __name__ == "__main__":
    app.run()

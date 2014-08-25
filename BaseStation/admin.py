#!/usr/bin/python
import db
import gun
import os
import IPython

DATABASE = os.path.join(os.path.dirname(os.path.abspath(__file__)),'guns.db')
d=db.Database(DATABASE)

def main():
    variables = locals()
    variables.update(globals())
    IPython.start_ipython(user_ns=variables)

if __name__ == "__main__":
    main()

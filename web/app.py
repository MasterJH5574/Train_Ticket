# filename app.py

import client
from flask import Flask, render_template, session, redirect, url_for, request

app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'


def get_privilege(user_id):
    return 1


@app.route("/")
def initial():
    current_user = session.get('user_id')
    app.logger.debug(current_user)
    if current_user:
        return render_template("main_page.html",
                               user_id=current_user,
                               administrator=get_privilege(current_user)
                               )
    else:
        return render_template("main_page.html",
                               user_id=current_user  # == None
                               )


@app.route("/login")
def login():
    current_user = session.get('user_id')
    app.logger.debug(current_user)
    if current_user:
        # alart 已登录
        return 1 #TODO alart & return to main page
    else:
        return render_template("login.html",
                               user_id=current_user
                               )


@app.route("/user_login", methods=['POST'])  # method=['GET', 'POST']
def user_login():
    if request.method == 'POST':
        element = ('user_id', 'password')

        for item in element:
            if not request.form.has_key(item):
                return ""

        res = client.login(request.form['user_id'], request.form['password'])
        if res == "Success\n":
            session['user_id'] = request.form['user_id']
            return '1'
        else:
            return '0'


@app.route("/register")
def register():
    current_user = session.get('user_id')
    if current_user:
        # alart 已登录
        return 1  #TODO alart & return to main page
    else:
        return render_template("register.html",
                               user_id=current_user
                               )


@app.route("/user_register", methods=['POST'])
def user_register():
    return 0


if __name__ == '__main__':
    # app.config.from_object('config')
    app.run(host="127.0.0.1", port=5000, debug=True)

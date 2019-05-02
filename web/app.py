# filename app.py

import client
from flask import Flask, render_template, session, redirect, url_for, request, flash

app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'


def get_privilege(user_id):
    return 1


@app.route("/")
def main_page():
    current_user = session.get('username')
    return render_template("main_page.html", username=current_user)


@app.route("/login", methods=['GET', 'POST'])
def login():
    if request.method == 'GET':
        current_user = session.get('username')
        if current_user:
            flash("info 请勿重复登录！ 您已登录，请勿重复登录")
            return redirect(url_for('main_page'))
        else:
            return render_template("login.html", username=None)

    else:
        element = ('user_id', 'password')

        for item in element:
            if item not in request.form:
                flash("danger 登录失败！ 未知登录错误")
                return render_template("login.html", username=None)

        res = client.login(request.form['user_id'], request.form['password'])
        if res == "1":
            res = client.query_profile(request.form['user_id']).split(' ')
            session['username'] = res[0]
            return redirect(url_for('main_page'))
        else:
            flash("danger 登录失败！ 用户ID或密码错误")
            return render_template("login.html", username=None)


@app.route("/register", methods=['GET', 'POST'])
def register():
    if request.method == 'GET':
        current_user = session.get('username')
        if current_user:
            flash("danger 注册失败！ 您已登录，请先登出后再进行注册")
            return redirect(url_for("main_page"))
        else:
            return render_template("register.html", username=None)

    else:
        element = ('username', 'password', 'email', 'phone')

        for item in element:
            if item not in request.form:
                flash("danger 注册失败！ 未知注册错误")
                return render_template("register.html", username=None)

        res = client.register(request.form['username'],
                              request.form['password'],
                              request.form['email'],
                              request.form['phone']
                              )
        current_user = int(res)

        if current_user != -1:
            session['username'] = request.form['username']
            flash("success 注册成功！ 您的用户ID是{}，请牢记此ID，本系统只能使用ID登录。请尽情使用吧~".format(current_user))
            return redirect(url_for("main_page"))
        else:
            flash("danger 注册失败！ 未知注册错误")
            return render_template("register.html", username=None)


@app.route("/logout")
def logout():
    current_user = session.get('username')
    if current_user:
        flash("success 登出成功！")
        session.pop('username')
    else:
        flash("danger 登出失败！ 未登录，焉登出？")
    return redirect(url_for("main_page"))


if __name__ == '__main__':
    # app.config.from_object('config')
    app.run(host="127.0.0.1", port=5000, debug=True)

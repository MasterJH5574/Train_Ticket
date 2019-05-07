# filename app.py

import client
from flask import Flask, render_template, session, redirect, url_for, request, flash

app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'


def get_privilege(user_id):
    res = client.query_profile(user_id).split(' ')
    return int(res[3])


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
            flash("success 登录成功！ 已自动跳转到首页")
            res = client.query_profile(request.form['user_id']).split(' ')
            session['username'] = res[0]
            session['user_id'] = int(request.form['user_id'])
            session['privilege'] = int(get_privilege(request.form['user_id']))
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
            session['user_id'] = current_user
            session['username'] = request.form['username']
            session['privilege'] = get_privilege(current_user)
            flash("success 注册成功！ 您的用户ID是{}，请牢记此ID，本系统只能使用ID登录。请尽情使用吧~".format(current_user))
            return redirect(url_for("main_page"))
        else:
            flash("danger 注册失败！ 未知注册错误")
            return redirect("register.html", username=None)


@app.route("/logout")
def logout():
    current_user = session.get('username')
    if current_user:
        session.clear()
        flash("success 登出成功！ 已自动跳转到首页")
    else:
        flash("danger 登出失败！ 未登录，焉登出？")
    return redirect(url_for("main_page"))


@app.route("/profile")
def profile():
    current_user_id = session.get('user_id')
    if not current_user_id:
        flash("danger 尚未登录！ 请登录后再进行操作")
        return redirect(url_for("login"))

    user_id = int(request.args.get('id', str(current_user_id)))
    current_privilege = session.get('privilege')
    privileges = ('不存在', '普通用户', '管理员')

    if current_user_id == user_id:
        res = client.query_profile(user_id).split(' ')
        return render_template("profile.html",
                               user_id=session.get('user_id'),
                               username=session.get('username'),
                               privilege=current_privilege,
                               user_query_id=user_id,
                               user_query_name=res[0],
                               user_query_email=res[1],
                               user_query_phone=res[2],
                               user_privilege=privileges[int(res[3])]
                               )
    elif current_privilege == 2:
        res = client.query_profile(user_id).split(' ')
        if res[0] == "0":
            flash("danger 查询错误！ 未找到该用户")
            return redirect(url_for("profile"))
        else:
            return render_template("profile.html",
                                   user_id=session.get('user_id'),
                                   username=session.get('username'),
                                   privilege=2,
                                   user_query_id=user_id,
                                   user_query_name=res[0],
                                   user_query_email=res[1],
                                   user_query_phone=res[2],
                                   user_privilege=privileges[int(res[3])]
                                   )
    else:
        flash("danger 没有权限！ 您没有权限查看他人的信息")
        return redirect(url_for("profile"))


@app.route("/modify_profile")
def modify_profile():
    current_user_id = session.get('user_id')
    if not current_user_id:
        flash("danger 尚未登录！ 请登录后再进行操作")
        return redirect(url_for("login"))



if __name__ == '__main__':
    # app.config.from_object('config')
    app.run(host="127.0.0.1", port=5000, debug=True)

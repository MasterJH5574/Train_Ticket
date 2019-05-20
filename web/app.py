# filename app.py

import client
from flask import Flask, render_template, session, redirect, url_for, request, flash, jsonify

app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'


@app.route("/")
def main_page():
    return render_template("main_page.html",
                           user_id=session.get('user_id'),
                           username=session.get('username'),
                           privilege=session.get('privilege')
                           )


@app.route("/login", methods=['GET', 'POST'])
def login():
    if request.method == 'GET':
        current_user = session.get('username')
        if current_user:
            flash("info 请勿重复登录！ 您已登录，请勿重复登录")
            return redirect(url_for('main_page'))
        else:
            return render_template("login.html",
                                   user_id=None,
                                   username=None,
                                   privilege=None
                                   )

    else:
        element = ('user_id', 'password')

        for item in element:
            if item not in request.form:
                flash("danger 登录失败！ 未知登录错误")
                return redirect(url_for('login'))

        res = client.login(request.form['user_id'], request.form['password'])
        if res == 1:
            flash("success 登录成功！ 已自动跳转到首页")
            res = client.query_profile(request.form['user_id']).split(' ')
            session['username'] = res[0]
            session['user_id'] = int(request.form['user_id'])
            session['privilege'] = int(res[3])
            return redirect(url_for('main_page'))
        else:
            flash("danger 登录失败！ 用户ID或密码错误")
            return redirect(url_for('login'))


@app.route("/register", methods=['GET', 'POST'])
def register():
    if request.method == 'GET':
        current_user = session.get('username')
        if current_user:
            flash("danger 注册失败！ 您已登录，请先登出后再进行注册")
            return redirect(url_for("main_page"))
        else:
            return render_template("register.html",
                                   user_id=None,
                                   username=None,
                                   privilege=None
                                   )

    else:
        element = ('username', 'password_1', 'email', 'phone')

        for item in element:
            if item not in request.form:
                flash("danger 注册失败！ 未知注册错误A")
                return redirect(url_for('register'))

        current_user = client.register(request.form['username'],
                                       request.form['password_1'],
                                       request.form['email'],
                                       request.form['phone']
                                       )

        if current_user != -1:
            session['user_id'] = current_user
            session['username'] = request.form['username']
            session['privilege'] = client.get_privilege(current_user)
            flash("success 注册成功！ 您的用户ID是{}，请牢记此ID，本系统只能使用ID登录。请尽情使用吧~".format(current_user))
            return redirect(url_for("main_page"))
        else:
            flash("danger 注册失败！ 未知注册错误B")
            return redirect(url_for('register'))


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


@app.route("/modify_profile", methods=['POST', 'GET'])
def modify_profile():
    if request.method == 'GET':
        current_user_id = session.get('user_id')
        if not current_user_id:
            flash("danger 尚未登录！ 请登录后再进行操作(error_1)")
            return redirect(url_for("login"))
        else:
            return render_template("modify_profile.html",
                                   user_id=session.get('user_id'),
                                   username=session.get('username'),
                                   privilege=session.get('privilege')
                                   )

    else:
        current_user_id = session.get('user_id')
        if not current_user_id:
            flash("danger 尚未登录！ 请登录后再进行操作(error_2)")
            return redirect(url_for("login"))

        old_password = request.form['old_password']
        username = request.form['username']
        new_password = request.form['new_password']
        email = request.form['email']
        phone = request.form['phone']

        if old_password != client.query_password(current_user_id):
            flash('danger 旧密码错误！ 请检查您的密码')
            return redirect(url_for('modify_profile'))

        old_profile = client.query_profile(current_user_id).split(' ')
        if username == "":
            username = old_profile[0]
        if new_password == "":
            new_password = old_password
        if email == "":
            email = old_profile[1]
        if phone == "":
            phone = old_profile[2]

        session['username'] = username
        if client.modify_profile(current_user_id, username, new_password, email, phone) == 1:
            flash("success 修改成功！ 信息修改成功")
            return redirect(url_for('main_page'))
        else:
            flash("danger 修改失败！ 未知错误")
            return redirect(url_for('modify_profile'))


@app.route("/modify_privilege", methods=['POST', 'GET'])
def modify_privilege():
    current_user_id = session.get('user_id')
    if not current_user_id:
        flash("danger 尚未登录！ 请登录后再进行操作(error_1)")
        return redirect(url_for("login"))
    if session.get('privilege') != 2:
        flash("danger 没有权限！ 您不是管理员，不能修改用户权限")
        return redirect(url_for('main_page'))

    if request.method == 'GET':
        return render_template("modify_privilege.html",
                               user_id=session.get('user_id'),
                               username=session.get('username'),
                               privilege=session.get('privilege')
                               )
    # method == 'POST'
    user_id = int(request.form['user_id'])
    res = client.modify_privilege(current_user_id, user_id, 2)
    if res == 0:
        flash("danger 操作失败！ 未知错误信息")
    else:
        flash("success 操作成功！ 已成功将用户{}升级为管理员".format(user_id))
    return redirect(url_for('modify_privilege'))


@app.route('/tickets', methods=['POST', 'GET'])
def query_ticket():
    if request.method == 'GET':
        return render_template("query_ticket.html",
                               user_id=session.get('user_id'),
                               username=session.get('username'),
                               privilege=session.get('privilege')
                               )
    else:
        user_id = session.get('user_id')
        if not user_id:
            flash("danger 尚未登录！ 请登录后再进行操作")
            return redirect(url_for('login'))
        train_id = request.form['train_id']
        source = request.form['source']
        terminal = request.form['terminal']
        time_departure = request.form['time_departure']
        seat_list = request.form['seat'].split(' ')
        seat = seat_list[0]
        price = seat_list[1]
        return render_template("buy_confirm.html",
                               user_id=session.get('user_id'),
                               username=session.get('username'),
                               privilege=session.get('privilege'),
                               train_id=train_id,
                               source=source,
                               terminal=terminal,
                               seat=seat,
                               time_departure=time_departure,
                               price=price
                               )


@app.route('/buy_ticket', methods=['POST', 'GET'])
def buy_ticket():
    if request.method == 'GET':
        flash("danger 访问失败！ 请先选择车票")
        return redirect(url_for('main_page'))

    user_id = session.get('user_id')
    if not user_id:
        flash("danger 尚未登录！ 请登录后再进行操作")
        return redirect(url_for('login'))

    train_id = request.form['train_id']
    source = request.form['source']
    terminal = request.form['terminal']
    date = request.form['date']
    seat = request.form['seat'].split(' ')[0]

    res = client.buy_ticket(user_id, train_id, source, terminal, date, seat)
    if res == 1:
        flash("success 购票成功！ 您已成功购票！")
    else:
        flash("danger 购票失败！ 未知错误，请联系管理员或再次尝试操作")
    return redirect(url_for('main_page'))


@app.route('/tickets/query_general_ajax')
def query_general_ajax():
    s = request.args.get("S")
    t = request.args.get("T")
    date = request.args.get("date")
    type_c = request.args.get("type_C") == "true"
    type_d = request.args.get("type_D") == "true"
    type_g = request.args.get("type_G") == "true"
    type_k = request.args.get("type_K") == "true"
    type_o = request.args.get("type_O") == "true"
    type_t = request.args.get("type_T") == "true"
    type_z = request.args.get("type_Z") == "true"

    res = ""
    if type_c:
        res += client.query_ticket(s, t, date, "C")
    if type_d:
        res += client.query_ticket(s, t, date, "D")
    if type_g:
        res += client.query_ticket(s, t, date, "G")
    if type_k:
        res += client.query_ticket(s, t, date, "K")
    if type_o:
        res += client.query_ticket(s, t, date, "O")
    if type_t:
        res += client.query_ticket(s, t, date, "T")
    if type_z:
        res += client.query_ticket(s, t, date, "Z")
    return res


@app.route('/tickets/query_transit_ajax')
def query_transit_ajax():
    s = request.args.get("S")
    t = request.args.get("T")
    date = request.args.get("date")
    type_c = request.args.get("type_C") == "true"
    type_d = request.args.get("type_D") == "true"
    type_g = request.args.get("type_G") == "true"
    type_k = request.args.get("type_K") == "true"
    type_o = request.args.get("type_O") == "true"
    type_t = request.args.get("type_T") == "true"
    type_z = request.args.get("type_Z") == "true"

    res = ""
    if type_c:
        res += client.query_transfer(s, t, date, "C")
    if type_d:
        res += client.query_transfer(s, t, date, "D")
    if type_g:
        res += client.query_transfer(s, t, date, "G")
    if type_k:
        res += client.query_transfer(s, t, date, "K")
    if type_o:
        res += client.query_transfer(s, t, date, "O")
    if type_t:
        res += client.query_transfer(s, t, date, "T")
    if type_z:
        res += client.query_transfer(s, t, date, "Z")
    return res


@app.route('/my_ticket', methods=["GET", "POST"])
def my_ticket():
    user_id = session.get("user_id")
    if not user_id:
        flash("danger 尚未登录！ 请登录后再进行操作")
        return redirect(url_for('login'))

    if request.method == "GET":
        return render_template("my_ticket.html",
                               user_id=session.get('user_id'),
                               username=session.get('username'),
                               privilege=session.get('privilege')
                               )

    train_id = request.form['train_id']
    source = request.form['source']
    terminal = request.form['terminal']
    date = request.form['time'].split(" ")[0]
    seat = request.form['seat']

    res = client.refund_ticket(user_id, train_id, source, terminal, date, seat)
    if res == 1:
        flash("success 退票成功！ 您已成功退票")
    else:
        flash("danger 退票失败！ 未知错误")
    return redirect(url_for("main_page"))


@app.route('/my_ticket/ajax')
def my_ticket_ajax():
    user_id = session.get("user_id")
    if not user_id:
        flash("danger 尚未登录！ 请登录后再进行操作")
        return redirect(url_for('login'))

    date = request.args.get("date")
    type_c = request.args.get("type_C") == "true"
    type_d = request.args.get("type_D") == "true"
    type_g = request.args.get("type_G") == "true"
    type_k = request.args.get("type_K") == "true"
    type_o = request.args.get("type_O") == "true"
    type_t = request.args.get("type_T") == "true"
    type_z = request.args.get("type_Z") == "true"

    res = ""
    if date == "":
        if type_c:
            res += client.query_order_all(user_id, "C")
        if type_d:
            res += client.query_order_all(user_id, "D")
        if type_g:
            res += client.query_order_all(user_id, "G")
        if type_k:
            res += client.query_order_all(user_id, "K")
        if type_o:
            res += client.query_order_all(user_id, "O")
        if type_t:
            res += client.query_order_all(user_id, "T")
        if type_z:
            res += client.query_order_all(user_id, "Z")
    else:
        if type_c:
            res += client.query_order(user_id, date, "C")
        if type_d:
            res += client.query_order(user_id, date, "D")
        if type_g:
            res += client.query_order(user_id, date, "G")
        if type_k:
            res += client.query_order(user_id, date, "K")
        if type_o:
            res += client.query_order(user_id, date, "O")
        if type_t:
            res += client.query_order(user_id, date, "T")
        if type_z:
            res += client.query_order(user_id, date, "Z")
            
    app.logger.debug(res)
    return res


if __name__ == '__main__':
    # app.config.from_object('config')
    app.run(host="127.0.0.1", port=5000, debug=True)

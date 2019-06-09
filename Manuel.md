# 开发文档

------

## 一、模块划分图
![avatar](https://raw.githubusercontent.com/MasterJH5574/Train_Ticket/master/%E5%AF%BC%E5%9B%BE2.png)
## 二、函数功能
### user类
| 返回类型        |调用方法   |  描述  |
| :--------: | :-----:  | :----:  |
| 无返回     | user() |   构造函数    |
| 无返回        |   ~user()   |   析构函数   |
| user        |    read_user(const int _id)    |  读入用户 |
| 无返回        |    save_user(user &tempuser)    |  将用户信息存入文件 |
| 无返回       |    save_info()    |  将用户个数存入文件 |
| 0失败/1成功       |    delete_file()    | 删除用户文件 |
| 用户id      |    user_register(is,os)    | 注册用户 |
|0失败/1成功      |    user_login(is,os)    | 用户登录 |
|无返回     |    query_user(is,os)    | 用户信息查询 |
|0失败/1成功  |   modify_user(is,os)    | 用户信息修改 |
|0失败/1成功  |   modify_user_priv(is,os)    | 用户权限修改 |
### train类
| 返回类型        |调用方法   |  描述  |
| :--------: | :-----:  | :----:  |
| 无返回     | train() |   构造函数    |
| 无返回     | ~train() |   析构函数    |
### ticket类
| 返回类型        |调用方法   |  描述  |
| :--------: | :-----:  | :----:  |
| 无返回     | ticket() |   构造函数    |
| 无返回     | ~ticket() |   析构函数    |
### TrainManager类
| 返回类型        |调用方法   |  描述  |
| :--------: | :-----:  | :----:  |
| 无返回     | TrainManager() |   构造函数    |
| 无返回     | ~TrainManager() |   析构函数    |
|    0失败/1成功  | add_train(is,os) |   加入车次    |
|    0失败/1成功  | delete_train(is,os) |   删除车次    |
|    0失败/1成功  | modify_train(is,os) |   修改车次信息    |
|    0失败/1成功  | sale_train(is,os) |   车次开放售票    |
|    无返回  | query_train(is,os) |   查询车次信息    |
|    0失败/1成功  | buy_ticket(is,os) |   购票    |
|    0失败/1成功  | query_ticket(is,os) |   查询两站之间车票信息    |
|    无返回  | query_transfer(is,os) |   查询两站之间带中转车票信息    |
|    0失败/1成功  | refund_ticket(is,os) |   退票 |
|   无返回  | query_order(is,os) |   查询购票信息 |

## 三、文件设计
| 文件名称        |功用   |
| :--------: | :-----:  | 
| train_record    | 记录车次信息（总）|  
| station_record   | 记录车站信息|  
|ticket_record    | 记录车票信息|  
|route    | 记录station列表 |  
|ticket_price   | 记录票价信息|  
|ticket_left    | 记录余票信息|  
|user_data    | 记录用户信息|  


## 四、使用手册与系统安装手册
### 关于用户(注册、登录、查询个人信息、修改个人信息、查询用户信息、修改用户权限)
![avatar](https://github.com/MasterJH5574/Train_Ticket/blob/master/gif/user.gif)
### 关于车票（查询车票、带中转查询车票、购票、查询购票信息、退票）
![avatar](https://github.com/MasterJH5574/Train_Ticket/blob/master/gif/ticket.gif)
### 关于车次（新建车次、公开车次、查询车次、修改车次、删除车次）
![avatar](https://github.com/MasterJH5574/Train_Ticket/blob/master/gif/train.gif)

## 五、特色功能

### 关于用户
* 快捷查询上海天气预报，单击后可查询详细天气预报。
* 快捷查询车票，不用登陆一键查询。
* 一言窗口，刷出惊喜。
### 关于车票
* 注册时验证密码强度验证：Too weak、Could be stronger、Strong password。
* 图案验证码，刷新可切换图片。
* 查询个人信息时快捷跳转到修改信息页面。
* 用户登录成功后根据权限更新页面导航栏。
* 实时检测注册和登录提交的信息。
* 找回密码，找到惊喜。
### 关于车次
* 按钮动态添加/删除座位类型，不需要用户输入座位类型数，实时检测并更新座位类型数目。
* 按钮动态添加车站，批量删除车站，不需要用户输入车站数，实时检测并更新车站数目。
* 支持同时添加车站和座位类型：添加/删除座位类型后，不需要重新生成车站信息表格，会自动更新车站表格，自动添加/删除相应的座位类型价格列。
* 智能纠错：用户提交信息后，会自动验证车站数目，座位数目等信息是否合法。
* 查询车次时，如果车站数目太多，会自动换页。
* 支持excel导出当前页面车站信息/所有车站信息。

## 六、组员工作情况

| 姓名        |任务   |
| :--------: | :-----:  | 
| 郭林松    | |  
| 郭睿涵   | 后端（除bptree）|  
|计家宝    | |   
|赖睿航    | |  


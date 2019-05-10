$().ready(function() {
    $("#login_form").validate({
        errorClass: "form_error",
        rules: {
            user_id: {
                required: true,
                digits: true,
                min: 1,
                maxlength: 20
            },
            password: {
                required: true,
                minlength: 6,
                noSpace: true,
                maxlength: 16,
            }
        },
        messages: {
            user_id: {
                required: "请输入用户ID",
                digits: "用户ID必须为正整数",
                min: "用户ID必须为正整数",
                maxlength: "用户ID长度不能超过20位"
            },
            password: {
                required: "请输入密码",
                minlength: "密码长度不得低于6位",
                noSpace: "密码不得包含空格",
                maxlength: "密码长度不能超过16位"
            }
        }
    })
});

$().ready(function() {
    $("#modal_login_form").validate({
        errorClass: "form_error",
        rules: {
            user_id: {
                required: true,
                digits: true,
                min: 1,
                maxlength: 20
            },
            password: {
                required: true,
                minlength: 6,
                noSpace: true,
                maxlength: 16,
            }
        },
        messages: {
            user_id: {
                required: "请输入用户ID",
                digits: "用户ID必须为正整数",
                min: "用户ID必须为正整数",
                maxlength: "用户ID长度不能超过20位"
            },
            password: {
                required: "请输入密码",
                minlength: "密码长度不得低于6位",
                noSpace: "密码不得包含空格",
                maxlength: "密码长度不能超过16位"
            }
        }
    })
});

$().ready(function() {
    $("#register_form").validate({
        errorClass: "form_error",
        rules: {
            username: {
                required: true,
                noSpace: true,
                minlength: 2,
                byteRangeLength: 20
            },
            password_1: {
                required: true,
                minlength: 6,
                noSpace: true,
                byteRangeLength: 16
            },
            password_2: {
                required: true,
                minlength: 6,
                noSpace: true,
                byteRangeLength: 16,
                equalTo: "#password_1"
            },
            email: {
                required: true,
                email: true,
                byteRangeLength: 50
            },
            phone: {
                required: true,
                digits: true,
                byteRangeLength: 20
            }
        },
        messages: {
            username: {
                required: "请输入用户名",
                noSpace: "用户名不得包含空格",
                minlength: "用户名长度不得低于2位",
                byteRangeLength: "用户名长度不得超过20位"
            },
            password_1: {
                required: "请输入密码",
                minlength: "密码长度不得低于6位",
                noSpace: "密码不得包含空格",
                byteRangeLength: "密码长度不能超过16位"
            },
            password_2: {
                required: "请再次输入密码",
                minlength: "密码长度不得低于6位",
                noSpace: "密码不得包含空格",
                byteRangeLength: "密码长度不能超过16位",
                equalTo: "两次密码输入不一致"
            },
            email: {
                required: "请输入您的电子邮箱",
                email: "请输入正确的邮箱格式",
                byteRangeLength: "邮箱长度不能超过50位"
            },
            phone: {
                required: "请输入您的电话号码",
                digits: "电话号码格式不正确",
                byteRangeLength: "电话号码长度不能超过20位"
            }
        }
    })
});

$().ready(function() {
    $("#query_profile_form").validate({
        errorClass: "form_error",
        rules: {
            query_id: {
                digits: true,
                min: 1,
                maxlength: 20
            }
        },
        messages: {
            query_id: {
                digits: "用户ID必须为正整数",
                min: "用户ID必须为正整数",
                maxlength: "用户ID长度不能超过20位"
            }
        }
    })
});

$().ready(function() {
    $("#profile_form").validate({
        errorClass: "form_error",
        rules: {
            username: {
                noSpace: true,
                minlength: 2,
                byteRangeLength: 20
            },
            old_password: {
                required: true,
                minlength: 6,
                noSpace: true,
                byteRangeLength: 16
            },
            new_password: {
                minlength: 6,
                noSpace: true,
                byteRangeLength: 16
            },
            email: {
                email: true,
                byteRangeLength: 50
            },
            phone: {
                digits: true,
                byteRangeLength: 20
            }
        },
        messages: {
            username: {
                noSpace: "用户名不得包含空格",
                minlength: "用户名长度不得低于2位",
                byteRangeLength: "用户名长度不得超过20位"
            },
            old_password: {
                required: "请输入旧密码",
                minlength: "密码长度不得低于6位",
                noSpace: "密码不得包含空格",
                byteRangeLength: "密码长度不能超过16位"
            },
            new_password: {
                minlength: "密码长度不得低于6位",
                noSpace: "密码不得包含空格",
                byteRangeLength: "密码长度不能超过16位"
            },
            email: {
                email: "请输入正确的邮箱格式",
                byteRangeLength: "邮箱长度不能超过50位"
            },
            phone: {
                digits: "电话号码格式不正确",
                byteRangeLength: "电话号码长度不能超过20位"
            }
        }
    })
});

$().ready(function() {
    $("#modify_privilege_form").validate({
        errorClass: "form_error",
        rules: {
            user_id: {
                required: true,
                digits: true,
                min: 1,
                maxlength: 20
            }
        },
        messages: {
            user_id: {
                required: "请填写用户ID",
                digits: "用户ID必须为正整数",
                min: "用户ID必须为正整数",
                maxlength: "用户ID长度不能超过20位"
            }
        }
    })
});
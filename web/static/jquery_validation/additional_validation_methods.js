function checkbyteLength(str) {
    var length = 0;
    var ch;
    for (var i = 0; i < str.length; i++) {
        ch = str.charCodeAt(i);
        if (ch < 0x007f)
            length++;
        else if (ch >= 0x0080 && ch <= 0x07ff)
            length += 2;
        else if (ch >= 0x0800 && ch <= 0xffff)
            length += 3;
    }
    return length;
}

jQuery.validator.addMethod("byteRangeLength", function(value, element, param) {
    var length = checkbyteLength(value);
    return this.optional(element) || length <= param;
}, "字符串长度超出限制");

jQuery.validator.addMethod("noSpace", function(value, element) {
    var tel = / /;
    return this.optional(element) || !(tel.test(value));
}, "不能包含空格");